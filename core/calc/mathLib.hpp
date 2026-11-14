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

struct Mat4
{
    float m[4][4] = {0};

    static Mat4 identity()
    {
        Mat4 res;
        res.m[0][0] = 1.0f;
        res.m[1][1] = 1.0f;
        res.m[2][2] = 1.0f;
        res.m[3][3] = 1.0f;
        return res;
    }

    Mat4 operator*(const Mat4 &o) const
    {
        Mat4 res;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                res.m[i][j] = 0;
                for (int k = 0; k < 4; k++)
                {
                    res.m[i][j] += m[i][k] * o.m[k][j];
                }
            }
        }

        return res;
    }
};
