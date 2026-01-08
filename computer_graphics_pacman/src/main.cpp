#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "scene.h"
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "Errore inizializzazione GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Pacman 3D Animation", NULL, NULL);
    if (!window) {
        std::cerr << "Errore creazione finestra GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1280.0 / 720.0, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);

    Scene scene;
    scene.init();

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        if (deltaTime > 0.1) deltaTime = 0.1;
        if (deltaTime <= 0.0) deltaTime = 0.001;
        lastTime = currentTime;

        scene.update(deltaTime, window);

        glLoadIdentity();
        gluLookAt(0.0, 35.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        scene.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}