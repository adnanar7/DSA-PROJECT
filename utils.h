#ifndef UTILS_H
#define UTILS_H

#include <string>
using namespace std;

// Hash function for strings
inline unsigned int hashString(const string& str) {
    unsigned int h = 0;
    for (size_t i = 0; i < str.length(); i++) {
        h = h * 31 + str[i];                // //prime number 31 is used to reduce collisions
    }
    return h;
}

#endif // UTILS_H


