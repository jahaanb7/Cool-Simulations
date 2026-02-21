#include <glm/glm.hpp>
#include <cmath>

class Particle3D{
  public:
    glm::vec3 position, velocity;
    glm::vec3 acceleration;

    // additional vars
    float mass, radius;
    float damping = 0.96f;

    Particle3D(glm::vec3 pos, glm::vec3 vel, float m, float r){
      position = pos; 
      velocity = vel;
      mass = m;
      radius = r;

      acceleration = glm::vec3(0.0f, -98.0f, 0.0f);
    }

  void drawParticle3D(int lats, int longs){
    for(int i = 0; i < lats; i++){
      float lat0 = M_PI * (-0.5f + (float)i / lats);
      float z0  = sin(lat0) * radius;
      float zr0 = cos(lat0) * radius;

      float lat1 = M_PI * (-0.5f + (float)(i+1) / lats);
      float z1 = sin(lat1) * radius;
      float zr1 = cos(lat1) * radius;

      glBegin(GL_TRIANGLE_STRIP);
      for(int j = 0; j <= longs; j++){

        float lng = 2 * M_PI * (float)(j) / longs;
        float x = cos(lng);
        float y = sin(lng);

        glm::vec3 v1 = position + glm::vec3(x*zr0, y*zr0, z0);
        glm::vec3 n1 = glm::normalize(glm::vec3(x*zr0, y*zr0, z0));
          
        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(v1.x, v1.y, v1.z);
          
        glm::vec3 v2 = position + glm::vec3(x*zr1, y*zr1, z1);
        glm::vec3 n2 = glm::normalize(glm::vec3(x*zr1, y*zr1, z1));
          
        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(v2.x, v2.y, v2.z);
      }
      glEnd();
    }
  }

  void updatePosition3D(float deltaTime){
      velocity += acceleration * deltaTime;
      position += velocity * deltaTime;
  }

  void boundingBoundary3D(int WIDTH, int HEIGHT){
    float boundsX = WIDTH/2.0f;
    float boundsY = HEIGHT/2.0f;
    float boundsZ = 400.0f;

    if(position.x + radius > boundsX){
        position.x = boundsX - radius;
        velocity.x *= -1.0f;
    }

    if(position.x - radius < -boundsX){
        position.x = -boundsX + radius;
        velocity.x *= -1.0f;
    }

    if(position.y + radius > boundsY){
        position.y = boundsY - radius;
        velocity.y *= -1.0f;
    }

    if(position.y - radius < -boundsY){
        position.y = -boundsY + radius;
        velocity.y *= -1.0f;
    }

    if(position.z + radius > boundsZ){
        position.z = boundsZ - radius;
        velocity.z *= -1.0f;
    }

    if(position.z - radius < -boundsZ){
        position.z = -boundsZ + radius;
        velocity.z *= -1.0f;
    }
  }

  void checkSphereCollision(int boundaryRadius)
  {
    float distance = glm::length(position);

    if(distance + radius >= boundaryRadius)
    {
        glm::vec3 normal = glm::normalize(position);

        velocity = velocity - 2.0f * glm::dot(velocity, normal) * normal;
        velocity *= damping;
        position = normal * (boundaryRadius - radius);
    }
  }

  void Particle3DCollision(Particle3D& compareParticle){
    glm::vec3 delta = compareParticle.position - position;
    float distance = glm::length(delta);
    float minDistance = radius + compareParticle.radius;

    if(distance < minDistance){
      glm::vec3 temp = velocity;
      velocity = compareParticle.velocity * damping;
      compareParticle.velocity = temp * damping;

      glm::vec3 normal = glm::normalize(delta);
      float overlap = minDistance - distance;

      position -= normal * (overlap * 0.5f);
      compareParticle.position += normal * (overlap * 0.5f);
    }
  }
};