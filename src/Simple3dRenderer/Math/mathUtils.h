#pragma once

#include <glm/glm.hpp>

namespace mathUtils
{
   //Implement swap with template
   void swap(glm::fvec2& a, glm::fvec2& b);
   void swap(glm::ivec3& a, glm::ivec3& b);
   void orderInAscendingOrdX(glm::vec2 (&v)[3]);
   void orderInAscendingOrdY(glm::vec2 (&v)[3]);
   void getBoundingBoxes(
         const glm::fvec4 (&v)[3],
         glm::fvec2& boundingBoxMin,
         glm::fvec2& boundingBoxMax
   );
   const glm::fvec3 getBarycentricCoords(
         glm::fvec4 (&v)[3],
         const float area,
         const glm::fvec2& p
   );
   const bool isPointInTriangle(
      const glm::fvec4 (&v)[3],
      const glm::fvec2& p,
      float (&area)[3]
   );
   float edgeFunction(
      const glm::vec4& v1,
      const glm::vec4& v2,
      const glm::vec4& v3
   );

   float edgeFunction(
         const glm::vec4& v1,
         const glm::vec4& v2,
         const glm::vec2& p
   );
   const float getDepth(const glm::fvec4 (&v)[3], const glm::fvec3& bc);
};
