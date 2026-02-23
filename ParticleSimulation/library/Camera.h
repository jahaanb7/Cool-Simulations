#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Quaternion.h"

class Camera{
  public:

    // position and speed
    glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    float camSpeed = 10.0f;
    Quaternion orientation = Quaternion(1.0f,0.0f,0.0f,0.0f);

    // orientation based on the basis vectors
    glm::vec3 iHat;
    glm::vec3 jHat;
    glm::vec3 kHat;

    Camera(double x, double y, double z, double cameraSpeed){
      camPosition = glm::vec3(x,y,z);
      camSpeed = cameraSpeed;
      orientation = Quaternion(1.0f,0.0f,0.0f,0.0f);

      updateCam();
    }

    void updateCam(){

      // basis vectors that help to determine direction even after rotation
      glm::vec3 defaultForward = glm::vec3(0.0f, 0.0f, -1.0f);
      glm::vec3 defaultRight = glm::vec3(1.0f, 0.0f, 0.0f);
      glm::vec3 defaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
      
      // Rotate the default vectors by camera orientation
      kHat= orientation.rotate(defaultForward);
      iHat= orientation.rotate(defaultRight);
      jHat= orientation.rotate(defaultUp);

    }

    void setCam(double x, double y, double z, double cameraSpeed){

      //Initialize camera based on position and speed
      camPosition = glm::vec3(x, y, z);
      camSpeed = cameraSpeed;
      orientation = Quaternion(1.0f,0.0f,0.0f,0.0f);

      updateCam();
    }

    void rotate(double pitch, double yaw){

      //create the rotations as quaternions
      Quaternion pitchQ = Quaternion::quaternionRotation(iHat, pitch);
      Quaternion yawQ   = Quaternion::quaternionRotation(jHat, yaw);
      
      //apply yaw then pitch
      Quaternion rotation = yawQ*pitchQ;
      
      //apply that to the camera
      orientation = rotation*orientation;
      orientation.normalize();
      
      updateCam();
    }


    void moveZ(float amount){
      camPosition += kHat * amount;
    }

    void moveX(float amount) {
      camPosition += iHat * amount;
    }

    void moveY(float amount) {
      camPosition += jHat * amount;
    }

    glm::vec3 get_kHat(){return kHat;}
    glm::vec3 get_iHat(){return iHat;}
    glm::vec3 get_jHat(){return jHat;}

    glm::vec3 getPosition() {
      return glm::vec3(camPosition.x, camPosition.y, camPosition.z);
    }

    void set_position(double x, double y, double z) {
      camPosition.x = x;
      camPosition.y = y;
      camPosition.z = z;
    }    
}; 