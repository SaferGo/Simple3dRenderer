#include <Simple-3D-Renderer/TextureProcessing/textureProcessing.h>

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

// Interpolates the coordinates with the barycentric coordinates.
glm::fvec2 textureProcessing::getTextureCoords(
      const glm::fvec2 (&t)[3],
      const glm::fvec3& bc
) {
   return glm::fvec2(
      t[0].x * bc.x + t[1].x * bc.y + t[2].x * bc.z,
      t[0].y * bc.x + t[1].y * bc.y + t[2].y * bc.z
   );
}

TGAColor textureProcessing::sampleTexture(
         const glm::fvec2 (&t)[3],
         const glm::fvec3& bc,
         const TGAImage& texture
) {
   glm::fvec2 uv = getTextureCoords(t, bc);

   return texture.get(uv.x * texture.width(), uv.y * texture.height());
}
