#ifndef HISTORY_H
#define HISTORY_H

#include <string>
using namespace std;
// This stores information about one usage session. // A usage session = One time you turned a device ON and then OFF. 
struct HistoryRecord {
    string deviceID;
    string deviceName;
    float consumptionRate;
    int timestamp;
    int duration;
    float unitsConsumed;
    
    HistoryRecord() : consumptionRate(0), timestamp(0), duration(0), unitsConsumed(0) {}
    
    HistoryRecord(string id, string name, float rate, int ts, int dur, float units)
        : deviceID(id), deviceName(name), consumptionRate(rate), 
          timestamp(ts), duration(dur), unitsConsumed(units) {}
};

//Node for the Binary Search Tree.
class HistoryNode {
public:
    HistoryRecord data;
    HistoryNode* left;
    HistoryNode* right;
    
    HistoryNode(HistoryRecord record) : data(record), left(nullptr), right(nullptr) {}
};

class UsageHistoryBST {
private:
    HistoryNode* root;
    int nodeCount;

    // insert record based on timestamp
    HistoryNode* insertHelper(HistoryNode* node, HistoryRecord record) {
        if (node == nullptr) {
            nodeCount++;
            return new HistoryNode(record);
        }
        
        if (record.timestamp < node->data.timestamp) {
            node->left = insertHelper(node->left, record);
        } else {
            node->right = insertHelper(node->right, record);
        }
        return node;
    }
    
    //Gets ALL records from the tree in TIME ORDER (earliest to latest)
    void inorderHelper(HistoryNode* node, HistoryRecord* arr, int& index) {
        if (node == nullptr) return;
        inorderHelper(node->left, arr, index);
        arr[index++] = node->data;
        inorderHelper(node->right, arr, index);
    }
    
    void rangeQueryHelper(HistoryNode* node, int start, int end, HistoryRecord* arr, int& index) {
        if (node == nullptr) return;
        
        if (node->data.timestamp > start) {
            rangeQueryHelper(node->left, start, end, arr, index);
        }
        
        if (node->data.timestamp >= start && node->data.timestamp <= end) {
            arr[index++] = node->data;
        }
        
        if (node->data.timestamp < end) {
            rangeQueryHelper(node->right, start, end, arr, index);
        }
    }
    
    void destroyTree(HistoryNode* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
    
public:
    UsageHistoryBST() : root(nullptr), nodeCount(0) {}
    
    ~UsageHistoryBST() {
        destroyTree(root);
    }
    
    void insertRecord(HistoryRecord record) {
        root = insertHelper(root, record);
    }
    
    //Gets ALL records from the tree in TIME ORDER (earliest to latest)
    void getAllRecords(HistoryRecord* arr, int& size) {
        size = 0;
        inorderHelper(root, arr, size);
    }
    
    void getRecordsByTimeRange(int start, int end, HistoryRecord* arr, int& size) {
        size = 0;
        rangeQueryHelper(root, start, end, arr, size);
    }
    
    int getCount() const { return nodeCount; }
};

#endif // HISTORY_H


