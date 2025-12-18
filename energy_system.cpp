#include "energy_system.h"

void EnergyOptimizationSystem::addDevice() {
    char id[50], name[50];
    float rate;
    int critical, priority;
    
    cout << "\n--- Add New Device ---" << endl;
    cout << "Device ID: ";
    cin >> id;
    cout << "Device Name: ";
    cin.ignore();
    cin.getline(name, 50);
    cout << "Consumption Rate (Watts): ";
    cin >> rate;
    cout << "Is Critical? (1=Yes, 0=No): ";
    cin >> critical;
    
    if (critical) {
        cout << "*** Critical device will automatically get priority 10 ***" << endl;
        priority = 10;
    } else {
        cout << "Priority (1-10): ";
        cin >> priority;
    }
    
    Device* device = new Device(string(id), string(name), rate, critical, priority);
    deviceRegistry.insert(string(id), device);
    deviceCount++;
    
    cout << "Device added successfully!" << endl;
    if (critical) {
        cout << "*** This is a CRITICAL device - it will be protected from load shedding ***" << endl;
    }
}

void EnergyOptimizationSystem::monitorDevices() {
    cout << "\n===== Device Monitoring =====" << endl;
    Device* devices[100];
    int size;
    deviceRegistry.getAllValues(devices, size);
    
    if (size == 0) {
        cout << "No devices registered." << endl;
        return;
    }
    
    float totalConsumption = 0;
    int criticalCount = 0;
    float criticalLoad = 0;
    
    cout << "\nID\t\tName\t\t\tRate(W)\t\tStatus\t\tPriority\tType" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    
    for (int i = 0; i < size; i++) {
        cout << devices[i]->deviceID << "\t\t"
             << devices[i]->deviceName << "\t\t"
             << devices[i]->consumptionRate << "\t\t"
             << devices[i]->status << "\t\t"
             << devices[i]->priority << "\t\t"
             << (devices[i]->isCritical ? "[CRITICAL]" : "[NORMAL]") << endl;
        
        if (devices[i]->status == "ON") {
            totalConsumption += devices[i]->consumptionRate;
            if (devices[i]->isCritical) {
                criticalCount++;
                criticalLoad += devices[i]->consumptionRate;
            }
        }
    }
    
    cout << "\n--- Load Statistics ---" << endl;
    cout << "Total Active Consumption: " << totalConsumption << " W" << endl;
    cout << "Critical Devices Load: " << criticalLoad << " W (" << criticalCount << " devices)" << endl;
    cout << "Non-Critical Load: " << (totalConsumption - criticalLoad) << " W" << endl;
    cout << "Load Capacity: " << maxLoadCapacity << " W" << endl;
    cout << "Load Percentage: " << (totalConsumption / maxLoadCapacity * 100) << "%" << endl;
    
    if (totalConsumption > maxLoadCapacity * 0.9f) {
        cout << "\n*** WARNING: Approaching maximum load capacity! ***" << endl;
        cout << "*** Automatic load shedding may be triggered ***" << endl;
        cout << "*** Critical devices will remain protected ***" << endl;
    }
}

