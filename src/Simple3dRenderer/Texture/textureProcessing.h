#pragma once

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

namespace textureProcessing
{
   glm::fvec2 getTextureCoords(const glm::fvec2 (&t)[3], const glm::fvec3& bc);
   glm::fvec4 normalMapping(
         const glm::fvec2 (&t)[3],
         const glm::fvec3& bc,
         const TGAImage& normalTexture
   );
   TGAColor sampleTexture(
         const glm::fvec2 (&t)[3],
         const glm::fvec3& bc,
         const TGAImage& texture
   );
};
