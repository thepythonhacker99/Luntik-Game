#pragma once

#include <math.h>
#include "Math.h"

namespace Luntik::Utils {
    struct vec2 {
        float x, y;

        vec2() {
            this->x = 0.0f;
            this->y = 0.0f;
        };

        vec2(float x, float y) {
            this->x = x;
            this->y = y;
        }

        vec2(sf::Vector2f v) {
            this->x = v.x;
            this->y = v.y;
        }

        vec2(sf::Vector2u v) {
            this->x = v.x;
            this->y = v.y;
        }

        vec2(sf::Vector2i v) {
            this->x = v.x;
            this->y = v.y;
        }

        static vec2 clamp(vec2 v, float minVal, float maxVal) {
            return vec2(std::max(std::min(v.x, maxVal), minVal), std::max(std::min(v.y, maxVal), minVal));
        }

        static vec2 normalize(vec2 v) {
            double s = Math::Q_rsqrt(v.x*v.x + v.y*v.y);
            sf::Vector2f r = { float(v.x * s), float(v.y * s) };
            return { v.x == 0 ? 0 : r.x, v.y == 0 ? 0 : r.y};
        }
        
        operator sf::Vector2f() {
            return sf::Vector2f(this->x, this->y);
        }

        operator std::string() {
            return "x: " + std::to_string(this->x) + " y: " + std::to_string(this->y);
        }

        vec2 operator+(const vec2& vec) {
            return vec2(x + vec.x, y + vec.y);
        }

        vec2 operator-(const vec2& vec) {
            return vec2(x - vec.x, y - vec.y);
        }

        vec2 operator*(const vec2& vec) {
            return vec2(x * vec.x, y * vec.y);
        }

        vec2 operator/(const vec2& vec) {
            return vec2(x / vec.x, y / vec.y);
        }


        vec2 operator+(float n) {
            return vec2(x + n, y + n);
        }

        vec2 operator-(float n) {
            return vec2(x - n, y - n);
        }

        vec2 operator*(float n) {
            return vec2(x * n, y * n);
        }

        vec2 operator/(float n) {
            return vec2(x / n, y / n);
        }

        ////////////////////////////////////////////////////////////////

        vec2& operator+=(const vec2& vec) {
            x += vec.x; y += vec.y;
            return *this;
        }

        vec2& operator-=(const vec2& vec) {
            x -= vec.x; y -= vec.y;
            return *this;
        }

        vec2& operator*=(const vec2& vec) {
            x *= vec.x; y *= vec.y;
            return *this;
        }

        vec2& operator/=(const vec2& vec) {
            x /= vec.x; y /= vec.y;
            return *this;
        }


        vec2& operator+=(float n) {
            x += n; y += n;
            return *this;
        }

        vec2& operator-=(float n) {
            x -= n; y -= n;
            return *this;
        }

        vec2& operator*=(float n) {
            x *= n; y *= n;
            return *this;
        }

        vec2& operator/=(float n) {
            x *= n; y *= n;
            return *this;
        }

        ////////////////////////////////////////////////////////////////

        bool operator==(float n) {
            return x == n && y == n;
        }

        bool operator!=(float n) {
            return x != n || y != n;
        }

        bool operator<(float n) {
            return x < n && y < n;
        }

        bool operator<=(float n) {
            return x <= n && y <= n;
        }

        bool operator>(float n) {
            return x > n && y > n;
        }

        bool operator>=(float n) {
            return x >= n && y >= n;
        }


        bool operator==(const vec2& v) {
            return x == v.x && y == v.y;
        }

        bool operator!=(const vec2& v) {
            return x != v.x || y != v.y;
        }

        bool operator<(const vec2& v) {
            return x < v.x && y < v.y;
        }

        bool operator<=(const vec2& v) {
            return x <= v.x && y <= v.y;
        }

        bool operator>(const vec2& v) {
            return x > v.x && y > v.y;
        }

        bool operator>=(const vec2& v) {
            return x >= v.x && y >= v.y;
        }

    };
}