void EnergyOptimizationSystem::toggleDevice() {
    char id[50];
    cout << "\nEnter Device ID: ";
    cin >> id;
    
    Device** device = deviceRegistry.get(string(id));
    if (!device) {
        cout << "Device not found!" << endl;
        return;
    }
    
    if ((*device)->status == "OFF") {
        // Check if turning ON will exceed capacity
        float currentLoad = getCurrentTotalLoad();     // getting the total load of all devices
        float newLoad = currentLoad + (*device)->consumptionRate;
        
        if (newLoad > maxLoadCapacity) {
            cout << "\n*** OVERLOAD WARNING ***" << endl;
            cout << "Current Load: " << currentLoad << " W" << endl;
            cout << "Device Consumption: " << (*device)->consumptionRate << " W" << endl;
            cout << "New Total: " << newLoad << " W" << endl;
            cout << "Capacity: " << maxLoadCapacity << " W" << endl;
            
            if ((*device)->isCritical) {
                cout << "\n*** This is a CRITICAL device ***" << endl;
                cout << "*** Attempting automatic load shedding... ***" << endl;
                
                if (performLoadShedding((*device)->consumptionRate)) {
                    (*device)->turnOn();
                    updateMyHomeConsumption();  // ← NEW
                    cout << (*device)->deviceName << " turned ON (Critical device protected)" << endl;
                } else {
                    cout << "Unable to free enough capacity. Cannot turn on device." << endl;
                }
            } else {
                cout << "\nCannot turn ON - would exceed capacity!" << endl;
                cout << "Suggestion: Turn off some non-critical devices first" << endl;
            }
            return;
        }
        
        (*device)->turnOn();
        updateMyHomeConsumption();  // ← NEW
        cout << (*device)->deviceName << " turned ON." << endl;
        if ((*device)->isCritical) {
            cout << "[CRITICAL device - protected from load shedding]" << endl;
        }
    } else {
        (*device)->turnOff();
        
        int duration = time(0) - (*device)->startTime;
        float units = ((*device)->consumptionRate * (duration / 3600.0f)) / 1000.0f;
        
        HistoryRecord record(
            (*device)->deviceID,
            (*device)->deviceName,
            (*device)->consumptionRate,
            time(0),
            duration,
            units
        );
        
        historyTracker.insertRecord(record);
        
        cout << (*device)->deviceName << " turned OFF." << endl;
        cout << "Energy consumed: " << units << " kWh" << endl;
        
        updateMyHomeConsumption();  // ← NEW
    }
}

float EnergyOptimizationSystem::getCurrentTotalLoad() {
    Device* devices[100];
    int size;
    deviceRegistry.getAllValues(devices, size);
    
    float total = 0;
    for (int i = 0; i < size; i++) {
        if (devices[i]->status == "ON") {
            total += devices[i]->consumptionRate;
        }
    }
    return total;
}

bool EnergyOptimizationSystem::performLoadShedding(float requiredCapacity) {
    cout << "\n--- Initiating Automatic Load Shedding ---" << endl;
    cout << "Need to free: " << requiredCapacity << " W" << endl;
    
    Device* devices[100];
    int size;
    deviceRegistry.getAllValues(devices, size);
    
    // Sort non-critical devices by priority (lowest first)
    Device* nonCritical[100];
    int nonCritCount = 0;
    
    for (int i = 0; i < size; i++) {
        if (devices[i]->status == "ON" && !devices[i]->isCritical) {
            nonCritical[nonCritCount++] = devices[i];
        }
    }
    
    // Bubble sort by priority (ascending)
    for (int i = 0; i < nonCritCount - 1; i++) {
        for (int j = 0; j < nonCritCount - i - 1; j++) {
            if (nonCritical[j]->priority > nonCritical[j + 1]->priority) {
                Device* temp = nonCritical[j];
                nonCritical[j] = nonCritical[j + 1];
                nonCritical[j + 1] = temp;
            }
        }
    }
    
    float freedCapacity = 0;
    int shedCount = 0;
    
    for (int i = 0; i < nonCritCount && freedCapacity < requiredCapacity; i++) {
        cout << "Turning OFF: " << nonCritical[i]->deviceName 
             << " (Priority " << nonCritical[i]->priority 
             << ", " << nonCritical[i]->consumptionRate << " W)" << endl;
        
        nonCritical[i]->turnOff();
        freedCapacity += nonCritical[i]->consumptionRate;
        shedCount++;
    }
    
    cout << "\nLoad Shedding Results:" << endl;
    cout << "Devices turned off: " << shedCount << endl;
    cout << "Capacity freed: " << freedCapacity << " W" << endl;
    
    if (freedCapacity >= requiredCapacity) {
        cout << "*** Load shedding successful ***" << endl;
        return true;
    } else {
        cout << "*** Insufficient capacity - load shedding failed ***" << endl;
        return false;
    }
}

