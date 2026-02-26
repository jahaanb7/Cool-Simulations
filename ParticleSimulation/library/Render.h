#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <cmath>

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

    // Latitude lines
    for (int i = 0; i <= lats; i++) {
        float thetaLong = i * M_PI / lats;

        glBegin(GL_LINE_STRIP);
        for (int j = 0; j <= longs; j++) {

            //calculate phi for both hemispheres for longitude
            float phi = j * 2 * M_PI / longs;

            float x = radius * sin(thetaLong) * cos(phi);
            float y = radius * sin(thetaLong) * sin(phi);
            float z = radius * cos(thetaLong);

            glVertex3f(x, y, z);
        }
        glEnd();
    }

    // Longitude lines
    for (int j = 0; j <= longs; j++) {
        float phi = j * 2 * M_PI / longs;

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= lats; i++) {

            //calculate phi for both hemispheres for latitude
            float thetaLat = i * M_PI / lats;

            float x = radius * sin(thetaLat) * cos(phi);
            float y = radius * sin(thetaLat) * sin(phi);
            float z = radius * cos(thetaLat);

            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
}