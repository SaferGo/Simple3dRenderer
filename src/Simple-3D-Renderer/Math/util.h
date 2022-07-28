#pragma once

#include <glm/glm.hpp>

namespace util
{
   void swap(glm::vec2& x, glm::vec2& y);
   void orderInAscendingOrdX(glm::vec2 (&v)[3]);
   void orderInAscendingOrdY(glm::vec2 (&v)[3]);
};