void EnergyOptimizationSystem::viewCriticalDevices() {
    cout << "\n===== Critical Devices Report =====" << endl;
    Device* devices[100];
    int size;
    deviceRegistry.getAllValues(devices, size);
    
    int criticalCount = 0;
    float criticalLoad = 0;
    
    cout << "\nID\t\tName\t\t\tRate(W)\t\tStatus\t\tPriority" << endl;
    cout << "----------------------------------------------------------------" << endl;
    
    for (int i = 0; i < size; i++) {
        if (devices[i]->isCritical) {
            cout << devices[i]->deviceID << "\t\t"
                 << devices[i]->deviceName << "\t\t"
                 << devices[i]->consumptionRate << "\t\t"
                 << devices[i]->status << "\t\t"
                 << devices[i]->priority << endl;
            criticalCount++;
            if (devices[i]->status == "ON") {
                criticalLoad += devices[i]->consumptionRate;
            }
        }
    }
    
    if (criticalCount == 0) {
        cout << "No critical devices registered." << endl;
        return;
    }
    
    cout << "\nTotal Critical Devices: " << criticalCount << endl;
    cout << "Critical Load (Active): " << criticalLoad << " W" << endl;
    cout << "\n*** These devices are protected from automatic load shedding ***" << endl;
}

void EnergyOptimizationSystem::viewHistory() {
    cout << "\n===== Usage History =====" << endl;
    HistoryRecord records[100];
    int size;
    historyTracker.getAllRecords(records, size);
    
    if (size == 0) {
        cout << "No history records." << endl;
        return;
    }
    
    cout << "\nDevice\t\t\tRate(W)\t\tDuration(s)\tUnits(kWh)" << endl;
    cout << "----------------------------------------------------------------" << endl;
    
    float totalUnits = 0;
    for (int i = 0; i < size; i++) {
        cout << records[i].deviceName << "\t\t"
             << records[i].consumptionRate << "\t\t"
             << records[i].duration << "\t\t"
             << records[i].unitsConsumed << endl;
        totalUnits += records[i].unitsConsumed;
    }
    
    cout << "\nTotal Energy Consumed: " << totalUnits << " kWh" << endl;
    cout << "Estimated Cost (Rs 15/kWh): Rs " << (totalUnits * 15) << endl;
}

void EnergyOptimizationSystem::scheduleDevice() {
    char id[50];
    int timeHour, timeMinute, duration; 
    
    cout << "\n--- Schedule Device ---" << endl;
    cout << "Device ID: ";
    cin >> id;
    
    Device** device = deviceRegistry.get(string(id));
    if (!device) {
        cout << "Device not found!" << endl;
        return;
    }
    
    cout << "Schedule time (hour 0-23): ";
    cin >> timeHour;
    cout << "Schedule minute (0-59): "; 
    cin >> timeMinute;                   
    
    // Validate input                   
    if (timeHour < 0 || timeHour > 23) {
        cout << "Invalid hour! Must be 0-23." << endl;
        return;
    }
    if (timeMinute < 0 || timeMinute > 59) {
        cout << "Invalid minute! Must be 0-59." << endl;
        return;
    }
    
    cout << "Duration (minutes): ";
    cin >> duration;
    
    ScheduledTask task(
        (*device)->deviceID,
        (*device)->deviceName,
        timeHour,
        timeMinute,
        duration,   
        (*device)->priority,
        (*device)->isCritical
    );
    
    float tariff = (timeHour >= 6 && timeHour <= 22) ? 20.0f : 10.0f;
    task.estimatedCost = ((*device)->consumptionRate * duration / 60.0f / 1000.0f) * tariff;
    
    scheduler.enqueue(task);
    
    cout << "\nDevice scheduled successfully!" << endl;
    cout << "Scheduled for: " << timeHour << ":"  
         << (timeMinute < 10 ? "0" : "") << timeMinute << endl;
    cout << "Priority in queue: " << task.priority << endl;
    if ((*device)->isCritical) {
        cout << "*** CRITICAL device - will execute with highest priority ***" << endl;
    }
    cout << "Estimated cost: Rs " << task.estimatedCost << endl;
    
    if (timeHour >= 6 && timeHour <= 22 && !(*device)->isCritical) {
        cout << "\nSuggestion: Running this device between 11 PM - 6 AM" << endl;
        cout << "could save up to 50% on electricity costs!" << endl;
    }
}


