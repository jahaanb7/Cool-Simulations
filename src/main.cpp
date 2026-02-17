#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>
#include "../include/particle.h"

const int WIDTH = 800;
const int HEIGHT = 600;

void drawParticle(const Particle& p, int numSegments) {

    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < numSegments; i++){

        float theta = 2.0f * glm::pi<float>() * float(i) / float(numSegments);

        float x = p.radius * cos(theta);
        float y = p.radius * sin(theta);

        glVertex2f(x + p.position.x, y + p.position.y);
    }
    glEnd();
}

int main(void)
{
    if (!glfwInit()){
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Space Simulation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);  // Left, Right, Bottom, Top

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    glm::vec2 ParticlePosition(400.0f, 300.0f);
    glm::vec2 ParticleVelocity = glm::vec2(30.0f, 0.0f);

    Particle particle(ParticlePosition, ParticleVelocity, 1.0f, 50.0f);

    float deltaTime = 0.0167f;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        drawParticle(particle, 50);

        particle.update(deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

