#pragma once
#include "pacman.h"
#include "ghost.h"
#include "power_pellet.h"
#include <vector>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

enum GameState {
    CHASING_PELLET,
    EXITING_CENTER,
    POWERED_UP_CHASING,
    VULNERABLE_FLEEING,
    FINISHED
};

struct Wall {
    float x_min, x_max, z_min, z_max;
};

struct Scene {
    Pacman pacman;
    std::vector<Ghost> ghosts;
    PowerPellet powerPellet;
    std::vector<Wall> walls;

    GameState currentState;
    double totalTime;
    Ghost* targetGhost = nullptr;

    void init();
    void update(double deltaTime, GLFWwindow* window);
    void render();
    bool checkCollision(const Vec3& pos, float radius);
    Vec3 resolveCollision(const Vec3& currentPos, const Vec3& desiredVel, double deltaTime, float radius);
    Vec3 getAxisAligned(Vec3 dir);
    Vec3 getRandomDirection();
    Vec3 getTurnedLeft(const Vec3& dir);
    Vec3 getTurnedRight(const Vec3& dir);
};
