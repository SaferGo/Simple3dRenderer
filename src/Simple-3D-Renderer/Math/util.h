#pragma once

#include <glm/glm.hpp>

namespace util
{
   //Implement swap with template
   void swap(glm::fvec2& a, glm::fvec2& b);
   void swap(glm::ivec3& a, glm::ivec3& b);
   void orderInAscendingOrdX(glm::vec2 (&v)[3]);
   void orderInAscendingOrdY(glm::vec2 (&v)[3]);
};
