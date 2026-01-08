#pragma once
#include <cmath>

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
     float lengthSq() const {
        return x * x + y * y + z * z;
    }

    void normalize() {
        float len = length();
        if (len > 0.0001f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }
     Vec3 normalized() const {
        Vec3 result = *this;
        result.normalize();
        return result;
    }
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) {
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vec3 operator-(const Vec3& a, const Vec3& b) {
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vec3 operator*(const Vec3& a, float s) {
    return Vec3(a.x * s, a.y * s, a.z * s);
}

inline Vec3 operator/(const Vec3& a, float s) {
    if (std::abs(s) < 0.0001f) return Vec3();
    return Vec3(a.x / s, a.y / s, a.z / s);
}