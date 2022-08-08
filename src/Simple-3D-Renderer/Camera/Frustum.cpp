#include <Simple-3D-Renderer/Camera/Frustum.h>

#include <Simple-3D-Renderer/Settings/config.h>

#include <glm/glm.hpp> 

Frustum::Frustum() {}

Frustum::Frustum(
      const float otherZnear,
      const float otherZfar,
      const float otherFovY
) {
   zNear = otherZnear;
   zFar  = otherZfar;
   fovY  = otherFovY;
  
   const float tangent = glm::tan(fovY * 0.5);
   const float height = zNear * tangent;
   const float width = height * config::ASPECT;

   left   = -width;
   right  = width;
   bottom = -height;
   top    = height;
}
