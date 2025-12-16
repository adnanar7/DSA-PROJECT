#include <iostream>
#include <cassert>
#include "../energy_system.h"

// NOTE: Most EnergyOptimizationSystem methods are interactive (cin/cout),
// so here we only test a few non-interactive aspects via internal APIs.

int main() {
    std::cout << "[test_energy_system_basic] Running tests..." << std::endl;

    EnergyOptimizationSystem system;

    // We cannot directly add devices without cin, but we can at least
    // call some non-interactive methods to ensure they don't crash
    // in an empty system.
    float load = system.getCurrentTotalLoad();
    assert(load == 0.0f);

    bool ok = system.performLoadShedding(100.0f);
    // With no devices, it should not be able to free capacity.
    assert(ok == false);

    // History / report functions should also not crash on empty data.
    system.generateReport();

    std::cout << "[test_energy_system_basic] All tests (non-interactive subset) passed!" << std::endl;
    return 0;
}


