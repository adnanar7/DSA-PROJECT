#include <iostream>
#include <cassert>
#include <string>
#include "../hashmap.h"
#include "../device.h"

using namespace std;

void test_int_hashmap() {
    HashMap<string, int> m;
    m.insert("one", 1);
    m.insert("two", 2);
    m.insert("three", 3);

    assert(m.size() == 3);
    assert(*m.get("one") == 1);
    assert(*m.get("two") == 2);
    assert(*m.get("three") == 3);

    // overwrite
    m.insert("two", 22);
    assert(*m.get("two") == 22);

    // remove
    assert(m.remove("one") == true);
    assert(m.get("one") == nullptr);
    assert(m.size() == 2);
}

void test_hashmap_edge_cases() {
    HashMap<string, int> m;

    // get/remove on empty map
    assert(m.get("missing") == nullptr);
    assert(m.remove("missing") == false);
    assert(m.size() == 0);

    // insert a few, then remove non-existent key
    m.insert("a", 10);
    m.insert("b", 20);
    assert(m.size() == 2);
    assert(m.remove("c") == false);
    assert(m.size() == 2);

    // multiple inserts of same key
    m.insert("a", 11);
    m.insert("a", 12);
    int* v = m.get("a");
    assert(v && *v == 12);
}

void test_device_pointer_hashmap() {
    HashMap<string, Device*> m;
    Device* d1 = new Device("D1", "Fan", 60);
    Device* d2 = new Device("D2", "Light", 20);

    m.insert("D1", d1);
    m.insert("D2", d2);

    int size = 0;
    Device* arr[10];
    m.getAllValues(arr, size);
    assert(size == 2);

    Device** got = m.get("D1");
    assert(got && *got == d1);

    // cleanup allocated devices
    delete d1;
    delete d2;
}

int main() {
    cout << "[test_hashmap] Running tests..." << endl;
    test_int_hashmap();
    test_hashmap_edge_cases();
    test_device_pointer_hashmap();
    cout << "[test_hashmap] All tests passed!" << endl;
    return 0;
}


