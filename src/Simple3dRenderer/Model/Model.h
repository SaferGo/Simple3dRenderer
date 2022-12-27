#pragma once

#include <string>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>
#include <tga/TGAImage.h>

class Model
{

public:

   explicit Model(const std::string& file);
   ~Model();
   const TGAImage& getDiffuseTexture() const;
   const TGAImage& getNormalTexture() const;
   const TGAImage& getSpecularTexture() const;
   const glm::fvec4 getVertex(
         const size_t meshIndex,
         const size_t vertexIndex
   ) const;
   const glm::fvec2 getUV(
         const size_t meshIndex,
         const size_t vertexIndex
   ) const;
   const glm::fvec3 getNormal(
         const size_t meshIndex,
         const size_t vertexIndex
   ) const;
   const glm::fvec4& getAvgTangent(
         const size_t meshIndex,
         const size_t vertexIndex
   ) const;
   const glm::fvec3& getAvgNormal(
         const size_t meshIndex,
         const size_t vertexIndex
   ) const;
   const bool hasNormals() const;
   const bool hasTexture() const;
   const size_t getCountOfFaces(const size_t meshIndex) const;
   const size_t getCountOfMeshes() const;
   const size_t getCountOfVerticesInFace(
         const size_t meshIndex,
         const size_t faceIndex
   ) const;
   const size_t getVertexIndex(
         const size_t meshIndex,
         const size_t vertexIndex
   ) const;

private:

   void readModel(const std::string& file);
   void loadDiffuseTexture(const std::string& file);
   void loadNormalTexture(const std::string& file);
   void loadSpecularTexture(const std::string& file);
   void calculateTangents();


   std::vector<tinyobj::shape_t> m_meshes;
   TGAImage m_diffuseTexture;
   TGAImage m_normalTexture;
   TGAImage m_specularTexture;
   bool m_hasNormals;
   bool m_hasTexture;

   glm::fvec4 m_tangent[7900];
   glm::fvec3 m_normal[7900];

   tinyobj::ObjReaderConfig m_readerConfig;
   tinyobj::ObjReader m_reader;
   tinyobj::attrib_t m_attrib;

};
