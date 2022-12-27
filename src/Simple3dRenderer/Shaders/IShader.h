#pragma once

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tga/TGAImage.h>

#include <Simple3dRenderer/Model/Model.h>

struct IShader
{
   glm::mat4 uniformModelM4;
   glm::mat4 uniformViewM4;
   glm::mat3 uniformViewM3;
   glm::mat4 uniformProjectionM4;
   glm::mat4 uniformSa;

   virtual ~IShader() {}
   virtual glm::fvec4 vertex(
         const size_t objIndx,
         const size_t vertexIndx,
         const size_t vertexFaceIndx,
         const Model& model
   ) = 0;
   virtual bool fragment(
         const glm::fvec3& bc,
         const Model& model,
         TGAColor& color
   ) = 0;
};
