#pragma once

#include "Transform.h"

namespace Luntik::Utils::Math {
    float Q_rsqrt( float number )
    {
        long i;
        float x2, y;
        const float threehalfs = 1.5F;

        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

        return y;
    }

    template<typename T>
    T interpolate(T srt, T end, T pos, T amt) { // AMT IS IN 0-1 PERCENT
        if (srt < end) {
            T amount = (end - srt) * amt;
            if (pos + amount < end) return pos + amount;
        } else if (srt > end) {
            T amount = (srt - end) * amt;
            if (pos - amount > end) return pos - amount;
        }

        return end;
    }

    int hash_function(int key) {
        // Convert the integer key to a string
        std::string str_key = std::to_string(key);

        // Initialize variables for hash computation
        int hash_value = 0;
        int prime = 31;
        int exponent = 1;

        // Compute hash value using Horner's method
        for (int i = str_key.size() - 1; i >= 0; i--) {
            hash_value += (str_key[i] - '0') * exponent;
            exponent *= prime;
        }

        // Reduce hash value to a smaller range
        return std::abs(hash_value % 1000000);
    }
}