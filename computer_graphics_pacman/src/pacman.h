#pragma once
#include "vec3.h"
#include "model.h"

struct Pacman {
    Vec3 position;
    Vec3 startPosition = Vec3(-12.0f, 1.0f, 0.0f);
    Vec3 velocity;
    Vec3 desiredVelocity;
    float speed;
    float flee_speed_boost;
    bool poweredUp;
    float radius;
    bool justHitWall = false; 

    Model pacmanModel;

    void init();
    void update(double deltaTime);
    void draw();
};