#pragma once

#include <string>

namespace config
{
   inline constexpr int RESOLUTION_WIDTH  = 800;
   inline constexpr int RESOLUTION_HEIGHT = 800;
   inline constexpr int HALF_RESOLUTION_WIDTH  = RESOLUTION_WIDTH  / 2;
   inline constexpr int HALF_RESOLUTION_HEIGHT = RESOLUTION_HEIGHT / 2;
   
   inline const std::string MODEL_TO_READ = "../obj/alien.obj";
};
