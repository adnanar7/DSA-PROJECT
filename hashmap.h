#ifndef HASHMAP_H
#define HASHMAP_H

#include <cstring>
#include <cstdlib>
#include <ctime>
#include "utils.h"
using namespace std;

//This is a helper class for the HashMap - think of it as a container.
template<typename K, typename V>
class HashNode {
public:
    K key;
    V value;
    HashNode* next;     // Pointer to next node (for collision handling) // If two devices hash to the same position, they form a linked list:
    
    HashNode(K k, V v) : key(k), value(v), next(nullptr) {}
};

//This stores all devices for ultra-fast lookup.
template<typename K, typename V>
class HashMap {
private:
    int table_size;
    double load_factor_threshold;
    unsigned long long a, b;
    const unsigned long long P = 1000000007ULL;
    HashNode<K, V>** table;
    int count;
    
    unsigned int hashFunction(const string& key) const {   //Universal hashing 
        unsigned long long h = hashString(key);
        unsigned long long res = ((a * h + b) % P);
        return res % table_size;
    }
    
public:
    HashMap() : count(0), table_size(16), load_factor_threshold(0.75) {   // Dynamic table size (starts small)
        srand(time(0));
        a = (unsigned long long)rand() % P + 1;
        b = (unsigned long long)rand() % P;
        table = new HashNode<K, V>*[table_size];    // create array of 16 HashNode pointers
        for (int i = 0; i < table_size; i++) {    // 16 linkedLists , 1LL at each index
            table[i] = nullptr;
        }
    }
    
    ~HashMap() {
        for (int i = 0; i < table_size; i++) {
            HashNode<K, V>* entry = table[i];
            while (entry != nullptr) {
                HashNode<K, V>* prev = entry;
                entry = entry->next;
                delete prev;
            }
        }
        delete[] table;
    }
    
    void resize(int new_size) {
        HashNode<K, V>** new_table = new HashNode<K, V>*[new_size];
        for (int i = 0; i < new_size; i++) {
            new_table[i] = nullptr;
        }
        for (int i = 0; i < table_size; i++) {
            HashNode<K, V>* entry = table[i];
            while (entry != nullptr) {
                unsigned long long h = hashString(entry->key);
                unsigned long long res = ((a * h + b) % P);
                unsigned int new_hash = res % new_size;
                HashNode<K, V>* next = entry->next;
                entry->next = new_table[new_hash];
                new_table[new_hash] = entry;
                entry = next;
            }
        }
        delete[] table;
        table = new_table;
        table_size = new_size;
    }
    
    void insert(K key, V value) {
        unsigned int hash = hashFunction(key);
        HashNode<K, V>* entry = table[hash];
        
        while (entry != nullptr) {
            if (entry->key == key) {
                entry->value = value;
                return;
            }
            entry = entry->next;
        }
        
        entry = new HashNode<K, V>(key, value);
        entry->next = table[hash];
        table[hash] = entry;
        count++;
        if ((double)count / table_size > load_factor_threshold) {
            resize(table_size * 2);
        }
    }
    
    V* get(K key) {
        unsigned int hash = hashFunction(key);
        HashNode<K, V>* entry = table[hash];
        
        while (entry != nullptr) {
            if (entry->key == key) {
                return &(entry->value);
            }
            entry = entry->next;
        }
        return nullptr;
    }
    
    bool remove(K key) {
        unsigned int hash = hashFunction(key);
        HashNode<K, V>* entry = table[hash];
        HashNode<K, V>* prev = nullptr;
        
        while (entry != nullptr) {
            if (entry->key == key) {
                if (prev == nullptr) {
                    table[hash] = entry->next;
                } else {
                    prev->next = entry->next;
                }
                delete entry;
                count--;
                return true;
            }
            prev = entry;
            entry = entry->next;
        }
        return false;
    }
    
    bool contains(K key) {
        return get(key) != nullptr;
    }
    
    int size() const { return count; }
    
    double loadFactor() const { return (double)count / table_size; }
    
    // Now fully generic: works for any V
    void getAllValues(V* arr, int& size) {
        size = 0;
        for (int i = 0; i < table_size; i++) {
            HashNode<K, V>* entry = table[i];
            while (entry != nullptr) {
                arr[size++] = entry->value;
                entry = entry->next;
            }
        }
    }
    
    void getAllKeys(K* arr, int& size) {
        size = 0;
        for (int i = 0; i < table_size; i++) {
            HashNode<K, V>* entry = table[i];
            while (entry != nullptr) {
                arr[size++] = entry->key;
                entry = entry->next;
            }
        }
    }
};

#endif // HASHMAP_H


