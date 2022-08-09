#pragma once

#include <Simple-3D-Renderer/MeshLoader.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tga/TGAImage.h>

struct IShader
{
   glm::mat4 uniformModelM;
   glm::mat4 uniformViewM;
   glm::mat4 uniformProjectionM;
   glm::mat4 uniformViewportM;
   glm::mat4 uniformSa;

   virtual ~IShader() {}
   virtual glm::fvec4 vertex(
         const size_t objIndx,
         const size_t faceIndx,
         const size_t vertexIndx,
         MeshLoader& mesh
   ) = 0;
   virtual bool fragment(
         const glm::fvec3& bc,
         const TGAImage& texture,
         TGAColor& color
   ) = 0;
};
