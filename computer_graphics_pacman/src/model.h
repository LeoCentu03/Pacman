#pragma once
#include <vector>
#include <string>

struct Model {
    std::vector<float> vertices;
    std::vector<float> normals;

    bool load(const std::string& filename);
    void draw();
};