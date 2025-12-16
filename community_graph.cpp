#include "community_graph.h"

void CommunityGraph::findAllNeighborsBFS(string startHomeID, string* neighbors, int& count) {
    cout << "\n===== BFS: Finding All Neighbors =====" << endl;
    cout << "Starting from: " << startHomeID << endl;
    
    count = 0;
    bool visited[100] = {false};
    BFSQueue q;
    
    int startIndex = hashHomeID(startHomeID);
    visited[startIndex] = true;
    q.enqueue(startHomeID);
    
    int level = 0;
    cout << "\nBFS Traversal:" << endl;
    cout << "Level 0: " << startHomeID << " (Starting home)" << endl;
    
    while (!q.isEmpty()) {
        int levelSize = q.getSize();
        level++;
        
        if (levelSize > 0) {
            cout << "Level " << level << ": ";
        }
        
        for (int i = 0; i < levelSize; i++) {
            string currentHome = q.dequeue();
            
            GraphEdge** edgeList = adjacencyList.get(currentHome);
            if (!edgeList || *edgeList == nullptr) continue;
            
            GraphEdge* edge = *edgeList;
            while (edge != nullptr) {
                int neighborIndex = hashHomeID(edge->neighborID);
                
                if (!visited[neighborIndex]) {
                    visited[neighborIndex] = true;
                    neighbors[count++] = edge->neighborID;
                    q.enqueue(edge->neighborID);
                    
                    cout << edge->neighborID << "(" << edge->distance << "km) ";
                }
                edge = edge->next;
            }
        }
        if (levelSize > 0) {
            cout << endl;
        }
    }
    
    cout << "\nTotal neighbors found: " << count << endl;
    cout << "======================================" << endl;
}

float CommunityGraph::findCheapestPath(string startHome, string targetHome, string* path, int& pathLength) {
    cout << "\n===== Dijkstra: Finding Cheapest Path =====" << endl;
    cout << "From: " << startHome << " -> To: " << targetHome << endl;
    
    string allHomes[100];
    int homeCountLocal;
    homes.getAllKeys(allHomes, homeCountLocal);
    
    float distance[100];
    string previous[100];
    bool visited[100];
    
    for (int i = 0; i < 100; i++) {
        distance[i] = 999999;
        visited[i] = false;
    }
    
    int startIdx = -1;
    for (int i = 0; i < homeCountLocal; i++) {
        if (allHomes[i] == startHome) {
            startIdx = i;
            distance[i] = 0;
            break;
        }
    }
    
    if (startIdx == -1) {
        cout << "Start home not found!" << endl;
        return -1;
    }
    
    for (int iteration = 0; iteration < homeCountLocal; iteration++) {
        int minIdx = -1;
        float minDist = 999999;
        
        for (int i = 0; i < homeCountLocal; i++) {
            if (!visited[i] && distance[i] < minDist) {
                minDist = distance[i];
                minIdx = i;
            }
        }
        
        if (minIdx == -1) break;
        
        visited[minIdx] = true;
        string currentHome = allHomes[minIdx];
        
        cout << "Visiting: " << currentHome << " (Cost: " << distance[minIdx] << ")" << endl;
        
        GraphEdge** edgeList = adjacencyList.get(currentHome);
        if (!edgeList || *edgeList == nullptr) continue;
        
        GraphEdge* edge = *edgeList;
        while (edge != nullptr) {
            int neighborIdx = -1;
            for (int i = 0; i < homeCountLocal; i++) {
                if (allHomes[i] == edge->neighborID) {
                    neighborIdx = i;
                    break;
                }
            }
            
            if (neighborIdx != -1 && !visited[neighborIdx]) {
                float edgeCost = edge->distance * 5;
                float newDist = distance[minIdx] + edgeCost;
                
                if (newDist < distance[neighborIdx]) {
                    distance[neighborIdx] = newDist;
                    previous[neighborIdx] = currentHome;
                    cout << "  Updated " << edge->neighborID << " cost to " << newDist << endl;
                }
            }
            
            edge = edge->next;
        }
    }
    
    int targetIdx = -1;
    for (int i = 0; i < homeCountLocal; i++) {
        if (allHomes[i] == targetHome) {
            targetIdx = i;
            break;
        }
    }
    
    if (targetIdx == -1 || distance[targetIdx] == 999999) {
        cout << "No path found!" << endl;
        return -1;
    }
    
    string reversePath[100];
    int revCount = 0;
    string current = targetHome;
    
    while (current != startHome) {
        reversePath[revCount++] = current;
        bool found = false;
        for (int i = 0; i < homeCountLocal; i++) {
            if (allHomes[i] == current) {
                current = previous[i];
                found = true;
                break;
            }
        }
        if (!found) break;
    }
    reversePath[revCount++] = startHome;
    
    pathLength = revCount;
    for (int i = 0; i < revCount; i++) {
        path[i] = reversePath[revCount - 1 - i];
    }
    
    cout << "\nCheapest path: ";
    for (int i = 0; i < pathLength; i++) {
        cout << path[i];
        if (i < pathLength - 1) cout << " -> ";
    }
    cout << endl;
    cout << "Total cost: Rs " << distance[targetIdx] << endl;
    cout << "======================================" << endl;
    
    return distance[targetIdx];
}

