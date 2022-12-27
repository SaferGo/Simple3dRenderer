#pragma once

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

#include <Simple3dRenderer/Shaders/IShader.h>
#include <Simple3dRenderer/Model/Model.h>

struct GouradShader : public IShader
{
   // Variables that will be interpolated
   // (out variables in glsl)
   glm::fvec3 m_normals[3];
   glm::fvec2 m_texCoords[3];
   glm::fvec4 m_lightDir;

   ~GouradShader() override;
   glm::fvec4 vertex(
         const size_t objIndx,
         const size_t vertexIndx,
         const size_t vertexFaceIndx,
         const Model& model
   ) override;
   bool fragment(
         const glm::fvec3& bc,
         const Model& model,
         TGAColor& color
   ) override;
};
