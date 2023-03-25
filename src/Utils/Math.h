#pragma once

namespace Luntik::Utils::Math {
    static float Q_rsqrt( float number )
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
}