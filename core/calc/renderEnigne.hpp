#include <iostream>
#include <vector>

#include "mathLib.hpp"

class RenderEnigne
{
private:
    Vec3 center = {0, 0, 0};

    float W = 1200.0f;
    float H = 800.0f;

    float FOV = 2.0f;

public:
    std::vector<Point> processVertex(const std::vector<Vertex> &inputVertex)
    {
        std::vector<Point> outputVertex;
        outputVertex.reserve(inputVertex.size());

        for (const auto &v : inputVertex)
        {
            float x = v.x;
            float y = v.y;
            float z = v.z;

            float x_ = FOV * x / z;
            float y_ = FOV * y / z;

            x_ = (x_ + 1.0f) * 0.5f * W;
            y_ = (1.0f - (y_ + 1.0f) * 0.5f) * H;

            outputVertex.push_back({x_, y_});
        }

        return outputVertex;
    }
};