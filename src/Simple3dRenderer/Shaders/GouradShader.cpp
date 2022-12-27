#include <Simple3dRenderer/Shaders/GouradShader.h>

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

#include <Simple3dRenderer/Vertex/vertexProcessing.h>
#include <Simple3dRenderer/Texture/textureProcessing.h>
#include <Simple3dRenderer/Model/Model.h>

GouradShader::~GouradShader() {}

glm::fvec4 GouradShader::vertex(
      const size_t objIndx,
      const size_t vertexIndx,
      const size_t vertexFaceIndx,
      const Model& model
) {

   glm::fvec4 v = model.getVertex(objIndx, vertexIndx);

   if (model.hasNormals())
      m_normals[vertexFaceIndx] = model.getNormal(objIndx, vertexIndx);

   if (model.hasTexture())
      m_texCoords[vertexFaceIndx] = model.getUV(objIndx, vertexIndx);

   m_lightDir = glm::normalize(glm::fvec4(0.5, 0.5, 1.0, 0.0));

   // Transfroms object to world coordinates.
   v = uniformProjectionM4 * uniformViewM4 * uniformModelM4 * v;

   vertexProcessing::makePerspectiveDivision(v);

   if (vertexProcessing::isInClipSpace(v) == false)
      return glm::fvec4(-1);

   return v;
}

bool GouradShader::fragment(
      const glm::fvec3& uv,
      const Model& model,
      TGAColor& color
) {
   float lightIntensity;

   if (model.hasNormals())
   {
      lightIntensity = (
            glm::dot(
               uniformSa * glm::normalize(glm::fvec4(m_normals[0], 0.0f)),
               m_lightDir
            ) * uv.x +
            glm::dot(
               uniformSa * glm::normalize(glm::fvec4(m_normals[1], 0.0f)),
               m_lightDir
            ) * uv.y +
            glm::dot(
               uniformSa * glm::normalize(glm::fvec4(m_normals[2], 0.0f)),
               m_lightDir
            ) * uv.z
      );
   }

   TGAColor texel;

   if (model.hasTexture())
   {
      texel = textureProcessing::sampleTexture(
            m_texCoords,
            uv,
            model.getDiffuseTexture()
      );

   } else
      texel = TGAColor(250, 250, 250);

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
