#include <Simple3dRenderer/Camera/Camera.h>

#include <glm/glm.hpp> 

#include <Simple3dRenderer/Camera/Frustum.h>
#include <Simple3dRenderer/Settings/config.h>

Camera::Camera(
      const glm::fvec3 eyeP,
      const glm::fvec3 targetP,
      const glm::fvec3 upV,
      const float otherZnear,
      const float otherZfar,
      const float otherFovY
) {
   position = eyeP;

   // Z-axis
   axis[2] = glm::normalize(position - targetP);
   // X-axis
   axis[0] = glm::cross(glm::normalize(upV), axis[2]);
   // Y-axis
   axis[1] = glm::cross(axis[2], axis[0]);

   createFrustum(otherZnear, otherZfar, otherFovY);
}

void Camera::createFrustum(
      const float otherZnear,
      const float otherZfar,
      const float otherFovY
){
   frustum = Frustum(otherZnear, otherZfar, otherFovY);
}
