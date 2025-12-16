#include <iostream>
#include <cassert>
#include <string>
#include "../community_graph.h"

using namespace std;

int main() {
    cout << "[test_community_graph] Running tests..." << endl;

    CommunityGraph graph;
    Home* h1 = new Home("H001", "A", 2000, 1500, 5000);
    Home* h2 = new Home("H002", "B", 1000, 500, 3000);
    Home* h3 = new Home("H003", "C", 500, 200, 2000);

    graph.addHome(h1);
    graph.addHome(h2);
    graph.addHome(h3);

    graph.connectHomes("H001", "H002", 1.0f);
    graph.connectHomes("H002", "H003", 2.0f);

    string neighbors[10];
    int count = 0;
    graph.findAllNeighborsBFS("H001", neighbors, count);
    assert(count >= 2); // should see H002 and H003 reachable

    string path[10];
    int pathLen = 0;
    float cost = graph.findCheapestPath("H001", "H003", path, pathLen);
    assert(cost > 0);
    assert(pathLen >= 2);

    // Edge case: disconnected home, no path
    Home* h4 = new Home("H004", "D", 1000, 900, 1000);
    graph.addHome(h4); // no connections from H004
    string path2[10];
    int pathLen2 = 0;
    float cost2 = graph.findCheapestPath("H004", "H001", path2, pathLen2);
    assert(cost2 == -1); // no path should return -1

    cout << "[test_community_graph] All tests passed!" << endl;
    return 0;
}


