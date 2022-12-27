#include <Simple3dRenderer/Model/Model.h>

#include <iostream>
#include <string>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <tga/TGAImage.h>

#include <Simple3dRenderer/Settings/config.h>

Model::Model(const std::string& file)
{

   readModel(file);

   if (config::DIFFUSE_TEXTURE_TO_READ == "")
      m_hasTexture = false;
   else
   {
      m_hasTexture = true;
      loadDiffuseTexture(config::DIFFUSE_TEXTURE_TO_READ);
   }

   if (config::NORMAL_TEXTURE_TO_READ == "")
      m_hasNormals = false;
   else
   {
      m_hasNormals = true;
      loadNormalTexture(config::NORMAL_TEXTURE_TO_READ);
   }

   loadSpecularTexture(config::SPECULAR_TEXTURE_TO_READ);

   calculateTangents();
}

const bool Model::hasNormals() const
{
   return m_hasNormals;
}

const bool Model::hasTexture() const
{
   return m_hasTexture;
}

void Model::readModel(const std::string& file)
{
   m_readerConfig.mtl_search_path = "../assets/obj/";
   
   if (!m_reader.ParseFromFile(file, m_readerConfig))
   {
      if (!m_reader.Error().empty())
         std::cerr << "TinyObjReader: " << m_reader.Error();
   }
   if (!m_reader.Warning().empty()) {
      std::cout << "TinyObjReader: " << m_reader.Warning();
   }

   m_attrib  = m_reader.GetAttrib();
   m_meshes = m_reader.GetShapes();
}


void Model::loadDiffuseTexture(const std::string& file)
{
   m_diffuseTexture.read_tga_file(file);
   m_diffuseTexture.flip_vertically();
}

void Model::loadNormalTexture(const std::string& file)
{
   m_normalTexture.read_tga_file(file);
   m_normalTexture.flip_vertically();
}

void Model::loadSpecularTexture(const std::string& file)
{
   m_specularTexture.read_tga_file(file);
   m_specularTexture.flip_vertically();
}

const size_t Model::getVertexIndex(
      const size_t meshIndex,
      const size_t vertexIndex
) const 
{
   return m_meshes[meshIndex].mesh.indices[vertexIndex].vertex_index;
}

const TGAImage& Model::getDiffuseTexture() const
{
   return m_diffuseTexture;
}

const TGAImage& Model::getSpecularTexture() const
{
   return m_specularTexture;
}

const TGAImage& Model::getNormalTexture() const
{
   return m_normalTexture;
}

const glm::fvec4 Model::getVertex(
      const size_t meshIndex,
      const size_t vertexIndex
) const
{
   tinyobj::index_t i = m_meshes[meshIndex].mesh.indices[vertexIndex];

   return glm::fvec4(
         m_attrib.vertices[3 * size_t(i.vertex_index) + 0],
         m_attrib.vertices[3 * size_t(i.vertex_index) + 1],
         m_attrib.vertices[3 * size_t(i.vertex_index) + 2],
         1.0 // 0.0 -> for direction and 1.0 -> for points/vertices
   );
}

const glm::fvec3 Model::getNormal(
      const size_t meshIndex,
      const size_t vertexIndex
) const
{
   tinyobj::index_t i = m_meshes[meshIndex].mesh.indices[vertexIndex];
   
   return glm::fvec3(
         m_attrib.normals[3 * size_t(i.normal_index) + 0],
         m_attrib.normals[3 * size_t(i.normal_index) + 1],
         m_attrib.normals[3 * size_t(i.normal_index) + 2]
   );
}

const glm::fvec2 Model::getUV(
      const size_t meshIndex,
      const size_t vertexIndex
) const
{

   tinyobj::index_t i = m_meshes[meshIndex].mesh.indices[vertexIndex];

   return glm::fvec2(
         m_attrib.texcoords[2 * size_t(i.texcoord_index) + 0],
         m_attrib.texcoords[2 * size_t(i.texcoord_index) + 1]
   );
}

