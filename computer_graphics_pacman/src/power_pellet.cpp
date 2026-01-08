#include "power_pellet.h"
#include <GL/glu.h>

void PowerPellet::init(float x, float z) {
    position = Vec3(x, 1.0f, z);
    eaten = false;
}

void PowerPellet::draw() {
    if (eaten) return;
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glColor3f(1.0f, 0.8f, 0.6f);
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 0.4, 16, 16);
    gluDeleteQuadric(quad);
    glPopMatrix();
}