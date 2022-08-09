#pragma once

#include <Simple-3D-Renderer/Shaders/IShader.h>
#include <Simple-3D-Renderer/MeshLoader.h>

#include <glm/glm.hpp> 
#include <tga/TGAImage.h>

struct GouradShader : public IShader
{
   glm::fvec4 varyingV[3];
   glm::fvec4 varyingNormal[3];
   glm::fvec2 varyingUV[3];
   glm::fvec4 varyingLightDir;

   ~GouradShader() override;
   glm::fvec4 vertex(
         const size_t objIndx,
         const size_t faceIndx,
         const size_t vertexIndx,
         MeshLoader& mesh
   ) override;
   bool fragment(
         const glm::fvec3& bc,
         const TGAImage& texture,
         TGAColor& color
   ) override;
};
