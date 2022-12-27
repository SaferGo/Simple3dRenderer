#include <Simple3dRenderer/Shaders/NormalMapping.h>
#include <iostream>

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

#include <Simple3dRenderer/Vertex/vertexProcessing.h>
#include <Simple3dRenderer/Texture/textureProcessing.h>
#include <Simple3dRenderer/Model/Model.h>

NormalMapping::~NormalMapping() {}

glm::fvec4 NormalMapping::vertex(
      const size_t objIndx,
      const size_t vertexIndx,
      const size_t vertexFaceIndx,
      const Model& model
) {

   m_vertices[vertexFaceIndx] = model.getVertex(objIndx, vertexIndx);
   m_texCoords[vertexFaceIndx] = model.getUV(objIndx, vertexIndx);
   m_normals[vertexFaceIndx] = model.getAvgNormal(objIndx, vertexIndx);
   m_tangents[vertexFaceIndx] = model.getAvgTangent(objIndx, vertexIndx);

   // In camara space
   m_lightDir = glm::normalize(
         uniformViewM4 * glm::fvec4(0.0, 0.0, 1.0, 0.0)
   );

   // Transfroms world to clip coordinates.
   glm::fvec4 v = (
      uniformProjectionM4 * uniformViewM4 * uniformModelM4 *
      m_vertices[vertexFaceIndx]
   );

   vertexProcessing::makePerspectiveDivision(v);

   if (vertexProcessing::isInClipSpace(v) == false)
      return glm::fvec4(-1);

   return v;
}

bool NormalMapping::fragment(
      const glm::fvec3& bc,
      const Model& model,
      TGAColor& color
) {
   glm::fvec4 tangent = vertexProcessing::getInterpolatedTangent(
         m_tangents,
         bc
   );
   glm::fvec3 normal = vertexProcessing::getInterpolatedVector(
         m_normals,
         bc
   );

   glm::mat3 TBNinversed = vertexProcessing::getTBNinversed(normal, tangent);

   glm::fvec4 normalMapped = textureProcessing::normalMapping(
         m_texCoords,
         bc,
         model.getNormalTexture()
   );

   TGAColor diffuseTexel = textureProcessing::sampleTexture(
         m_texCoords,
         bc,
         model.getDiffuseTexture()
   );

   TGAColor specularTexel = textureProcessing::sampleTexture(
         m_texCoords,
         bc,
         model.getSpecularTexture()
   );

   TBNinversed = glm::mat3(0.0f);
   TBNinversed[2] = glm::normalize(normal);
   glm::fvec3 edge1 = m_vertices[1] - m_vertices[0];
   glm::fvec3 edge2 = m_vertices[2] - m_vertices[0];
   float x1 = m_texCoords[1].x - m_texCoords[0].x;
   float x2 = m_texCoords[2].x - m_texCoords[0].x;
   float y1 = m_texCoords[1].y - m_texCoords[0].y;
   float y2 = m_texCoords[2].y - m_texCoords[0].y;
   //TBNinversed[0] = glm::normalize((
   //      (edge1 * y2 - edge2 * y1) * (1.0f / (x1 * y2 - x2 * y1))
   //));
   //TBNinversed[1] = glm::normalize(glm::cross(TBNinversed[2], TBNinversed[0]));
   //TBNinversed = glm::inverse(TBNinversed);

   // Forma que anda pero no anda con rotaciones(sin TBA)
   //normal = uniformViewM4 * uniformModelM4 * normal;
   // Con TBA (opcion 1, no optima)
   //normal = varyingTBA * normal;
   //normal = uniformViewM4 * uniformModelM4 * normal;

   //glm::fvec3 rs = uniformViewM3 * varyingTBA * glm::fvec3(
   //      m_lightDir.x,
   //      m_lightDir.y,
   //      m_lightDir.z
   //);
   float diffuseLight = glm::max(
         0.0f,
         glm::dot(
            uniformViewM3 * TBNinversed * glm::fvec3(normalMapped),
            //glm::fvec3(uniformSa * glm::fvec4(normal, 0.0)),
            //glm::transpose(glm::inverse(uniformViewM3)) * normal,
            glm::fvec3(m_lightDir.x, m_lightDir.y, m_lightDir.z)
         )
   );

   //glm::fvec4 r = glm::normalize(
   //      normalMapped * 
   //      glm::dot(
   //         normalMapped,
   //         m_lightDir
   //      ) * 2.0f -
   //      m_lightDir
   //);

   //float specularLight = glm::pow(
   //      glm::max(
   //         r.z,
   //         0.0f
   //      ), specularTexel.bgra[0]

   //);

   TGAColor light = TGAColor(
         diffuseLight * 250,
         diffuseLight * 250,
         diffuseLight * 250
   );

   diffuseTexel[0] *= diffuseLight;
   diffuseTexel[1] *= diffuseLight;
   diffuseTexel[2] *= diffuseLight;

   //color = diffuseTexel;

   //for (int i = 0; i < 3; i++)
   //   color[i] = glm::min(
   //         5.0f + diffuseTexel[i] * (diffuseLight + 0.6f * specularLight),
   //         255.0f
   //   );

   color = light;

   return true;
}
