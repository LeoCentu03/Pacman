#include "pacman.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Pacman::init() {
    position = startPosition;
    velocity = Vec3(0.0f, 0.0f, 0.0f);
    desiredVelocity = Vec3(0.0f, 0.0f, 0.0f);
    speed = 6.0f;
    flee_speed_boost = 7.0f;
    poweredUp = false;
    radius = 0.3f;

    if (!pacmanModel.load("build/Pac-Man.obj")) {
        std::cerr << "Errore: Impossibile caricare il modello Pac-Man.obj" << std::endl;
    }
}

void Pacman::update(double deltaTime) {
    position.y = 1.0f;
}

void Pacman::draw() {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    if (velocity.lengthSq() > 0.1f * 0.1f) {
        float angle = atan2(velocity.x, velocity.z) * (180.0 / M_PI);
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
    } else {
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    }

    if(poweredUp) {
        glColor3f(0.0f, 1.0f, 0.0f);
    } else {
        glColor3f(1.0f, 1.0f, 0.0f);
    }

    if (pacmanModel.vertices.empty()) {
        GLUquadric* quad = gluNewQuadric();
        gluSphere(quad, radius, 32, 32);
        gluDeleteQuadric(quad);
    } else {
        glScalef(0.8f, 0.8f, 0.8f);
        pacmanModel.draw();
    }

    glPopMatrix();
}