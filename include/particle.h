#include <glm/glm.hpp> 

class Particle{
  public:
    glm::vec2 position; // 2D position vector (x,y)
    glm::vec2 velocity; // magnitude and direction
    float mass;
    float radius;

    Particle(glm::vec2 pos, glm::vec2 vel, float m, float r)
      : position(pos), velocity(vel), mass(m), radius(r){}

    void update(float deltaTime) {
      position += velocity * deltaTime;
    }
};