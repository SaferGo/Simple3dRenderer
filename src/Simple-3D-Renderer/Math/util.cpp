#include <Simple-3D-Renderer/Math/util.h>

void util::orderInAscendingOrdY(glm::vec2 (&v)[3])
{
   if (v[0].y > v[1].y)
      swap(v[0], v[1]);
   if (v[0].y > v[2].y)
      swap(v[0], v[2]);
   if (v[1].y > v[2].y)
      swap(v[1], v[2]);
}

void util::orderInAscendingOrdX(glm::vec2 (&v)[3])
{
   if (v[0].x > v[1].x)
      swap(v[0], v[1]);
   if (v[0].x > v[2].x)
      swap(v[0], v[2]);
   if (v[1].x > v[2].x)
      swap(v[1], v[2]);
}

void util::swap(glm::fvec2& a, glm::fvec2& b)
{
   glm::fvec2 tmp = a;
   a = b;
   b = tmp;
}

void util::swap(glm::ivec3& a, glm::ivec3& b)
{
   glm::ivec3 tmp = a;
   a = b;
   b = tmp;
}