void EnergyOptimizationSystem::viewSchedule() {
    scheduler.display();
}

void EnergyOptimizationSystem::setupCommunity() {
    cout << "\n--- Community Energy Setup ---" << endl;
    
    Home* home1 = new Home(string("H001"), string("123 St 7"), 2000, 1500, 5000);
    Home* home2 = new Home(string("H002"), string("456 St 8"), 1000, 1800, 3000);
    Home* home3 = new Home(string("H003"), string("789 St 9"), 3000, 1200, 6000);
    Home* home4 = new Home(string("H004"), string("101 St 10"), 2500, 1600, 4000);
    Home* home5 = new Home(string("H005"), string("202 St 11"), 1800, 1400, 5500);
    
    communityNetwork.addHome(home1);
    communityNetwork.addHome(home2);
    communityNetwork.addHome(home3);
    communityNetwork.addHome(home4);
    communityNetwork.addHome(home5);
    
    // ← NEW: Update H001 with real current consumption
    updateMyHomeConsumption();
    
    communityNetwork.connectHomes(string("H001"), string("H002"), 0.5f);
    communityNetwork.connectHomes(string("H002"), string("H003"), 0.3f);
    communityNetwork.connectHomes(string("H001"), string("H003"), 0.8f);
    communityNetwork.connectHomes(string("H003"), string("H004"), 0.6f);
    communityNetwork.connectHomes(string("H004"), string("H005"), 0.4f);
    communityNetwork.connectHomes(string("H001"), string("H004"), 1.2f);
    
    communitySetup = true;
    
    cout << "Community network initialized with 5 homes." << endl;
    communityNetwork.displayCommunityStatus();
}

void EnergyOptimizationSystem::requestEnergy() {
    if (!communitySetup) {
        cout << "\n  Please setup community network first (Option 7)!" << endl;
        return;
    }
    
    char homeID[10];
    float energy;
    
    cout << "\n--- Request Energy from Community ---" << endl;
    cout << "Your Home ID: ";
    cin >> homeID;
    cout << "Required Energy (Watts): ";
    cin >> energy;
    
    communityNetwork.findEnergySharing(string(homeID), energy);
}

void EnergyOptimizationSystem::generateReport() {
    FileManager::generateReport(deviceRegistry, historyTracker, scheduler, maxLoadCapacity, deviceCount);
}

void EnergyOptimizationSystem::displayMenu() {
    cout << "\n========================================" << endl;
    cout << "  ENERGY CONSUMPTION OPTIMIZER" << endl;
    cout << "========================================" << endl;
    cout << "1.  Add Device" << endl;
    cout << "2.  Monitor All Devices" << endl;
    cout << "3.  Turn Device ON/OFF" << endl;
    cout << "4.  View Usage History" << endl;
    cout << "5.  Schedule Device" << endl;
    cout << "6.  View Schedule" << endl;
    cout << "7.  Setup Community Network" << endl;
    cout << "8.  Generate Report" << endl;
    cout << "9.  Request Energy from Community" << endl;
    cout << "10. View Critical Devices" << endl;
    cout << "11. Save Data Now" << endl;              // ← NEW
    cout << "12. Generate Complete Report File" << endl;  // ← NEW
    cout << "0.  Exit" << endl;
    cout << "========================================" << endl;
    cout << "Choice: ";
}
void EnergyOptimizationSystem::checkAndExecuteScheduledTasks() {
    // Get current hour and minute
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    int currentHour = timeinfo->tm_hour;
    int currentMinute = timeinfo->tm_min;  
    
    while (!scheduler.isEmpty()) {
        ScheduledTask task = scheduler.peek();
        
        bool shouldExecute = false;
        if (task.scheduledTime < currentHour) {
            shouldExecute = true;  
        } else if (task.scheduledTime == currentHour && task.scheduledMinute <= currentMinute) {
            shouldExecute = true;  
        }
        
        if (shouldExecute) {
            scheduler.dequeue();  
            
            
            Device** device = deviceRegistry.get(task.deviceID);
            if (device && (*device)->status == "OFF") {
                
                float currentLoad = getCurrentTotalLoad();
                if (currentLoad + (*device)->consumptionRate <= maxLoadCapacity) {
                    (*device)->turnOn();
                    cout << "\n🔔 AUTO-EXECUTED: " << task.deviceName 
                         << " (Scheduled for " << task.scheduledTime << ":"   // ← UPDATED
                         << (task.scheduledMinute < 10 ? "0" : "") << task.scheduledMinute << ")" << endl;
                } else {
                    cout << "\n⚠️  Cannot auto-execute " << task.deviceName 
                         << " - would exceed capacity" << endl;
                }
            }
        } else {
            break;
        }
    }
}

