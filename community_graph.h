#ifndef COMMUNITY_GRAPH_H
#define COMMUNITY_GRAPH_H

#include <iostream>
#include <string>
#include "hashmap.h"
using namespace std;

struct Home {
    string homeID;
    string address;
    float currentProduction;
    float currentConsumption;
    float excessEnergy;
    float batteryLevel;
    
    Home() : currentProduction(0), currentConsumption(0), 
             excessEnergy(0), batteryLevel(0) {}
    
    Home(string id, string addr, float prod, float cons, float battery)
        : homeID(id), address(addr), currentProduction(prod), 
          currentConsumption(cons), batteryLevel(battery) {
        excessEnergy = prod - cons;
    }
    
    void updateEnergy() {   // Recalculate excess energy
        excessEnergy = currentProduction - currentConsumption;
    }
};

struct GraphEdge {
    string neighborID;
    float distance;
    GraphEdge* next;
    
    GraphEdge(string id, float dist) : neighborID(id), distance(dist), next(nullptr) {}
};

class BFSQueue {
private:
    string data[100];
    int front, rear;
    
public:
    BFSQueue() : front(0), rear(0) {}
    
    void enqueue(string item) {
        if (rear < 100) {
            data[rear++] = item;
        }
    }
    
    string dequeue() {
        if (front < rear) {
            return data[front++];
        }
        return "";
    }
    
    bool isEmpty() {
        return front >= rear;
    }
    
    int getSize() {
        return rear - front;
    }
};

class CommunityGraph {
private:
    HashMap<string, Home*> homes;
    HashMap<string, GraphEdge*> adjacencyList;
    int homeCount;
    
    int hashHomeID(const string& id) {
        unsigned int h = hashString(id);
        return h % 100;
    }
    
public:
    CommunityGraph() : homeCount(0) {}
    
    ~CommunityGraph() {
        string keys[100];
        int keyCount;
        adjacencyList.getAllKeys(keys, keyCount);
        
        for (int i = 0; i < keyCount; i++) {
            GraphEdge** edgePtr = adjacencyList.get(keys[i]);
            if (edgePtr && *edgePtr) {
                GraphEdge* current = *edgePtr;
                while (current != nullptr) {
                    GraphEdge* temp = current;
                    current = current->next;
                    delete temp;
                }
            }
        }
    }
    
    void addHome(Home* home) {
        homes.insert(home->homeID, home);
        adjacencyList.insert(home->homeID, nullptr);
        homeCount++;
    }
    
    void connectHomes(string home1, string home2, float distance) {
        GraphEdge** list1 = adjacencyList.get(home1);
        if (list1) {
            GraphEdge* newEdge = new GraphEdge(home2, distance);
            newEdge->next = *list1;
            *list1 = newEdge;
        }
        
        GraphEdge** list2 = adjacencyList.get(home2);
        if (list2) {
            GraphEdge* newEdge = new GraphEdge(home1, distance);
            newEdge->next = *list2;
            *list2 = newEdge;
        }
    }
    
    // ← NEW METHOD: Get a specific home
    Home** getHome(string homeID) {
        return homes.get(homeID);
    }
    
    void findAllNeighborsBFS(string startHomeID, string* neighbors, int& count);
    
    float findCheapestPath(string startHome, string targetHome, string* path, int& pathLength);
    
    void displayCommunityStatus();
    
    void findEnergySharing(string requestingHomeID, float requiredEnergy);
};

#endif // COMMUNITY_GRAPH_H


