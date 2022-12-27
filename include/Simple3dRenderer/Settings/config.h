#pragma once

#include <string>

#include <glm/glm.hpp> 

namespace config
{
   inline constexpr int RESOLUTION_WIDTH  = 800;
   inline constexpr int RESOLUTION_HEIGHT = 800;
   inline constexpr int HALF_RESOLUTION_WIDTH  = RESOLUTION_WIDTH  / 2;
   inline constexpr int HALF_RESOLUTION_HEIGHT = RESOLUTION_HEIGHT / 2;
   inline constexpr float ASPECT = RESOLUTION_WIDTH / RESOLUTION_HEIGHT;

   // Camera config.
   //inline constexpr glm::fvec3 eyeP = glm::fvec3(1.0, 0.0, 3.5);
   inline constexpr glm::fvec3 eyeP = glm::fvec3(0.8, 0.5, 3.5);
   inline constexpr glm::fvec3 targetP = glm::fvec3(0.0, 0.0, -1.0);
   // Here if we change upV to -upV, the camera will rotate 180 degrees.
   inline constexpr glm::fvec3 upV = glm::fvec3(0.0, 1.0, 0.0);

   // Frustum config.
   inline constexpr float zNear = 0.01;
   inline constexpr float zFar = 100.0;
   inline constexpr float fovY = 0.78;
   
   inline const std::string MODEL_TO_READ = "../assets/obj/head.obj";
   inline const std::string DIFFUSE_TEXTURE_TO_READ = (
         "../assets/texture/diffuse/head.tga"
   );
   inline const std::string NORMAL_TEXTURE_TO_READ = (
         "../assets/texture/normal/head.tga"
   );
   inline const std::string SPECULAR_TEXTURE_TO_READ = (
         "../assets/texture/specular/head.tga"
   );
};
