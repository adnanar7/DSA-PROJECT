
// This file now only contains the entry point and uses the modular system.

#include <iostream>
#include "energy_system.h"
using namespace std;

int main() {
    EnergyOptimizationSystem system;
    
    cout << "========================================" << endl;
    cout << "   SMART HOME ENERGY OPTIMIZER" << endl;
    cout << "========================================" << endl;
    cout << "\nInitializing system..." << endl;
    
    system.run();
    
    return 0;
}