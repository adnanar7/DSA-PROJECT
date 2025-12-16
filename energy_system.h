#ifndef ENERGY_SYSTEM_H
#define ENERGY_SYSTEM_H

#include <iostream>
#include <cstring>
#include <ctime>
#include "device.h"
#include "hashmap.h"
#include "history.h"
#include "priority_queue.h"
#include "community_graph.h"
using namespace std;

class EnergyOptimizationSystem {
private:
    HashMap<string, Device*> deviceRegistry;
    UsageHistoryBST historyTracker;
    PriorityQueue scheduler;
    CommunityGraph communityNetwork;
    float maxLoadCapacity;
    int deviceCount;
    bool communitySetup;
    
    void checkAndExecuteScheduledTasks();  // ← ADD THIS LINE
    
public:
    EnergyOptimizationSystem() : maxLoadCapacity(5000), deviceCount(0), communitySetup(false) {}
    
    void addDevice();
    void monitorDevices();
    void toggleDevice();
    float getCurrentTotalLoad();
    bool performLoadShedding(float requiredCapacity);
    void viewCriticalDevices();
    void viewHistory();
    void scheduleDevice();
    void viewSchedule();
    void setupCommunity();
    void requestEnergy();
    void generateReport();
    void displayMenu();
    void run();
};

#endif // ENERGY_SYSTEM_H


