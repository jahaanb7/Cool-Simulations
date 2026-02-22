#include <glm/glm.hpp>

class Quaternion{
  public:
    float iHat, jHat, kHat;
    float realNum;

    glm::vec3 vectorQ = glm::vec3(iHat, jHat, kHat);

  Quaternion(float w,float x, float y, float z){
    iHat = x;
    jHat = y;
    kHat = z;
    realNum = w;
  }

  Quaternion(glm::vec3 position, float w){
    iHat = position.x;
    jHat = position.y;
    kHat = position.z;
    realNum = w;
  }

  Quaternion inverse(){
    double magnitude = (realNum * realNum) + (iHat*iHat) + (jHat*jHat) + (kHat*kHat);

    return Quaternion(realNum, -iHat/magnitude, -jHat/magnitude, -kHat/magnitude);
  }

  Quaternion multiply_q(Quaternion q){

    double w1 = realNum;  double w2 = q.realNum;
    double x1 = iHat;     double x2 = q.iHat;
    double y1 = jHat;     double y2 = q.jHat;
    double z1 = kHat;     double z2 = q.kHat;

    return Quaternion(
      (w1*w2 - x1*x2 - y1*y2 - z1*z2),
      (w1*x2 + x1*w2 + y1*z2 - z1*y2),
      (w1*y2 - x1*z2 + y1*w2 + z1*x2),
      (w1*z2 + x1*y2 - y1*x2 + z1*w2)
    );
  }

   glm::vec3 rotateAxis(glm::vec3 vec){
    Quaternion q = Quaternion(vec, 0);
    Quaternion rotate = multiply_q(q).multiply_q(inverse());
    return glm::vec3(rotate.iHat, rotate.jHat, rotate.kHat);
  }
};