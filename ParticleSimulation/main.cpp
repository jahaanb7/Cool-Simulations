#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>

#include "library/Particle.h"
#include "library/Particle3D.h"
#include "library/Camera.h"
#include "library/Render.h"

#include <vector>
#include <cmath>

#include <random>
std::random_device rd;
std::mt19937 gen(rd());

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

/*
 * Real-time 3D particle dynamics simulation.
 *
 * Each particle maintains mass, radius, position,
 * and velocity. Motion is updated per-frame using
 * delta-time integration with gravitational acceleration.
 *
 * Includes:
 *  - Elastic particle-to-particle collision handling
 *  - Sphere-boundary collision response
 *  - Time-based spawning system
 */

// Screen Dimension variables
const float WIDTH = 800.0f;
const float HEIGHT = 600.0f;

// Frame Timing, and delay in particle drawing
float elapsedTime = 0.0f;
double lastFrame = 0.0f; 
float TimeDelay = 0.01f;

// Array of Particles and spawnTime (float)
std::vector<Particle3D> particles;
std::vector<float> spawnTimes;


//Initialize camera
Camera cam(400.0f, 300.0f, 900.0f, 10.0f);

void WindowResize(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void drawParticleArray2D(std::vector<Particle>& particles, float deltaTime){

    for(auto& p : particles){        
        p.updatePosition(deltaTime);
        p.boundingBoundary(WIDTH, HEIGHT);
        p.drawParticle();
    }
}


void SetGravity(Particle3D& particle){
    
    glm::vec3 bottom = glm::vec3(0.0f, -400.0f, 0.0f);
    glm::vec3 direction = bottom - particle.position;
    float distance = glm::length(direction);

    direction = glm::normalize(direction);

    if (distance < 5.0f){
        distance = 5.0f;
    }

    float gConstant = 7000000.0f; 
    float accelMagnitude = gConstant / (distance * distance);

    particle.acceleration += direction * accelMagnitude;
}

void VerletIntegration(Particle3D& particle, float deltaTime){


    particle.position += particle.velocity * deltaTime + 0.5f * particle.acceleration * deltaTime * deltaTime;

    glm::vec3 oldAcceleration = particle.acceleration;

    particle.acceleration = glm::vec3(0.0f);

    SetGravity(particle);

    particle.velocity += 0.5f * (oldAcceleration + particle.acceleration) * deltaTime;
}

void drawParticleArray3D(std::vector<Particle3D>& particles, float deltaTime){
    for(int i = 0; i < particles.size(); i++){

        // elapsedTime is for the time delay in drawing each particle
        if(elapsedTime >= spawnTimes[i]){
            
            VerletIntegration(particles[i], deltaTime);

            // Detect collision for iteration i against iteration i + 1 (j)
            for(int j = i+1; j < particles.size(); j++){
                particles[i].Particle3DCollision(particles[j]);
            }

            // Boundary Sphere collision
            particles[i].checkSphereCollision(400);

            particles[i].drawParticle3D(10,10);
        }
    }
}

int main(void)
{
    // initialize glfw
    if (!glfwInit()){
        return -1;
    }

    // create GLFWwindow
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Space Simulation", NULL, NULL);

    // terminate glfw if the window is not found
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // connect it to openGl (context)
    glfwMakeContextCurrent(window);

    // initializes all OpenGL function pointers (retrieves openGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // resize window
    glfwSetFramebufferSizeCallback(window, WindowResize);

    // Depth Test (DepthBuffer)
    glEnable(GL_DEPTH_TEST);

    // Enable positional lighting that
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //initialize Particle
    glm::vec3 position(0.0f, 200.0f, 0.0f);

    // spherical rotation and direction
    const float magnitude = 400.0f;
    const float anglePhi = glm::radians(-90.0f);
    const float angleTheta = glm::radians(1.0f);

    // for rotational and directional control
    float directionX = magnitude*(cos(anglePhi)*cos(angleTheta));
    float directionY = magnitude*(sin(anglePhi)*sin(angleTheta));
    float directionZ = magnitude*(sin(angleTheta));

    glm::vec3 velocity = glm::vec3(directionX, directionY, directionZ);

    // variables for particle initialization
    const float mass = 30.0f;
    const float radius = 10.0f;
    int numParticles = 100;

    // append numParticles to the array of particles and to spawnTimes array
    for(int i = 0; i < numParticles; i++){

            particles.emplace_back(position, velocity, mass, radius);

            // each particle spawns after 0.1s
            spawnTimes.push_back(i * TimeDelay); 
    }

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Perspective Projection (Perspective Matrix)
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), WIDTH / HEIGHT, 0.1f, 2000.0f);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixf(glm::value_ptr(projection));

        // View pipeline (Camera, ViewModel Matrix)

        cam.CameraSystem(window);

        glm::vec3 camPosition = cam.getPosition();
        glm::vec3 forward = cam.get_kHat();
        glm::vec3 up = cam.get_jHat();

        cam.MoveCamera(window, deltaTime);

        glm::mat4 view = glm::lookAt(camPosition, camPosition + forward,up);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(glm::value_ptr(view));

        // set light as positional and set (x,y,z) coordinates
        GLfloat light[] = {camPosition.x, camPosition.y, camPosition.z, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, light);

        // Update
        elapsedTime += deltaTime;
        drawParticleArray3D(particles, deltaTime);
        drawBoundarySphere(20, 20, 400);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}