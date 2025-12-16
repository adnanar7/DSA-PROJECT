#include <iostream>
#include <cassert>
#include "../priority_queue.h"

using namespace std;

int main() {
    cout << "[test_priority_queue] Running tests..." << endl;

    PriorityQueue pq;

    ScheduledTask low("D1", "Low", 10, 30, 1, false);
    ScheduledTask high("D2", "High", 12, 30, 10, true);
    ScheduledTask mid("D3", "Mid", 8, 30, 5, false);

    pq.enqueue(low);
    pq.enqueue(high);
    pq.enqueue(mid);

    assert(pq.getSize() == 3);

    ScheduledTask top = pq.peek();
    assert(top.deviceID == "D2"); // highest priority

    ScheduledTask first = pq.dequeue();
    assert(first.deviceID == "D2");
    assert(pq.getSize() == 2);

     // Edge case: same priority, earlier time should win
    PriorityQueue pq2;
    ScheduledTask t1("A", "A-task", 5, 10, 5, false);
    ScheduledTask t2("B", "B-task", 3, 10, 5, false); // same priority, earlier time
    pq2.enqueue(t1);
    pq2.enqueue(t2);
    ScheduledTask top2 = pq2.dequeue();
    assert(top2.deviceID == "B");

    // Edge case: dequeue/peek on empty queue should not crash
    PriorityQueue emptyPQ;
    assert(emptyPQ.isEmpty());
    ScheduledTask peekEmpty = emptyPQ.peek();   // returns default task
    ScheduledTask popEmpty  = emptyPQ.dequeue(); // returns default task
    (void)peekEmpty;
    (void)popEmpty;

    cout << "[test_priority_queue] All tests passed!" << endl;
    return 0;
}


