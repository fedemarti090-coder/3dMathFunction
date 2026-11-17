#pragma once
#include <string>

struct RGB
{
    float R, G, B;
};

struct DataGet
{
    std::string function;
    float dominioMax;
    float dominioMin;
    float step;
    float rotateX;
    float rotateY;
    float rotateZ;
    RGB col1;
    RGB col2;
    float alpha;
    bool asseX;
    bool asseY;
    bool asseZ;
    bool Superfici0;
};

struct DataGive
{
    float rotateX;
    float rotateY;
};