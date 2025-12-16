## Build & Run Instructions

### 1. Go to the project directory

```bash
cd "/Users/macworld/Downloads/DSA Project"
```

### 2. Build the main application

```bash
g++ -std=c++17 main.cpp energy_system.cpp community_graph.cpp -o energy_optimizer
```

### 3. Run the main application

```bash
./energy_optimizer
```

---

### 4. Build all tests

```bash
g++ -std=c++17 tests/test_hashmap.cpp -o tests/test_hashmap
g++ -std=c++17 tests/test_community_graph.cpp community_graph.cpp -o tests/test_community_graph
g++ -std=c++17 tests/test_priority_queue.cpp -o tests/test_priority_queue
g++ -std=c++17 tests/test_energy_system_basic.cpp energy_system.cpp community_graph.cpp -o tests/test_energy_system_basic
```

### 5. Run all tests

```bash
./tests/test_hashmap
./tests/test_community_graph
./tests/test_priority_queue
./tests/test_energy_system_basic
```

---

## Module Ownership & File Mapping

To keep concepts consistent (each member owns one major data-structure “theme”) and still stay modular, we divide responsibilities as follows.

### 👤 Member 1 – Data Layer: Devices, Hashing & History (HashMap + BST)

- **Concept focus**: All **data storage** for devices and history  
  - Hashing for fast lookup (`HashMap`).
  - BST for time-based history (`UsageHistoryBST`).
  - Core “model” of a device.

- **Responsibilities**
  - Real-time device registry backed by a hash map.
  - Device model (fields, status, timestamps, critical flag).
  - Usage-history insert and traversal (BST).
  - History queries by timestamp range (can be presented as time-based search).
  - Supplying data for dashboards / reports (used by Member 2 & 3).

- **Files owned**
  - `device.h`  
    - `Device` class and its behavior (`turnOn`, `turnOff`, energy calculation).
  - `hashmap.h`  
    - Generic `HashMap<K, V>` implementation (collision handling, insert/get/remove, key/value traversal).
    - Used by:
      - Device registry (`HashMap<string, Device*>`).
      - Community graph’s internal maps (Member 3 uses it, but structure belongs to Member 1).
  - `history.h`  
    - `HistoryRecord`, `HistoryNode`, `UsageHistoryBST` (insert, in-order traversal, range queries).
  - `utils.h`  
    - `hashString` utility used by `hashmap.h` and graph code.
  - `energy_system.cpp` (Member 1–relevant methods)
    - `viewHistory()` – pulls data from `UsageHistoryBST`.
  - Tests
    - `tests/test_hashmap.cpp` – validates hash map behavior (including `Device*` values).

During presentation, Member 1 can clearly talk about **HashMap** and **BST** as their main data structures and show how they support the rest of the system.

---

### 👤 Member 2 – Scheduling & Critical Load Management (Priority Queue)

- **Concept focus**: All **priority-based decisions** using a **priority queue**  
  - Appliance scheduling by priority and time.
  - Critical vs non-critical load management (automatic load shedding).

- **Responsibilities**
  - Build and manage the scheduling queue for devices.
  - Decide order of execution based on priority and scheduled time.
  - Use priority and power ratings to free capacity during overload.
  - Maintain and report on critical devices (from a priority perspective).

- **Files owned**
  - `priority_queue.h`  
    - `ScheduledTask` struct.
    - `PriorityQueue` implementation (heap operations, `enqueue`, `dequeue`, `peek`, `heapifyUp/Down`).
  - `energy_system.cpp` (Member 2–relevant methods)
    - `scheduleDevice()` – creates `ScheduledTask`s, computes cost, and inserts into `PriorityQueue`.
    - `viewSchedule()` – displays queue contents.
    - `performLoadShedding()` – uses priority information on active devices to decide which non-critical devices to turn off.
    - `viewCriticalDevices()` – reports all devices marked as critical and their load.
  - Tests
    - `tests/test_priority_queue.cpp` – validates priority behavior, including same-priority/tie-breaking and empty-queue edge cases.

In the project demo, Member 2 can focus on the **priority queue** concept: how scheduling and load shedding are both driven by priority-based logic.

---

### 👤 Member 3 – Community Graph & System Integration (Graph + Orchestrator)

- **Concept focus**: **Graph algorithms** + overall **system orchestration**  
  - Community energy-sharing graph (homes and connections).
  - BFS + Dijkstra for finding contributors/receivers and cheapest paths.
  - Integrating all modules into one working application.

- **Responsibilities**
  - Model the community as a graph of homes with weighted edges (distance / cost).
  - Implement BFS to explore neighbors and Dijkstra to find cheapest sharing routes.
  - Provide energy-sharing recommendations and cost/savings comparison.
  - Orchestrate the entire system:
    - Call monitoring, scheduling, and sharing modules.
    - Present menu, handle user choices, generate final report.

- **Files owned**
  - `community_graph.h` / `community_graph.cpp`  
    - `Home`, `GraphEdge`, `BFSQueue`, `CommunityGraph`.
    - Algorithms:
      - `findAllNeighborsBFS()` – BFS traversal to find connected homes.
      - `findCheapestPath()` – Dijkstra-based path and cost computation.
      - `displayCommunityStatus()`, `findEnergySharing()` – sharing strategy and cost/savings.
  - `energy_system.h` / `energy_system.cpp` (integration layer)
    - System-level class `EnergyOptimizationSystem`.
    - High-level orchestration and integration of all modules:
      - `addDevice()` – integrates device-creation with critical flags and priorities.
      - `monitorDevices()` – uses Member 1’s data to show live status.
      - `toggleDevice()` – ties together capacity checks (Member 2 logic), history tracking (Member 1), and critical protection.
      - `setupCommunity()` and `requestEnergy()` – use `CommunityGraph` to set up and query sharing.
      - `generateReport()` – final report: energy consumed, utilization, savings suggestions, critical device stats.
      - `displayMenu()` / `run()` – main loop for user interaction and calling all features.
  - `main.cpp`
    - Application entry point: creates `EnergyOptimizationSystem` and starts `run()`.
  - Tests
    - `tests/test_energy_system_basic.cpp` – non-interactive tests around integrated behavior (load shedding and reporting with no devices).

Member 3 can present the **graph** as their primary data structure and also show how all three modules come together in the final integrated system.

---

With this division:

- Each member owns one **clear data-structure theme** (HashMap+BST, PriorityQueue, Graph).
- Ownership is **conceptually consistent** for presentations.
- The codebase remains modular: modules are split by headers/source files, but they collaborate through `EnergyOptimizationSystem` and shared utilities.

