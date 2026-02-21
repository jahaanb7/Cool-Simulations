#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>
#include "library/Particle.h"
#include "library/Particle3D.h"
#include <vector>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

// screen size vars
const float WIDTH = 800.0f;
const float HEIGHT = 600.0f;

float elapsedTime = 0.0f;
float deltaTime = 0.0167;

float TimeDelay = 0.5f;

bool mouseMode = false; 

std::vector<Particle3D> particles;
std::vector<float> spawnTimes;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

Particle mouseMovement(GLFWwindow* window, Particle& particle){
    double xpos; 
    double ypos;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwGetCursorPos(window, &xpos, &ypos);

    float xPosMouse = static_cast<float>(xpos);
    float yPosMouse = HEIGHT - static_cast<float>(ypos);

    glm::vec2 mousePos(xPosMouse, yPosMouse);
    glm::vec2 direction = mousePos - particle.position;

    particle.velocity = direction * particle.velocity;

    return particle;
}

void drawBoundaryBox(float width, float height, float depth) {
    float x = width / 2.0f;
    float y = height / 2.0f;
    float z = depth; 

    glDisable(GL_LIGHTING);
    glColor3f(1.0f,1.0f,1.0f); 

    glBegin(GL_LINES);

    glVertex3f(-x, -y, -z); glVertex3f( x, -y, -z);
    glVertex3f( x, -y, -z); glVertex3f( x,  y, -z);
    glVertex3f( x,  y, -z); glVertex3f(-x,  y, -z);
    glVertex3f(-x,  y, -z); glVertex3f(-x, -y, -z);

    glVertex3f(-x, -y,  z); glVertex3f( x, -y,  z);
    glVertex3f( x, -y,  z); glVertex3f( x,  y,  z);
    glVertex3f( x,  y,  z); glVertex3f(-x,  y,  z);
    glVertex3f(-x,  y,  z); glVertex3f(-x, -y,  z);

    glVertex3f(-x, -y, -z); glVertex3f(-x, -y,  z);
    glVertex3f( x, -y, -z); glVertex3f( x, -y,  z);
    glVertex3f( x,  y, -z); glVertex3f( x,  y,  z);
    glVertex3f(-x,  y, -z); glVertex3f(-x,  y,  z);

    glEnd();

    glEnable(GL_LIGHTING);
}

void drawBoundarySphere(int lats, int longs, int radius){
    glDisable(GL_LIGHTING);
    glColor3f(1.0f,1.0f,1.0f);

    glBegin(GL_POINTS);
    for(int i = 0; i <= lats; i++)
    {
        float theta = i * glm::pi<float>() / lats;
        for(int j = 0; j <= longs; j++)
        {
            float phi = j * 2 * glm::pi<float>() / longs;

            float x = radius * sin(theta) * cos(phi);
            float y = radius * sin(theta) * sin(phi);
            float z = radius * cos(theta);

            glVertex3f(x, y, z);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawParticleArray2D(std::vector<Particle>& particles, float deltaTime){
    for(auto& p : particles) {        
        p.updatePosition(deltaTime);
        p.boundingBoundary(WIDTH, HEIGHT);
        p.drawParticle();
    }
}

void drawParticleArray3D(std::vector<Particle3D>& particles, float deltaTime){
    for(int i = 0; i < particles.size(); i++){
        if(elapsedTime >= spawnTimes[i]){
            particles[i].updatePosition3D(deltaTime);

            for(int j = i+1; j < particles.size(); j++){
                particles[i].Particle3DCollision(particles[j]);
            }

            particles[i].checkSphereCollision(400);
            particles[i].drawParticle3D(10,10);
        }
    }
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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    //initialize Particle
    glm::vec3 position(0.0f, 200.0f, 0.0f);

    // spherical rotation and direction
    const float magnitude = 400.0f;
    const float anglePhi = glm::radians(-90.0f);
    const float angleTheta = glm::radians(1.0f);

    //for rotational and directional control
    float directionX = magnitude*(cos(anglePhi)*cos(angleTheta));
    float directionY = magnitude*(sin(anglePhi)*sin(angleTheta));
    float directionZ = magnitude*(sin(angleTheta));

    float velocityX = 0.0f;
    float velocityY = -200.0f;
    float velocityZ = 0.0f;

    glm::vec3 velocity = glm::vec3(directionX, directionY, directionZ);

    const float mass = 300.0f;
    const float radius = 10.0f;
    int numParticles = 100;

    for(int i = 0; i < numParticles; i++){
        particles.emplace_back(position, velocity, mass, radius);
        spawnTimes.push_back(i * TimeDelay); // each particle spawns after 0.1s
    }

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Perspective Projection (Perspective Matrix)
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), WIDTH / HEIGHT, 0.1f, 2000.0f);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixf(glm::value_ptr(projection));

        // View pipeline (Camera, ViewModel Matrix)
        glm::vec3 cameraPos(800.0f , 300.0f, 600.0f);
        glm::vec3 cameraTarget(0.0f,0.0f,0.0f);
        glm::vec3 cameraUp(0.0f,1.0f,0.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(glm::value_ptr(view));

        GLfloat lightPos[] = {400.0f, 300.0f, 200.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

        // Update
        elapsedTime += deltaTime;
        drawParticleArray3D(particles, deltaTime);
        drawBoundarySphere(20, 20, 400);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