// ← NEW: File handling implementations
void EnergyOptimizationSystem::saveAllData() {
    cout << "\n  Saving system data..." << endl;
    bool success = true;
    
    if (!FileManager::saveDevices(deviceRegistry)) {
        cout << "  Failed to save devices" << endl;
        success = false;
    }
    
    if (!FileManager::saveHistory(historyTracker)) {
        cout << "  Failed to save history" << endl;
        success = false;
    }
    
    if (!FileManager::saveSchedule(scheduler)) {
        cout << "  Failed to save schedule" << endl;
        success = false;
    }
    
    if (!FileManager::saveCommunity(communityNetwork, communitySetup)) {
        cout << "  Failed to save community" << endl;
        success = false;
    }
    
    if (success) {
        cout << "  All data saved successfully!" << endl;
    } else {
        cout << "   Some data may not have been saved" << endl;
    }
}

void EnergyOptimizationSystem::loadAllData() {
    cout << "\n  Loading system data..." << endl;
    bool success = true;
    
    if (!FileManager::loadDevices(deviceRegistry, deviceCount)) {
        cout << "  No device data found (first run)" << endl;
    } else {
        cout << "  Devices loaded: " << deviceCount << endl;
    }
    
    if (!FileManager::loadHistory(historyTracker)) {
        cout << "   No history data found" << endl;
    } else {
        cout << "  History loaded" << endl;
    }
    
    if (!FileManager::loadSchedule(scheduler)) {
        cout << "   No schedule data found" << endl;
    } else {
        cout << "  Schedule loaded" << endl;
    }
    
    if (!FileManager::loadCommunity(communityNetwork, communitySetup)) {
        cout << "   No community data found" << endl;
    } else {
        cout << "  Community loaded" << endl;
    }
    
    cout << "  System ready!" << endl;
}

// ← NEW: Update home consumption when devices change
void EnergyOptimizationSystem::updateMyHomeConsumption() {
    if (!communitySetup) return;  // Only if community is set up
    
    Home** myHome = communityNetwork.getHome("H001");
    if (myHome && *myHome) {
        float currentLoad = getCurrentTotalLoad();   // total load of all devices 
        (*myHome)->currentConsumption = currentLoad;   // total load of all devices 
        (*myHome)->updateEnergy();  // Recalculate excess energy
        
        // Optional: Show feedback
        if ((*myHome)->excessEnergy < 0) {
            cout << "   Your home is consuming more than producing (" 
                 << -(*myHome)->excessEnergy << " W deficit)" << endl;
        } else {
            cout << "  Your home has excess energy (" 
                 << (*myHome)->excessEnergy << " W available)" << endl;
        }
    }
}

void EnergyOptimizationSystem::run() {
    int choice;
    
    while (true) {
        checkAndExecuteScheduledTasks();
        
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: addDevice(); break;
            case 2: monitorDevices(); break;
            case 3: toggleDevice(); break;
            case 4: viewHistory(); break;
            case 5: scheduleDevice(); break;
            case 6: viewSchedule(); break;
            case 7: setupCommunity(); break;
            case 8: generateReport(); break;
            case 9: requestEnergy(); break;
            case 10: viewCriticalDevices(); break;
            case 11: saveAllData(); break;  // ← NEW
            case 12: FileManager::generateReport(deviceRegistry, historyTracker, scheduler, maxLoadCapacity, deviceCount); break;  // ← NEW
            case 0:
                cout << "\nThank you for using Energy Optimizer!" << endl;
                return;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}


