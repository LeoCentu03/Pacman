#pragma once
#include "vec3.h"
#include <GL/gl.h>

struct Ghost {
    Vec3 position;
    Vec3 velocity;
    Vec3 desiredVelocity;
    float speed;
    float flee_speed;
    bool eaten;
    float r, g, b;
    float radius;
    bool justHitWall = false;

    void init(float startX, float startZ, float red, float green, float blue);
    void update(double deltaTime);
    void draw(bool isPacmanPoweredUp);
};