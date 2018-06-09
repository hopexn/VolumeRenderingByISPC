#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <cassert>
#include <immintrin.h>

template<class T>
struct Vector2 {
public:
    Vector2() : x(0), y(0) {}

    Vector2(T _x, T _y) : x(_x), y(_y) {}

    inline void set(T _x, T _y) {
        x = _x;
        y = _y;
    }

    inline bool zero() {
        return (x == 0) && (y == 0);
    }

    inline float norm() {
        return sqrtf(x * x + y * y);
    }

    inline Vector2 normalize() {
        return *this / norm();
    }

    inline Vector2 operator+(const Vector2 &v) const {
        return {x + v.x, y + v.y};
    }

    inline Vector2 operator-(const Vector2 &v) const {
        return {x - v.x, y - v.y};
    }

    inline Vector2 operator-() const {
        return {-x, -y};
    }

    inline Vector2 operator*(float v) const {
        return {x * v, y * v};
    }

    inline Vector2 operator/(float v) const {
        assert(v);
        return *this * (1 / v);
    }

    inline float dot(const Vector2 &v) const {
        return x * v.x + y * v.y;
    }

    inline Vector2 &operator+=(const Vector2 &v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    inline Vector2 &operator-=(const Vector2 &v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline Vector2 &operator*=(float v) {
        x *= v;
        y *= v;
        return *this;
    }

    inline Vector2 &operator/=(float v) {
        assert(v);
        return *this *= (1 / v);
    }

    T x, y;
};

typedef Vector2<char> Vector2c;
typedef Vector2<int> Vector2i;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

template<class T>
struct Vector3 {
public:
    Vector3() : x(0), y(0), z(0) {}

    Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

    inline void set(T _x, T _y, T _z) {
        x = _x;
        y = _y;
        z = _z;
    }

    inline Vector3 operator-() const {
        return {-x, -y, -z};
    }

    inline Vector3 operator+(const Vector3 &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    inline Vector3 operator-(const Vector3 &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    inline Vector3 operator*(float v) const {
        return {x * v, y * v, z * v};
    }

    inline Vector3 operator/(float v) const {
        assert(v);
        return {x / v, y / v, z / v};
    }

    inline Vector3 &operator+=(const Vector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline Vector3 &operator-=(const Vector3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    inline Vector3 &operator*=(float v) {
        x *= v;
        y *= v;
        z *= v;
        return *this;
    }

    inline Vector3 &operator/=(float v) {
        assert(v);
        return *this *= (1 / v);
    }

    inline bool zero() {
        return (x == 0) && (y == 0) && (z == 0);
    }

    inline float norm() {
        return sqrtf(x * x + y * y + z * z);
    }

    inline Vector3 normalize() {
        return *this / norm();
    }

    inline float dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    inline Vector3 cross(const Vector3 &v) const {
        return {y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x};
    }

    inline Vector3 rotate(const Vector3 &axis, float theta) {
        const float cos_theta = cosf(theta);
        const float dot = this->dot(axis);
        Vector3 cross = this->cross(axis);
        Vector3 result = *this * cos_theta;
        result += axis * (dot * (1.0f - cos_theta));
        result -= cross * sin(theta);
        return result;
    }

    inline Vector3 rotate(const Vector3 &origin, const Vector3 &axis, float theta) {
        Vector3 tv = *this - origin;
        Vector3 result = tv.rotate(axis, theta);
        result += origin;
        return result;
    }

    inline T max() {
        return (x > y) ? (x > z ? x : z) : (y > z ? y : z);
    }

    T x, y, z;
};

typedef Vector3<char> Vector3c;
typedef Vector3<int> Vector3i;
typedef Vector3<double> Vector3d;

union __M128 {
    struct {
        float x, y, z, w;
    };
    __m128 xyzw;
};

struct Vector3f {
public:
    inline Vector3f() {}

    inline Vector3f(__m128 _data) {
        data = _data;
    }

    inline Vector3f(float _x, float _y, float _z) {
        data = _mm_set_ps(1, _z, _y, _x);
    }

    inline void set(float _x, float _y, float _z) {
        data = _mm_set_ps(1, _z, _y, _x);
    }

    inline Vector3f operator-() const {
        __m128 zero = _mm_setzero_ps();
        return {_mm_sub_ps(zero, data)};
    }

    inline Vector3f operator+(const Vector3f &v) const {
        return {_mm_add_ps(data, v.data)};
    }

    inline Vector3f operator-(const Vector3f &v) const {
        return {_mm_sub_ps(data, v.data)};
    }

    inline Vector3f operator*(float v) const {
        __m128 v2 = _mm_set_ps(1, v, v, v);
        return {_mm_mul_ps(data, v2)};
    }

    inline Vector3f operator/(float v) const {
        assert(v);
        return *this * (1 / v);
    }

    inline Vector3f &operator+=(const Vector3f &v) {
        data = _mm_add_ps(data, v.data);
        return *this;
    }

    inline Vector3f &operator-=(const Vector3f &v) {
        data = _mm_sub_ps(data, v.data);
        return *this;
    }

    inline Vector3f &operator=(const Vector3i &v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    inline Vector3f &operator*=(float v) {
        __m128 v2 = _mm_set_ps(1, v, v, v);
        data = _mm_mul_ps(data, v2);
        return *this;
    }

    inline Vector3f &operator/=(float v) {
        assert(v);
        return *this *= (1 / v);
    }

    inline bool zero() {
        return (x == 0) && (y == 0) && (z == 0);
    }

    inline float norm() {
        __M128 res;
        res.xyzw = _mm_mul_ps(data, data);
        return sqrtf(res.x + res.y + res.z);
    }

    inline Vector3f normalize() {
        return *this / norm();
    }

    inline float dot(const Vector3f &v) const {
        __M128 res;
        res.xyzw = _mm_mul_ps(data, v.data);
        return res.x + res.y + res.z;
    }

    inline Vector3f cross(const Vector3f &v) const {
        return {y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x};
    }

    inline Vector3f rotate(const Vector3f &axis, float theta) {
        const float cos_theta = cosf(theta);
        const float dot = this->dot(axis);
        const Vector3f cross = this->cross(axis);
        Vector3f result = *this * cos_theta;
        result += axis * (dot * (1.0f - cos_theta));
        result -= cross * sinf(theta);
        return result;
    }

    inline Vector3f rotate(const Vector3f &origin, const Vector3f &axis, float theta) {
        Vector3f tv = *this - origin;
        Vector3f result = tv.rotate(axis, theta);
        result += origin;
        return result;
    }

    union {
        struct {
            float x, y, z, w;
        };
        __m128 data;
    };
};

struct Vector4f {
    Vector4f() {}

    Vector4f(__m128 _data) : data(_data) {}

    Vector4f(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w) {}

    Vector4f(const Vector4f &v) : data(v.data) {}

    inline Vector4f operator-() const {
        __m128 zero = _mm_setzero_ps();
        return {_mm_sub_ps(zero, data)};
    }

    inline Vector4f operator+(const Vector4f &v) const {
        return {_mm_add_ps(data, v.data)};
    }

    inline Vector4f operator-(const Vector4f &v) const {
        return {_mm_sub_ps(data, v.data)};
    }

    inline Vector4f operator*(float v) const {
        __m128 v2 = _mm_set_ps(v, v, v, v);
        return {_mm_mul_ps(data, v2)};
    }

    inline Vector4f operator/(float v) const {
        assert(v);
        return *this * (1 / v);
    }

    inline Vector4f &operator+=(const Vector4f &v) {
        data = _mm_add_ps(data, v.data);
        return *this;
    }

    inline Vector4f &operator-=(const Vector4f &v) {
        data = _mm_sub_ps(data, v.data);
        return *this;
    }

    inline Vector4f &operator*=(float v) {
        __m128 v2 = _mm_set_ps(v, v, v, v);
        data = _mm_mul_ps(data, v2);
        return *this;
    }

    inline Vector4f &operator/=(float v) {
        assert(v);
        return *this *= (1 / v);
    }

    inline bool zero() {
        return (x == 0) && (y == 0) && (z == 0) && (w == 0);
    }

    union {
        struct {
            float x, y, z, w;
        };
        __m128 data;
    };
};


#endif //VECTOR_H