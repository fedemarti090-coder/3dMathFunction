#pragma once

#include <cmath>

struct Point
{
    float x, y;
};

struct Vertex
{
    float x, y, z;
};

struct Vec3
{
    float x, y, z;
};

inline Vec3 operator-(const Vec3 &a, const Vec3 &b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
inline float dot(const Vec3 &a, const Vec3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline Vec3 cross(const Vec3 &a, const Vec3 &b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}
inline Vec3 normalize(const Vec3 &v)
{
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f)
        return {0.0f, 0.0f, 0.0f};
    return {v.x / length, v.y / length, v.z / length};
}

struct Mat4
{
    float m[16];

    Mat4()
    {
        for (int i = 0; i < 16; ++i)
            m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    float *operator[](int col)
    {
        return &m[col * 4];
    }

    const float *operator[](int col) const
    {
        return &m[col * 4];
    }
};

inline float radians(float degrees)
{
    return degrees * 3.14159265358979323846f / 180.0f;
}

inline Mat4 customLookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up)
{
    Vec3 f = normalize(center - eye);
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    Mat4 view;
    view[0][0] = s.x;
    view[1][0] = s.y;
    view[2][0] = s.z;
    view[3][0] = -dot(s, eye);
    view[0][1] = u.x;
    view[1][1] = u.y;
    view[2][1] = u.z;
    view[3][1] = -dot(u, eye);
    view[0][2] = -f.x;
    view[1][2] = -f.y;
    view[2][2] = -f.z;
    view[3][2] = dot(f, eye);
    view[0][3] = 0.0f;
    view[1][3] = 0.0f;
    view[2][3] = 0.0f;
    view[3][3] = 1.0f;

    return view;
}

Mat4 rotateX(float angle)
{
    Mat4 m; 
    float c = cos(angle);
    float s = sin(angle);

    m[1][1] =  c;
    m[2][1] = -s;
    m[1][2] =  s;
    m[2][2] =  c;

    return m;
}

Mat4 rotateY(float angle)
{
    Mat4 m;
    float c = cos(angle);
    float s = sin(angle);

    m[0][0] =  c;
    m[2][0] =  s;
    m[0][2] = -s;
    m[2][2] =  c;

    return m;
}

Mat4 rotateZ(float angle)
{
    Mat4 m;
    float c = cos(angle);
    float s = sin(angle);

    m[0][0] =  c;
    m[1][0] = -s;
    m[0][1] =  s;
    m[1][1] =  c;

    return m;
}

inline Mat4 operator*(const Mat4& a, const Mat4& b)
{
    Mat4 result;

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result[col][row] = 
                a[0][row] * b[col][0] +
                a[1][row] * b[col][1] +
                a[2][row] * b[col][2] +
                a[3][row] * b[col][3];
        }
    }

    return result;
}