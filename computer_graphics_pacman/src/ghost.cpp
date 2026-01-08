#include "ghost.h"
#include <GL/glu.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Ghost::init(float startX, float startZ, float red, float green, float blue) {
    position = Vec3(startX, 1.0f, startZ);
    velocity = Vec3(0.0f, 0.0f, 0.0f);
    desiredVelocity = Vec3(0.0f, 0.0f, 0.0f);
    speed = 5.0f;
    flee_speed = 4.0f;
    eaten = false;
    r = red;
    g = green;
    b = blue;
    radius = 0.6f;
}

void Ghost::update(double deltaTime) {
    if (eaten) return;
    position.y = 1.0f;
}

void Ghost::draw(bool isPacmanPoweredUp) {
    if (eaten) return;
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

     if (velocity.lengthSq() > 0.1f * 0.1f) {
        float angle = atan2(velocity.x, velocity.z) * (180.0 / M_PI);
        glRotatef(angle + 90.0f, 0.0f, 1.0f, 0.0f);
    } else {
         glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    }


    if (isPacmanPoweredUp) {
        glColor3f(0.0f, 0.0f, 1.0f);
    } else {
        glColor3f(r, g, b);
    }

    GLUquadric* quad = gluNewQuadric();

    glPushMatrix();
    glTranslatef(0, -0.4f, 0);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quad, radius, radius, 0.8f, 16, 1);
    glPopMatrix();
    gluSphere(quad, radius, 16, 16);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(radius*0.4f, radius*0.3f, radius*0.8f);
    gluSphere(quad, radius*0.25f, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-radius*0.4f, radius*0.3f, radius*0.8f);
    gluSphere(quad, radius*0.25f, 8, 8);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(radius*0.4f, radius*0.3f, radius*0.9f);
    gluSphere(quad, radius*0.13f, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-radius*0.4f, radius*0.3f, radius*0.9f);
    gluSphere(quad, radius*0.13f, 8, 8);
    glPopMatrix();

    gluDeleteQuadric(quad);
    glPopMatrix();
}