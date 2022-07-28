#include <Simple-3D-Renderer/Math/util.h>

void util::swap(glm::vec2& x, glm::vec2& y)
{
   glm::vec2 tmp = x;
   x = y;
   y = tmp;
}

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
