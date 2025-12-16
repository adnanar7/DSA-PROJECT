#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <iostream>
#include <string>
using namespace std;

struct ScheduledTask {
    string deviceID;
    string deviceName;
    int scheduledTime;      // Hour (0-23)
    int scheduledMinute;    // NEW: Minute (0-59)
    int duration;
    int priority;
    bool isCritical;
    float estimatedCost;
    
    ScheduledTask() : scheduledTime(0), scheduledMinute(0), duration(0), 
                      priority(5), isCritical(false), estimatedCost(0) {}
    
    ScheduledTask(string id, string name, int time, int minute, int dur, int prio, bool crit)
        : deviceID(id), deviceName(name), scheduledTime(time), scheduledMinute(minute),
          duration(dur), priority(prio), isCritical(crit), estimatedCost(0) {}
};

class PriorityQueue {
private:
    ScheduledTask* heap;
    int capacity;
    int size;
    
    void swap(ScheduledTask& a, ScheduledTask& b) {
        ScheduledTask temp = a;
        a = b;
        b = temp;
    }
    
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }
    
    void heapifyUp(int index) {
        while (index > 0) {
            int p = parent(index);
            if (heap[index].priority > heap[p].priority ||
                (heap[index].priority == heap[p].priority && 
                 (heap[index].scheduledTime < heap[p].scheduledTime ||
                  (heap[index].scheduledTime == heap[p].scheduledTime && 
                   heap[index].scheduledMinute < heap[p].scheduledMinute)))) {
                swap(heap[index], heap[p]);
                index = p;
            } else {
                break;
            }
        }
    }
    
    void heapifyDown(int index) {
        while (true) {
            int largest = index;
            int left = leftChild(index);
            int right = rightChild(index);
            
            if (left < size && 
                (heap[left].priority > heap[largest].priority ||
                 (heap[left].priority == heap[largest].priority && 
                  (heap[left].scheduledTime < heap[largest].scheduledTime ||
                   (heap[left].scheduledTime == heap[largest].scheduledTime && 
                    heap[left].scheduledMinute < heap[largest].scheduledMinute))))) {
                largest = left;
            }
            
            if (right < size && 
                (heap[right].priority > heap[largest].priority ||
                 (heap[right].priority == heap[largest].priority && 
                  (heap[right].scheduledTime < heap[largest].scheduledTime ||
                   (heap[right].scheduledTime == heap[largest].scheduledTime && 
                    heap[right].scheduledMinute < heap[largest].scheduledMinute))))) {
                largest = right;
            }
            
            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }
    
public:
    PriorityQueue(int cap = 100) : capacity(cap), size(0) {
        heap = new ScheduledTask[capacity];
    }
    
    ~PriorityQueue() {
        delete[] heap;
    }
    
    void enqueue(ScheduledTask task) {
        if (size >= capacity) {
            cout << "Priority Queue is full!" << endl;
            return;
        }
        heap[size] = task;
        heapifyUp(size);
        size++;
    }
    
    ScheduledTask dequeue() {
        if (size == 0) {
            cout << "Priority Queue is empty!" << endl;
            return ScheduledTask();
        }
        
        ScheduledTask result = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        return result;
    }
    
    ScheduledTask peek() {
        if (size == 0) {
            cout << "Priority Queue is empty!" << endl;
            return ScheduledTask();
        }
        return heap[0];
    }
    
    bool isEmpty() { return size == 0; }
    int getSize() { return size; }
    
    void display() {
        if (size == 0) {
            cout << "No scheduled tasks." << endl;
            return;
        }
        
        cout << "\n===== Scheduled Tasks =====" << endl;
        for (int i = 0; i < size; i++) {
            cout << i + 1 << ". " << heap[i].deviceName 
                 << " | Priority: " << heap[i].priority
                 << (heap[i].isCritical ? " [CRITICAL]" : "")
                 << " | Time: " << heap[i].scheduledTime << ":" 
                 << (heap[i].scheduledMinute < 10 ? "0" : "") << heap[i].scheduledMinute
                 << " | Duration: " << heap[i].duration << " min" << endl;
        }
    }
};

#endif // PRIORITY_QUEUE_H


