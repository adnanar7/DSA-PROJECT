#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <fstream>
#include <iostream>
#include <string>
#include "device.h"
#include "history.h"
#include "priority_queue.h"
#include "community_graph.h"
#include "hashmap.h"
using namespace std;

class FileManager {
public:
    // Save functions
    static bool saveDevices(HashMap<string, Device*>& deviceRegistry) {
        ofstream file("devices.dat", ios::binary);
        if (!file.is_open()) {
            cout << "Error: Could not create devices.dat" << endl;
            return false;
        }
        
        Device* devices[100];
        int size;
        deviceRegistry.getAllValues(devices, size);
        
        // Write count first
        file.write(reinterpret_cast<char*>(&size), sizeof(int));
        
        // Write each device
        for (int i = 0; i < size; i++) {
            // Write string lengths and data
            int idLen = devices[i]->deviceID.length();
            int nameLen = devices[i]->deviceName.length();
            int statusLen = devices[i]->status.length();
            
            file.write(reinterpret_cast<char*>(&idLen), sizeof(int));
            file.write(devices[i]->deviceID.c_str(), idLen);
            
            file.write(reinterpret_cast<char*>(&nameLen), sizeof(int));
            file.write(devices[i]->deviceName.c_str(), nameLen);
            
            file.write(reinterpret_cast<char*>(&devices[i]->consumptionRate), sizeof(float));
            
            file.write(reinterpret_cast<char*>(&statusLen), sizeof(int));
            file.write(devices[i]->status.c_str(), statusLen);
            
            file.write(reinterpret_cast<char*>(&devices[i]->timestamp), sizeof(int));
            file.write(reinterpret_cast<char*>(&devices[i]->unitsUsed), sizeof(float));
            file.write(reinterpret_cast<char*>(&devices[i]->isCritical), sizeof(bool));
            file.write(reinterpret_cast<char*>(&devices[i]->priority), sizeof(int));
            file.write(reinterpret_cast<char*>(&devices[i]->startTime), sizeof(int));
        }
        
        file.close();
        return true;
    }
    
    static bool loadDevices(HashMap<string, Device*>& deviceRegistry, int& deviceCount) {
        ifstream file("devices.dat", ios::binary);
        if (!file.is_open()) {
            return false; // File doesn't exist, fresh start
        }
        
        int size;
        file.read(reinterpret_cast<char*>(&size), sizeof(int));
        
        for (int i = 0; i < size; i++) {
            int idLen, nameLen, statusLen;
            char buffer[256];
            
            // Read deviceID
            file.read(reinterpret_cast<char*>(&idLen), sizeof(int));
            file.read(buffer, idLen);
            buffer[idLen] = '\0';
            string deviceID(buffer);
            
            // Read deviceName
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
            file.read(buffer, nameLen);
            buffer[nameLen] = '\0';
            string deviceName(buffer);
            
            float consumptionRate;
            file.read(reinterpret_cast<char*>(&consumptionRate), sizeof(float));
            
            // Read status
            file.read(reinterpret_cast<char*>(&statusLen), sizeof(int));
            file.read(buffer, statusLen);
            buffer[statusLen] = '\0';
            string status(buffer);
            
            int timestamp;
            float unitsUsed;
            bool isCritical;
            int priority;
            int startTime;
            
            file.read(reinterpret_cast<char*>(&timestamp), sizeof(int));
            file.read(reinterpret_cast<char*>(&unitsUsed), sizeof(float));
            file.read(reinterpret_cast<char*>(&isCritical), sizeof(bool));
            file.read(reinterpret_cast<char*>(&priority), sizeof(int));
            file.read(reinterpret_cast<char*>(&startTime), sizeof(int));
            
            // Create device and restore state
            Device* device = new Device(deviceID, deviceName, consumptionRate, isCritical, priority);
            device->status = status;
            device->timestamp = timestamp;
            device->unitsUsed = unitsUsed;
            device->startTime = startTime;
            
            deviceRegistry.insert(deviceID, device);
            deviceCount++;
        }
        
        file.close();
        return true;
    }
    
