#pragma once

#include <glm/glm.hpp> 

#include <Simple3dRenderer/Camera/Frustum.h>

struct Camera
{
   glm::fvec3 position;
   glm::fvec3 axis[3];
   Frustum frustum;

   explicit Camera(
         const glm::fvec3 eyeP,
         const glm::fvec3 targetP,
         const glm::fvec3 upV,
         const float otherZnear,
         const float otherZfar,
         const float otherFovY
   );

   void createFrustum(
         const float otherZnear,
         const float otherZfar,
         const float otherFovY
   );
};
