#include <iostream>
#include <vector>

#include "uiStruct.hpp"

class UiEngine
{
private:
    std::vector<UIVertex_> vertices;
    std::vector<uint32_t> indices;

    float width = 0.0f;
    float heigth = 0.0f;

public:
    void init(float W, float H)
    {
        width = W;
        heigth = H;
    }

    void beginFrame()
    {
        vertices.clear();
        indices.clear();
    }

    void Rect(float x_, float y_, float w_, float h_, RGBa_ color)
    {
        float w = width * w_;
        float h = heigth * h_;
        float x = width * x_;
        float y = heigth * y_;

        uint32_t startIndex = static_cast<uint32_t>(vertices.size());
        // I 4 Vertici
        vertices.push_back({x, y, color.r, color.g, color.b, color.a});
        vertices.push_back({x + w, y, color.r, color.g, color.b, color.a});
        vertices.push_back({x + w, y + h, color.r, color.g, color.b, color.a});
        vertices.push_back({x, y + h, color.r, color.g, color.b, color.a});
        // 1 Triangolo
        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);
        // 2 triangolo
        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
    }

    const std::vector<UIVertex_> &getVertices() { return vertices; }
    const std::vector<uint32_t> &getIndices() { return indices; }
};