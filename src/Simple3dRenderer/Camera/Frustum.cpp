#include <Simple3dRenderer/Camera/Frustum.h>

#include <glm/glm.hpp> 

#include <Simple3dRenderer/Settings/config.h>

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
