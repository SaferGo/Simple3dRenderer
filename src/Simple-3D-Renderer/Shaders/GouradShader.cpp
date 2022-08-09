#include <Simple-3D-Renderer/Shaders/GouradShader.h>

#include <Simple-3D-Renderer/VertexProcessing/vertexProcessing.h>
#include <Simple-3D-Renderer/TextureProcessing/textureProcessing.h>
#include <Simple-3D-Renderer/MeshLoader.h>

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

GouradShader::~GouradShader() {}

glm::fvec4 GouradShader::vertex(
      const size_t objIndx,
      const size_t faceIndx,
      const size_t vertexIndx,
      MeshLoader& mesh
) {
   glm::fvec4 v = mesh.getVertex(objIndx, faceIndx, vertexIndx);
   varyingNormal[vertexIndx] = mesh.getNormal(
         objIndx, faceIndx, vertexIndx
   );
   varyingUV[vertexIndx] = mesh.getUVcoords(
         objIndx, faceIndx, vertexIndx
   );
   varyingLightDir = glm::normalize(glm::fvec4(0.5, 0.5, 1.0, 0.0));

   // Transfroms world to clip coordinates.
   v = uniformProjectionM * uniformViewM * uniformModelM * v;

   // After performing the perspective division, we won't need the
   // w-value anymore.
   vertexProcessing::makePerspectiveDivision(v);

   if (vertexProcessing::isInClipSpace(v) == false)
      return glm::fvec4(-1);

   // Transforms clip to screen coordinates.
   return uniformViewportM * v;
}

bool GouradShader::fragment(
      const glm::fvec3& bc,
      const TGAImage& texture,
      TGAColor& color
) {
   float lightIntensity = (
         glm::dot(
            uniformSa * glm::normalize(varyingNormal[0]),
            varyingLightDir
         ) * bc.x +
         glm::dot(
            uniformSa * glm::normalize(varyingNormal[1]),
            varyingLightDir
         ) * bc.y +
         glm::dot(
            uniformSa * glm::normalize(varyingNormal[2]),
            varyingLightDir
         ) * bc.z
   );

   TGAColor texel = textureProcessing::sampleTexture(varyingUV, bc, texture);
   TGAColor light = TGAColor(
                  lightIntensity * 250,
                  lightIntensity * 250,
                  lightIntensity * 250
   );

   if (lightIntensity < 0)
      return false;

   texel[0] *= lightIntensity;
   texel[1] *= lightIntensity;
   texel[2] *= lightIntensity;

   color = texel;

   return true;
}