    static bool saveHistory(UsageHistoryBST& historyTracker) {
        ofstream file("history.dat", ios::binary);
        if (!file.is_open()) {
            cout << "Error: Could not create history.dat" << endl;
            return false;
        }
        
        HistoryRecord records[100];
        int size;
        historyTracker.getAllRecords(records, size);
        
        file.write(reinterpret_cast<char*>(&size), sizeof(int));
        
        for (int i = 0; i < size; i++) {
            int idLen = records[i].deviceID.length();
            int nameLen = records[i].deviceName.length();
            
            file.write(reinterpret_cast<char*>(&idLen), sizeof(int));
            file.write(records[i].deviceID.c_str(), idLen);
            
            file.write(reinterpret_cast<char*>(&nameLen), sizeof(int));
            file.write(records[i].deviceName.c_str(), nameLen);
            
            file.write(reinterpret_cast<char*>(&records[i].consumptionRate), sizeof(float));
            file.write(reinterpret_cast<char*>(&records[i].timestamp), sizeof(int));
            file.write(reinterpret_cast<char*>(&records[i].duration), sizeof(int));
            file.write(reinterpret_cast<char*>(&records[i].unitsConsumed), sizeof(float));
        }
        
        file.close();
        return true;
    }
    
    static bool loadHistory(UsageHistoryBST& historyTracker) {
        ifstream file("history.dat", ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        int size;
        file.read(reinterpret_cast<char*>(&size), sizeof(int));
        
        for (int i = 0; i < size; i++) {
            int idLen, nameLen;
            char buffer[256];
            
            file.read(reinterpret_cast<char*>(&idLen), sizeof(int));
            file.read(buffer, idLen);
            buffer[idLen] = '\0';
            string deviceID(buffer);
            
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
            file.read(buffer, nameLen);
            buffer[nameLen] = '\0';
            string deviceName(buffer);
            
            float consumptionRate;
            int timestamp, duration;
            float unitsConsumed;
            
            file.read(reinterpret_cast<char*>(&consumptionRate), sizeof(float));
            file.read(reinterpret_cast<char*>(&timestamp), sizeof(int));
            file.read(reinterpret_cast<char*>(&duration), sizeof(int));
            file.read(reinterpret_cast<char*>(&unitsConsumed), sizeof(float));
            
            HistoryRecord record(deviceID, deviceName, consumptionRate, timestamp, duration, unitsConsumed);
            historyTracker.insertRecord(record);
        }
        
        file.close();
        return true;
    }
    
    static bool saveSchedule(PriorityQueue& scheduler) {
        ofstream file("schedule.dat", ios::binary);
        if (!file.is_open()) {
            cout << "Error: Could not create schedule.dat" << endl;
            return false;
        }
        
        // Create temporary array to store tasks
        ScheduledTask tasks[100];
        int size = 0;
        
        // Extract all tasks (this will empty the queue)
        PriorityQueue tempQueue;
        while (!scheduler.isEmpty()) {
            tasks[size++] = scheduler.dequeue();
        }
        
        // Write size
        file.write(reinterpret_cast<char*>(&size), sizeof(int));
        
        // Write each task and restore to queue
        for (int i = 0; i < size; i++) {
            int idLen = tasks[i].deviceID.length();
            int nameLen = tasks[i].deviceName.length();
            
            file.write(reinterpret_cast<char*>(&idLen), sizeof(int));
            file.write(tasks[i].deviceID.c_str(), idLen);
            
            file.write(reinterpret_cast<char*>(&nameLen), sizeof(int));
            file.write(tasks[i].deviceName.c_str(), nameLen);
            
            file.write(reinterpret_cast<char*>(&tasks[i].scheduledTime), sizeof(int));
            file.write(reinterpret_cast<char*>(&tasks[i].scheduledMinute), sizeof(int));
            file.write(reinterpret_cast<char*>(&tasks[i].duration), sizeof(int));
            file.write(reinterpret_cast<char*>(&tasks[i].priority), sizeof(int));
            file.write(reinterpret_cast<char*>(&tasks[i].isCritical), sizeof(bool));
            file.write(reinterpret_cast<char*>(&tasks[i].estimatedCost), sizeof(float));
            
            // Restore task back to original queue
            scheduler.enqueue(tasks[i]);
        }
        
        file.close();
        return true;
    }
    
    static bool loadSchedule(PriorityQueue& scheduler) {
        ifstream file("schedule.dat", ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        int size;
        file.read(reinterpret_cast<char*>(&size), sizeof(int));
        
        for (int i = 0; i < size; i++) {
            int idLen, nameLen;
            char buffer[256];
            
            file.read(reinterpret_cast<char*>(&idLen), sizeof(int));
            file.read(buffer, idLen);
            buffer[idLen] = '\0';
            string deviceID(buffer);
            
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
            file.read(buffer, nameLen);
            buffer[nameLen] = '\0';
            string deviceName(buffer);
            
            int scheduledTime, scheduledMinute, duration, priority;
            bool isCritical;
            float estimatedCost;
            
            file.read(reinterpret_cast<char*>(&scheduledTime), sizeof(int));
            file.read(reinterpret_cast<char*>(&scheduledMinute), sizeof(int));
            file.read(reinterpret_cast<char*>(&duration), sizeof(int));
            file.read(reinterpret_cast<char*>(&priority), sizeof(int));
            file.read(reinterpret_cast<char*>(&isCritical), sizeof(bool));
            file.read(reinterpret_cast<char*>(&estimatedCost), sizeof(float));
            
            ScheduledTask task(deviceID, deviceName, scheduledTime, scheduledMinute, duration, priority, isCritical);
            task.estimatedCost = estimatedCost;
            scheduler.enqueue(task);
        }
        
        file.close();
        return true;
    }
    
    static bool saveCommunity(CommunityGraph& communityNetwork, bool& communitySetup) {
        ofstream file("community.dat", ios::binary);
        if (!file.is_open()) {
            cout << "Error: Could not create community.dat" << endl;
            return false;
        }
        
        // Write setup flag
        file.write(reinterpret_cast<char*>(&communitySetup), sizeof(bool));
        
        // Note: Full community graph serialization is complex
        // For now, we'll just save the setup flag
        // In a complete implementation, you'd save all homes and edges
        
        file.close();
        return true;
    }
    
    static bool loadCommunity(CommunityGraph& communityNetwork, bool& communitySetup) {
        ifstream file("community.dat", ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file.read(reinterpret_cast<char*>(&communitySetup), sizeof(bool));
        
        file.close();
        return true;
    }
    
    // Generate comprehensive report
    static bool generateReport(HashMap<string, Device*>& deviceRegistry,
                              UsageHistoryBST& historyTracker,
                              PriorityQueue& scheduler,
                              float maxLoadCapacity,
                              int deviceCount) {
        // Get current timestamp for filename
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);
        char filename[100];
        sprintf(filename, "energy_report_%04d-%02d-%02d_%02d-%02d.txt",
                timeinfo->tm_year + 1900,
                timeinfo->tm_mon + 1,
                timeinfo->tm_mday,
                timeinfo->tm_hour,
                timeinfo->tm_min);
        
        ofstream report(filename);
        if (!report.is_open()) {
            cout << "Error: Could not create report file" << endl;
            return false;
        }
        
        report << "============================================\n";
        report << "   ENERGY OPTIMIZATION SYSTEM REPORT\n";
        report << "============================================\n\n";
        
        // Section 1: System Overview
        report << ">>> SECTION 1: SYSTEM OVERVIEW <<<\n";
        report << "Report Generated: " << asctime(timeinfo);
        report << "Total Devices Registered: " << deviceCount << "\n";
        report << "Max Load Capacity: " << maxLoadCapacity << " W\n\n";
        
        // Section 2: Device Summary
        report << ">>> SECTION 2: DEVICE SUMMARY <<<\n";
        Device* devices[100];
        int deviceSize;
        deviceRegistry.getAllValues(devices, deviceSize);
        
        float totalLoad = 0;
        int activeCount = 0;
        int criticalCount = 0;
        
        report << "ID\t\tName\t\t\tRate(W)\t\tStatus\t\tPriority\tType\n";
        report << "--------------------------------------------------------------------------------\n";
        
        for (int i = 0; i < deviceSize; i++) {
            report << devices[i]->deviceID << "\t\t"
                   << devices[i]->deviceName << "\t\t"
                   << devices[i]->consumptionRate << "\t\t"
                   << devices[i]->status << "\t\t"
                   << devices[i]->priority << "\t\t"
                   << (devices[i]->isCritical ? "[CRITICAL]" : "[NORMAL]") << "\n";
            
            if (devices[i]->status == "ON") {
                totalLoad += devices[i]->consumptionRate;
                activeCount++;
            }
            if (devices[i]->isCritical) criticalCount++;
        }
        
        report << "\nActive Devices: " << activeCount << "\n";
        report << "Critical Devices: " << criticalCount << "\n";
        report << "Current Total Load: " << totalLoad << " W\n";
        report << "Load Utilization: " << (maxLoadCapacity > 0 ? (totalLoad/maxLoadCapacity*100) : 0) << "%\n\n";
        
        // Section 3: Usage History
        report << ">>> SECTION 3: USAGE HISTORY <<<\n";
        HistoryRecord records[100];
        int historySize;
        historyTracker.getAllRecords(records, historySize);
        
        report << "Total Historical Records: " << historySize << "\n\n";
        
        if (historySize > 0) {
            report << "Device\t\t\tRate(W)\t\tDuration(s)\tUnits(kWh)\n";
            report << "----------------------------------------------------------------\n";
            
            float totalUnits = 0;
            for (int i = 0; i < historySize; i++) {
                report << records[i].deviceName << "\t\t"
                       << records[i].consumptionRate << "\t\t"
                       << records[i].duration << "\t\t"
                       << records[i].unitsConsumed << "\n";
                totalUnits += records[i].unitsConsumed;
            }
            
            report << "\nTotal Energy Consumed: " << totalUnits << " kWh\n";
            report << "Estimated Cost (Rs 15/kWh): Rs " << (totalUnits * 15) << "\n\n";
        } else {
            report << "No usage history available.\n\n";
        }
        
        // Section 4: Scheduled Tasks
        report << ">>> SECTION 4: SCHEDULED TASKS <<<\n";
        
        // Extract tasks without modifying queue
        ScheduledTask tasks[100];
        int scheduleSize = 0;
        PriorityQueue tempQueue;
        
        while (!scheduler.isEmpty()) {
            tasks[scheduleSize++] = scheduler.dequeue();
        }
        
        // Restore queue
        for (int i = 0; i < scheduleSize; i++) {
            scheduler.enqueue(tasks[i]);
        }
        
        if (scheduleSize > 0) {
            report << "Pending Tasks: " << scheduleSize << "\n\n";
            for (int i = 0; i < scheduleSize; i++) {
                report << (i+1) << ". " << tasks[i].deviceName 
                       << " | Priority: " << tasks[i].priority
                       << (tasks[i].isCritical ? " [CRITICAL]" : "")
                       << " | Time: " << tasks[i].scheduledTime << ":" 
                       << (tasks[i].scheduledMinute < 10 ? "0" : "") << tasks[i].scheduledMinute
                       << " | Duration: " << tasks[i].duration << " min"
                       << " | Est. Cost: Rs " << tasks[i].estimatedCost << "\n";
            }
        } else {
            report << "No scheduled tasks.\n";
        }
        report << "\n";
        
        // Section 5: Recommendations
        report << ">>> SECTION 5: RECOMMENDATIONS <<<\n";
        report << "1. Shift high-power devices to off-peak hours (11 PM - 6 AM) for cost savings\n";
        report << "2. Current load at " << (maxLoadCapacity > 0 ? (totalLoad/maxLoadCapacity*100) : 0) << "% capacity\n";
        if (totalLoad > maxLoadCapacity * 0.7f) {
            report << "3. WARNING: Approaching maximum capacity - consider load balancing\n";
        }
        report << "4. Critical devices (" << criticalCount << ") are protected from load shedding\n\n";
        
        report << "============================================\n";
        report << "           END OF REPORT\n";
        report << "============================================\n";
        
        report.close();
        
        cout << "\n✓ Report generated successfully: " << filename << endl;
        return true;
    }
};

#endif // FILE_MANAGER_H