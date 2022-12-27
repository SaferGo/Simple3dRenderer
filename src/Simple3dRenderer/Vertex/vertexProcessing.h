#pragma once

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>

#include <Simple3dRenderer/Camera/Camera.h>

namespace vertexProcessing
{
   glm::mat4 getModelM4();
   glm::mat3 getViewM3(const Camera& camera);
   glm::mat4 getViewM4(const Camera& camera);
   glm::mat4 getProjectionM4(const Frustum& frustum);
   glm::mat4 getViewportM4();
   glm::mat3 getTBNinversed(
      const glm::fvec3& normal,
      const glm::fvec4& tangent
   );

   glm::fvec3 getInterpolatedVector(
         const glm::fvec3 (&v)[3],
         const glm::fvec3& bc
   );
   
   glm::fvec4 getInterpolatedTangent(
         const glm::fvec4 (&v)[3],
         const glm::fvec3& bc
   );

   void makePerspectiveDivision(glm::fvec4& v);
   bool isInClipSpace(const glm::fvec4& clipCoords);
};