void CommunityGraph::displayCommunityStatus() {
    cout << "\n===== Community Energy Status =====" << endl;
    cout << "Total homes in network: " << homeCount << endl;
    
    string homeIDs[100];
    int count;
    homes.getAllKeys(homeIDs, count);
    
    cout << "\nHome ID\tProduction(W)\tConsumption(W)\tExcess(W)" << endl;
    cout << "--------------------------------------------------------" << endl;
    
    for (int i = 0; i < count; i++) {
        Home** home = homes.get(homeIDs[i]);
        if (home && *home) {
            (*home)->updateEnergy();
            cout << (*home)->homeID << "\t"
                 << (*home)->currentProduction << "\t\t"
                 << (*home)->currentConsumption << "\t\t"
                 << (*home)->excessEnergy << endl;
        }
    }
}

void CommunityGraph::findEnergySharing(string requestingHomeID, float requiredEnergy) {
    cout << "\n############################################" << endl;
    cout << "#   COMMUNITY ENERGY SHARING SYSTEM        #" << endl;
    cout << "############################################" << endl;
    
    Home** requester = homes.get(requestingHomeID);
    if (!requester || *requester == nullptr) {
        cout << "\n  Error: Home not found!" << endl;
        return;
    }
    
    cout << "\n  Requesting Home Details:" << endl;
    cout << "   ID: " << requestingHomeID << endl;
    cout << "   Address: " << (*requester)->address << endl;
    cout << "   Production: " << (*requester)->currentProduction << " W" << endl;
    cout << "   Consumption: " << (*requester)->currentConsumption << " W" << endl;
    cout << "   Deficit: " << requiredEnergy << " W" << endl;
    
    cout << "\n  Phase 1: Finding All Connected Homes (BFS)" << endl;
    cout << "============================================" << endl;
    
    string allNeighbors[100];
    int neighborCount = 0;
    findAllNeighborsBFS(requestingHomeID, allNeighbors, neighborCount);
    
    if (neighborCount == 0) {
        cout << "\n  No connected homes found!" << endl;
        return;
    }
    
    cout << "\n  Phase 2: Identifying Energy Providers" << endl;
    cout << "============================================" << endl;
    
    struct Provider {
        string homeID;
        float excessEnergy;
        float pathCost;
        string path[100];
        int pathLength;
    };
    
    Provider providers[100];
    int providerCount = 0;
    
    for (int i = 0; i < neighborCount; i++) {
        Home** neighbor = homes.get(allNeighbors[i]);
        if (!neighbor || *neighbor == nullptr) continue;
        
        (*neighbor)->updateEnergy();
        
        if ((*neighbor)->excessEnergy > 0) {
            cout << "✓ " << allNeighbors[i] << " has " 
                 << (*neighbor)->excessEnergy << " W excess" << endl;
            
            providers[providerCount].homeID = allNeighbors[i];
            providers[providerCount].excessEnergy = (*neighbor)->excessEnergy;
            providerCount++;
        } else {
            cout << "✗ " << allNeighbors[i] << " has deficit" << endl;
        }
    }
    
    if (providerCount == 0) {
        cout << "\n  No providers with excess energy!" << endl;
        return;
    }
    
    cout << "\n  Phase 3: Finding Cheapest Routes (Dijkstra)" << endl;
    cout << "============================================" << endl;
    
    for (int i = 0; i < providerCount; i++) {
        providers[i].pathCost = findCheapestPath(
            requestingHomeID,
            providers[i].homeID,
            providers[i].path,
            providers[i].pathLength
        );
    }
    
    cout << "\n  Phase 4: Ranking Providers by Cost" << endl;
    cout << "============================================" << endl;
    
    for (int i = 0; i < providerCount - 1; i++) {
        for (int j = 0; j < providerCount - i - 1; j++) {
            if (providers[j].pathCost > providers[j + 1].pathCost) {
                Provider temp = providers[j];
                providers[j] = providers[j + 1];
                providers[j + 1] = temp;
            }
        }
    }
    
    cout << "\nRank\tHome\tExcess(W)\tCost(Rs)" << endl;
    cout << "----------------------------------------" << endl;
    for (int i = 0; i < providerCount; i++) {
        cout << (i+1) << "\t" << providers[i].homeID << "\t"
             << providers[i].excessEnergy << "\t\t"
             << providers[i].pathCost << endl;
    }
    
    cout << "\n Phase 5: Optimal Energy Distribution" << endl;
    cout << "============================================" << endl;
    
    float remainingNeed = requiredEnergy;
    float totalCost = 0;
    int transferCount = 0;
    
    for (int i = 0; i < providerCount && remainingNeed > 0; i++) {
        float energyFromProvider = (providers[i].excessEnergy < remainingNeed)
                                    ? providers[i].excessEnergy : remainingNeed;
        
        float transferCost = (energyFromProvider / 1000.0f) * providers[i].pathCost;
        
        cout << "\n Transfer #" << (++transferCount) << ":" << endl;
        cout << "   From: " << providers[i].homeID << endl;
        cout << "   Energy: " << energyFromProvider << " W" << endl;
        cout << "   Route: ";
        for (int j = 0; j < providers[i].pathLength; j++) {
            cout << providers[i].path[j];
            if (j < providers[i].pathLength - 1) cout << " -> ";
        }
        cout << endl;
        cout << "   Cost: Rs " << transferCost << endl;
        
        remainingNeed -= energyFromProvider;
        totalCost += transferCost;
        
        Home** provider = homes.get(providers[i].homeID);
        if (provider && *provider) {
            (*provider)->excessEnergy -= energyFromProvider;
            (*provider)->batteryLevel -= energyFromProvider;
        }
    }
    
    cout << "\n  SUMMARY" << endl;
    cout << "============================================" << endl;
    cout << "Requested Energy: " << requiredEnergy << " W" << endl;
    cout << "Energy Received: " << (requiredEnergy - remainingNeed) << " W" << endl;
    
    if (remainingNeed > 0) {
        cout << "   Still Need: " << remainingNeed << " W (buy from grid)" << endl;
    } else {
        cout << "  Full requirement satisfied!" << endl;
    }
    
    cout << "\n  COST ANALYSIS" << endl;
    cout << "============================================" << endl;
    cout << "Community Cost: Rs " << totalCost << endl;
    
    float gridCost = (requiredEnergy / 1000.0f) * 20.0f;
    cout << "Grid Cost: Rs " << gridCost << endl;
    
    float savings = gridCost - totalCost;
    cout << "\n  You SAVED: Rs " << savings << " (" 
         << (gridCost > 0 ? (savings / gridCost * 100) : 0) << "%)" << endl;
    
    cout << "\n############################################" << endl;
}


