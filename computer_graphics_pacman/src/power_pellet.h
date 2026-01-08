#pragma once
#include "vec3.h"

struct PowerPellet {
    Vec3 position;
    bool eaten;

    void init(float x, float z);
    void draw();
};