const glm::fvec4& Model::getAvgTangent(
      const size_t meshIndex,
      const size_t vertexIndex
) const
{
   size_t realVertexIndex = getVertexIndex(meshIndex, vertexIndex);

   return m_tangent[realVertexIndex];
}

const glm::fvec3& Model::getAvgNormal(
      const size_t meshIndex,
      const size_t vertexIndex
) const
{
   size_t realVertexIndex = getVertexIndex(meshIndex, vertexIndex);

   return m_normal[realVertexIndex];
}

const size_t Model::getCountOfFaces(const size_t shapeIndex) const
{
   return m_meshes[shapeIndex].mesh.num_face_vertices.size();
}

const size_t Model::getCountOfVerticesInFace(
      const size_t shapeIndex,
      const size_t faceIndex
) const {

   return size_t(m_meshes[shapeIndex].mesh.num_face_vertices[faceIndex]);
}

const size_t Model::getCountOfMeshes() const
{
   return m_meshes.size();
}

void Model::calculateTangents()
{
   glm::fvec3 bitangent[7000];

   for (size_t s = 0; s < m_meshes.size(); s++)
   {
      size_t indexOffset = 0;

      for (size_t f = 0; f < getCountOfFaces(s); f++)
      {
         size_t fv = getCountOfVerticesInFace(s, f);

         glm::fvec4 v[3];
         glm::fvec2 t[3];
         glm::fvec3 n[3];

         // Loop over vertices in the face.
         for (size_t vi = 0; vi < fv; vi++)
         {
            v[vi] = getVertex(s, indexOffset + vi);
            t[vi] = getUV(s, indexOffset + vi);
            n[vi] = getNormal(s, indexOffset + vi);
         }
         // Normal avg
         glm::fvec3 edge1 = v[1] - v[0];
         glm::fvec3 edge2 = v[2] - v[0];

         glm::fvec3 newNormal = glm::cross(edge1, edge2);

         // Tangent avg
         float x1 = t[1].x - t[0].x;
         float x2 = t[2].x - t[0].x;
         float y1 = t[1].y - t[0].y;
         float y2 = t[2].y - t[0].y;

         float r = 1.0f / (x1 * y2 - x2 * y1);

         glm::fvec3 newTangent = r * (edge1 * y2 - edge2 * y1);
         glm::fvec3 newBitangent = r * (edge2 * x1 - edge1 * x2);

         for (size_t vi = 0; vi < fv; vi++)
         {
            int realVertexIndex = getVertexIndex(s, indexOffset + vi);

            m_tangent[realVertexIndex] += glm::fvec4(newTangent, 0.0);
            bitangent[realVertexIndex] += newBitangent;

            m_normal[realVertexIndex] += newNormal;
         }

         indexOffset += fv;
      }
   }
   
   // Orthogonalization of the vectors
   // Gram-Schmidt orthogonalization
   for (size_t s = 0; s < m_meshes.size(); s++)
   {
      size_t indexOffset = 0;

      for (size_t f = 0; f < getCountOfFaces(s); f++)
      {
         size_t fv = getCountOfVerticesInFace(s, f);

         // Loop over vertices in the face.
         for (size_t vi = 0; vi < fv; vi++)
         {
            int realVertexIndex = getVertexIndex(s, indexOffset + vi);

            glm::fvec3 normal = m_normal[realVertexIndex];
            
            glm::fvec3 newTangent = glm::fvec3(m_tangent[realVertexIndex]);

            newTangent = (
                  newTangent -
                  normal * glm::dot(normal, newTangent)
            );

            newTangent = glm::normalize(newTangent);


            float sigma = (
                  glm::dot(
                     glm::cross(normal, newTangent),
                     bitangent[realVertexIndex]
                  ) < 0.0f ? -1.0f : 1.0f
            );
            
            m_tangent[realVertexIndex] = glm::fvec4(
                  newTangent,
                  sigma
            );

            m_normal[realVertexIndex] = glm::normalize(
                  m_normal[realVertexIndex]
            );
         }

         indexOffset += fv;
      }
   }
   
}

Model::~Model() {}
