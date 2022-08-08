#include <Simple-3D-Renderer/Camera/Camera.h>

#include <Simple-3D-Renderer/Camera/Frustum.h>
#include <Simple-3D-Renderer/Settings/config.h>

#include <glm/glm.hpp> 

Camera::Camera(
      const glm::fvec3 eyeP,
      const glm::fvec3 targetP,
      const glm::fvec3 upV
) {
   position = eyeP;

   // Z-axis
   axis[2] = glm::normalize(position - targetP);
   // X-axis
   axis[0] = glm::cross(glm::normalize(upV), axis[2]);
   // Y-axis
   axis[1] = glm::cross(axis[2], axis[0]);
}

void Camera::createFrustum(
      const float otherZnear,
      const float otherZfar,
      const float otherFovY
){
   frustum = Frustum(otherZnear, otherZfar, otherFovY);
}
