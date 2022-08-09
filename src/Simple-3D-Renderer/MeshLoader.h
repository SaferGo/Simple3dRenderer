#pragma once

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>
#include <tga/TGAImage.h>

#include <string>

class MeshLoader
{
public:

   explicit MeshLoader(const std::string& file);
   void loadTexture(const std::string& file);
   const TGAImage& getTexture() const;
   glm::fvec4 getVertex(
         const size_t objIndx,
         const size_t faceIndx,
         const size_t vertexIndx
   ) const;
   glm::fvec2 getUVcoords(
         const size_t objIndx,
         const size_t faceIndx,
         const size_t vertexIndx
   ) const;
   glm::fvec4 getNormal(
         const size_t objIndx,
         const size_t faceIndx,
         const size_t vertexIndx
   ) const;
   size_t getNobjects() const;
   size_t getNfaces(const size_t objIndx) const;
   size_t getNverticesInFace(const size_t objIndx, const size_t faceIndx) const;

private:

   TGAImage m_texture;
   tinyobj::ObjReaderConfig m_readerConfig;
   tinyobj::ObjReader m_reader;
   tinyobj::attrib_t m_attrib;
   std::vector<tinyobj::shape_t> m_objects;
};
