#pragma once

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

namespace textureProcessing
{
   glm::fvec2 getTextureCoords(const glm::fvec2 (&t)[3], const glm::fvec3& bc);
   TGAColor sampleTexture(
         const glm::fvec2 (&t)[3],
         const glm::fvec3& bc,
         const TGAImage& texture
   );
};
