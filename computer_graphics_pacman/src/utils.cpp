#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <cmath>
#include "utils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawPacman(float radius, float mouthAngle, float mouthRotation) {
    GLfloat mat[] = {1.0f, 0.9f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, radius, 48, 48);
    gluDeleteQuadric(quad);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, radius - 0.01f);
    glRotatef(mouthRotation, 0, 1, 0);
    float half = mouthAngle * 0.5f;
    int segments = 40;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0, 0, 0);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float a = -half + (i / (float) segments) * mouthAngle;
        float x = cos(a) * radius * 1.02f;
        float y = sin(a) * radius * 1.02f;
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}
