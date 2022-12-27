#include <Simple3dRenderer/Math/mathUtils.h>

#include <algorithm>

#include <Simple3dRenderer/Settings/config.h>

void mathUtils::orderInAscendingOrdY(glm::vec2 (&v)[3])
{
   if (v[0].y > v[1].y)
      swap(v[0], v[1]);
   if (v[0].y > v[2].y)
      swap(v[0], v[2]);
   if (v[1].y > v[2].y)
      swap(v[1], v[2]);
}

void mathUtils::orderInAscendingOrdX(glm::vec2 (&v)[3])
{
   if (v[0].x > v[1].x)
      swap(v[0], v[1]);
   if (v[0].x > v[2].x)
      swap(v[0], v[2]);
   if (v[1].x > v[2].x)
      swap(v[1], v[2]);
}

void mathUtils::swap(glm::fvec2& a, glm::fvec2& b)
{
   glm::fvec2 tmp = a;
   a = b;
   b = tmp;
}

void mathUtils::swap(glm::ivec3& a, glm::ivec3& b)
{
   glm::ivec3 tmp = a;
   a = b;
   b = tmp;
}

void mathUtils::getBoundingBoxes(
      const glm::fvec4 (&v)[3],
      glm::fvec2& boundingBoxMin,
      glm::fvec2& boundingBoxMax
) {

   boundingBoxMin.x = std::min({
         v[0].x,
         v[1].x,
         v[2].x
   });
   boundingBoxMin.y = std::min({
         v[0].y,
         v[1].y,
         v[2].y
   });
   boundingBoxMax.x = std::max({
         v[0].x,
         v[1].x,
         v[2].x
   });
   boundingBoxMax.y = std::max({
         v[0].y,
         v[1].y,
         v[2].y
   });
   

   // Clip
   boundingBoxMax = glm::fvec2(
         std::min(
            (int)(glm::floor(boundingBoxMax.x)),
            config::RESOLUTION_WIDTH
         ),
         std::min(
            (int)(glm::floor(boundingBoxMax.y)),
            config::RESOLUTION_HEIGHT
         )
   );
   boundingBoxMin = glm::fvec2(
         std::max(
            (int)(glm::floor(boundingBoxMin.x)),
            0
         ),
         std::max(
            (int)(glm::floor(boundingBoxMin.y)),
            0
         )
   );
}

// It tells us:
//    1) Which side of the line the point is on(in counter-clockwise form):
//       e(p) < 0 -> inside the triagle
//       e(p) = 0 -> on the edge
//       e(p) > 0 -> outside of the triangle
//    2) The area of the parallelogram formed by these 3 vertices.
//
// .Also, this is the same as the 2D cross product of the vectors:
//
//    A = (p - v1)
//    B = (v2 - v1)
//
float mathUtils::edgeFunction(
      const glm::vec4& v1,
      const glm::vec4& v2,
      const glm::vec4& v3
) {
   return (v3.x - v1.x) * (v2.y - v1.y) - (v3.y - v1.y) * (v2.x - v1.x);
}

float mathUtils::edgeFunction(
      const glm::vec4& v1,
      const glm::vec4& v2,
      const glm::vec2& p
) {
   return (p.x - v1.x) * (v2.y - v1.y) - (p.y - v1.y) * (v2.x - v1.x);
}

const bool mathUtils::isPointInTriangle(
      const glm::fvec4 (&v)[3],
      const glm::fvec2& p,
      float (&area)[3]
) {
   area[0] = edgeFunction(v[1], v[2], p);
   area[1] = edgeFunction(v[2], v[0], p);
   area[2] = edgeFunction(v[0], v[1], p);

   for (int i = 0; i < 3; i++)
   {
      if (area[i] > 0)
         return false;
   }

   return true;
}

const glm::fvec3 mathUtils::getBarycentricCoords(
      glm::fvec4 (&v)[3],
      const float area,
      const glm::fvec2& p
) {
   glm::fvec3 bc(-1);

   float miniArea[3];
   
   if (isPointInTriangle(v, p, miniArea))
   {
      bc.x = miniArea[0] / area;
      bc.y = miniArea[1] / area;
      bc.z = 1 - bc.x - bc.y;
   }

   return bc;
}

const float mathUtils::getDepth(const glm::fvec4 (&v)[3], const glm::fvec3& bc)
{
   return v[0].z * bc.x + v[1].z * bc.y + v[2].z * bc.z;
}
