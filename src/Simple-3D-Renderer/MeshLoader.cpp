#include <Simple-3D-Renderer/MeshLoader.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>
#include <tga/TGAImage.h>

#include <iostream>
#include <string>

MeshLoader::MeshLoader(const std::string& file)
{

   m_readerConfig.mtl_search_path = "./";
   
   if (!m_reader.ParseFromFile(file, m_readerConfig))
   {
      if (!m_reader.Error().empty())
         std::cerr << "TinyObjReader: " << m_reader.Error();
   }
   if (!m_reader.Warning().empty()) {
      std::cout << "TinyObjReader: " << m_reader.Warning();
   }

   m_attrib  = m_reader.GetAttrib();
   m_objects = m_reader.GetShapes();
}


void MeshLoader::loadTexture(const std::string& file)
{
   m_texture.read_tga_file(file);
   m_texture.flip_vertically();
}

const TGAImage& MeshLoader::getTexture() const
{
   return m_texture;
}

glm::fvec4 MeshLoader::getVertex(
      const size_t objIndx,
      const size_t faceIndx,
      const size_t vertexIndx
) const {
   tinyobj::index_t i = m_objects[objIndx].mesh.indices[faceIndx + vertexIndx];

   return glm::fvec4(
         m_attrib.vertices[3 * size_t(i.vertex_index) + 0],
         m_attrib.vertices[3 * size_t(i.vertex_index) + 1],
         m_attrib.vertices[3 * size_t(i.vertex_index) + 2],
         1.0 // 0.0 -> for direction and 1.0 -> for points/vertices
   );
}

glm::fvec4 MeshLoader::getNormal(
      const size_t objIndx,
      const size_t faceIndx,
      const size_t vertexIndx
) const {
   tinyobj::index_t i = m_objects[objIndx].mesh.indices[faceIndx + vertexIndx];
   
   return glm::fvec4(
         m_attrib.normals[3 * size_t(i.normal_index) + 0],
         m_attrib.normals[3 * size_t(i.normal_index) + 1],
         m_attrib.normals[3 * size_t(i.normal_index) + 2],
         0.0
   );
}

glm::fvec2 MeshLoader::getUVcoords(
      const size_t objIndx,
      const size_t faceIndx,
      const size_t vertexIndx
) const {
   tinyobj::index_t i = m_objects[objIndx].mesh.indices[faceIndx + vertexIndx];

   return glm::fvec2(
         m_attrib.texcoords[2 * size_t(i.texcoord_index) + 0],
         m_attrib.texcoords[2 * size_t(i.texcoord_index) + 1]
   );
}

size_t MeshLoader::getNobjects() const
{
   return m_objects.size();
}

size_t MeshLoader::getNfaces(const size_t shapeIndx) const
{
   return m_objects[shapeIndx].mesh.num_face_vertices.size();
}

size_t MeshLoader::getNverticesInFace(
      const size_t shapeIndx,
      const size_t faceIndx
) const {

   return size_t(m_objects[shapeIndx].mesh.num_face_vertices[faceIndx]);
}
