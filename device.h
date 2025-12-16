#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <ctime>
using namespace std;

class Device {
public:
    string deviceID;
    string deviceName;
    float consumptionRate;
    string status;
    int timestamp;
    float unitsUsed;
    bool isCritical;
    int priority;
    int startTime;
    
    Device() : consumptionRate(0), timestamp(0), unitsUsed(0), 
               isCritical(false), priority(5), startTime(0) {}
    
    Device(string id, string name, float rate, bool critical = false, int prio = 5) 
        : deviceID(id), deviceName(name), consumptionRate(rate), 
          status("OFF"), unitsUsed(0), isCritical(critical), 
          priority(prio), startTime(0) {
        timestamp = time(0);
        // Critical devices automatically get higher priority
        if (critical && prio < 8) {
            priority = 10; // Max priority for critical devices
        }
    }
    
    void turnOn() {
        status = "ON";
        startTime = time(0);
        timestamp = startTime;
    }
    
    void turnOff() {
        if (status == "ON") {
            int duration = time(0) - startTime;
            float hours = duration / 3600.0;
            unitsUsed += (consumptionRate * hours) / 1000.0;   //// Convert to kWh      // The += ensures that every time you turn the device on and off, the energy consumed in that session is added to the running total. This gives you an accurate picture of total energy consumption over the device's lifetime.Without +=, you'd only know about the last session, and all previous usage data would be lost!
        }
        status = "OFF";
        timestamp = time(0);
    }
    
    float getCurrentConsumption() const {
        if (status == "ON") {
            return consumptionRate;
        }
        return 0;
    }
};

#endif // DEVICE_H


