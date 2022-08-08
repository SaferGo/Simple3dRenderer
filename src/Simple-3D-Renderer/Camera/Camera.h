#pragma once

#include <Simple-3D-Renderer/Camera/Frustum.h>

#include <glm/glm.hpp> 

struct Camera
{
   glm::fvec3 position;
   glm::fvec3 axis[3];
   Frustum frustum;

   explicit Camera(
         const glm::fvec3 eyeP,
         const glm::fvec3 targetP,
         const glm::fvec3 upV
   );

   void createFrustum(
         const float otherZnear,
         const float otherZfar,
         const float otherFovY
   );
};
