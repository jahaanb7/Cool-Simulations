#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp>

#include "library/Particle.h"
#include "library/Particle3D.h"
#include "library/Camera.h"

#include <vector>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

/*
Particle Simulation:

Each particle has intrinsic properties:
    - mass, radius
    - position, velocity
    - gravitational acceleration

Current Functionality:
    - Particle Collision
    - Gravity
    - Collision Boundary
*/

// Screen Dimension variables
const float WIDTH = 800.0f;
const float HEIGHT = 600.0f;

// Frame Timing, and delay in particle drawing
float elapsedTime = 0.0f;
double lastFrame = 0.0f; 
float TimeDelay = 0.5f;

// Array of Particles and spawnTime (float)
std::vector<Particle3D> particles;
std::vector<float> spawnTimes;

// Mouse Inputs
double mouseX;
double mouseY;

double yaw = 0.0;
double pitch = 0.0;

double lastMouseX = WIDTH / 2.0;
double lastMouseY = HEIGHT / 2.0;

bool mousePressed = true;

float sensitivity = 0.001f;
const float camRadius = 900.0f;

//Initialize camera
Camera cam;

void WindowResize(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

// 2D Boundary Box - collision and containment
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

// 3D Boundary Sphere - Visualization
void drawBoundarySphere(int lats, int longs, int radius){

    // Disabled Lighting so that it doesnt effect the boundary visualization
    glDisable(GL_LIGHTING);

    glColor3f(1.0f,1.0f,1.0f); 

    // Uses Legacy Opengl for creating singular points (GL_POINTS)
    glBegin(GL_POINTS);
    for(int i = 0; i <= lats; i++){

        float theta = i * M_PI/lats;
        for(int j = 0; j <= longs; j++){

            float phi = j * 2 * M_PI/longs;

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

    // auto& just gets the type of array it is automatically and is referenced
    for(auto& p : particles){        
        p.updatePosition(deltaTime);
        p.boundingBoundary(WIDTH, HEIGHT);
        p.drawParticle();
    }
}

void drawParticleArray3D(std::vector<Particle3D>& particles, float deltaTime){
    for(int i = 0; i < particles.size(); i++){
        // elapsedTime is for the time delay in drawing each particle
        if(elapsedTime >= spawnTimes[i]){
            particles[i].updatePosition3D(deltaTime);

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

void CameraSystem(GLFWwindow* window){
    glfwGetCursorPos(window, &mouseX, &mouseY); 

    if (mousePressed){
        lastMouseX = mouseX; 
        lastMouseY = mouseY; 

        mousePressed = false; 
    } 

    double deltaX = mouseX - lastMouseX; 
    double deltaY = mouseY - lastMouseY; 

    lastMouseX = mouseX; lastMouseY = mouseY; 

    deltaX *= sensitivity; 
    deltaY *= sensitivity; 

    yaw += deltaX; 
    pitch -= deltaY; 

    // Clamp pitch to prevent flipping 
    if (pitch > 89.0) pitch = 89.0; 
    if (pitch < -89.0) pitch = -89.0; 

    cam.rotate(pitch, yaw);

    float camX = camRadius * cos(pitch) * cos(yaw); 
    float camY = camRadius * cos(pitch) * sin(yaw); 
    float camZ = cos(pitch) * camRadius; 

    glm::vec3 camPosition = glm::vec3(camX, camY, 0.0f); 
    glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 camDirection = glm::normalize(camPosition - camTarget); 

    // up vector 
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, camDirection)); 
    glm::vec3 cameraUp = glm::cross(camDirection, cameraRight); 
    
    glm::mat4 view = glm::lookAt(camPosition, camTarget, cameraUp);
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

    // set a positional value to velocity
    float velocityX = 0.0f;
    float velocityY = -200.0f;
    float velocityZ = 0.0f;

    glm::vec3 velocity = glm::vec3(directionX, directionY, directionZ);

    // variables for particle initialization
    const float mass = 300.0f;
    const float radius = 10.0f;
    int numParticles = 100;

    // append numParticles to the array of particles and to spawnTimes array
    for(int i = 0; i < numParticles; i++){
        particles.emplace_back(position, velocity, mass, radius);
        spawnTimes.push_back(i * TimeDelay); // each particle spawns after 0.1s
    }

    // Testing particle
    Particle3D TestParticle = Particle3D(position, velocity, mass, 30.0f);

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

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(glm::value_ptr(view));

        // set light as positional and set (x,y,z) coordinates
        GLfloat light[] = {camPosition.x, camPosition.y, camPosition.z, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, light);

        // Update
        elapsedTime += deltaTime;
        drawParticleArray3D(particles, deltaTime);

        /* 
        TestParticle.drawParticle3D(10,10);
        TestParticle.updatePosition3D(deltaTime);
        TestParticle.checkSphereCollision(400);
        
        for(int i = 0; i < particles.size(); i++){
            TestParticle.Particle3DCollision(particles[i]);
        }
        */

        drawBoundarySphere(20, 20, 400);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

