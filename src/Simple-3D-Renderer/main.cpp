#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tga/TGAImage.h>
#include <tiny_obj_loader.h>

#include <Simple-3D-Renderer/Math/util.h>
#include <Simple-3D-Renderer/Settings/config.h>
#include <Simple-3D-Renderer/VertexProcessing/vertexProcessing.h>
#include <Simple-3D-Renderer/TextureProcessing/textureProcessing.h>

#include <iostream>
#include <cstring>
#include <limits>

#define WHITE TGAColor(255, 255, 255, 255)
#define RED TGAColor(255, 0, 0, 255)
#define GREEN TGAColor(0, 255, 0, 255)

struct TransfMatrices
{
   glm::mat4 modelM;
   glm::mat4 viewM;
   glm::mat4 projectionM;
   glm::mat4 viewportM;
   glm::mat4 sa;
};

//Delete
glm::fvec3 nn;
glm::fvec4 lightDir = glm::normalize(glm::fvec4(0.5, 0.5, 1, 0));
glm::mat4 sa;

float getLightIntensity(const glm::fvec3& normal, const glm::fvec3& lightDir)
{
   return glm::dot(normal, lightDir);
}

// .It tells us:
//    1) Which side of the line the point is on(in counter-clockwise form):
//       e(p) < 0 -> inside the triagle
//       e(p) = 0 -> on the edge
//       e(p) > 0 -> outside of the triangle
//    2) The area of the parallelogram formed by these 3 vertices.
//
// .Also, this is the same as the 2D cross product of the vectors:
//
//    A = (p - v1)
//    B = (v2 - v1)
//
float edgeFunction(
      const glm::vec4& v1,
      const glm::vec4& v2,
      const glm::vec4& v3
) {
   return (v3.x - v1.x) * (v2.y - v1.y) - (v3.y - v1.y) * (v2.x - v1.x);
}

float edgeFunction(
      const glm::vec4& v1,
      const glm::vec4& v2,
      const glm::vec2& p
) {
   return (p.x - v1.x) * (v2.y - v1.y) - (p.y - v1.y) * (v2.x - v1.x);
}

// In addition to telling us if the point is in the triangle, It also
// gives us the area of the parallelograms(or the area of the triangles
// multiplied by 2) forms by V1V2P, V1V3P and V2V3P.
bool isPointInTriangle(
      const glm::fvec4 (&v)[3],
      const glm::fvec2& p,
      float (&area)[3]
) {
   area[0] = edgeFunction(v[1], v[2], p);
   area[1] = edgeFunction(v[2], v[0], p);
   area[2] = edgeFunction(v[0], v[1], p);

   for (int i = 0; i < 3; i++)
   {
      if (area[i] > 0)
         return false;
   }

   return true;
}

glm::fvec3 getBarycentricCoords(
      glm::fvec4 (&v)[3],
      const float area,
      const glm::fvec2& p
) {
   glm::fvec3 bc(-1);

   float miniArea[3];
   
   if (isPointInTriangle(v, p, miniArea))
   {
      bc.x = miniArea[0] / area;
      bc.y = miniArea[1] / area;
      bc.z = 1 - bc.x - bc.y;
   }

   return bc;
}

void getBoundingBoxes(
      const glm::fvec4 (&v)[3],
      glm::fvec2& boundingBoxMin,
      glm::fvec2& boundingBoxMax
) {
   boundingBoxMin = glm::fvec2(
         config::RESOLUTION_WIDTH,
         config::RESOLUTION_HEIGHT
   );
   boundingBoxMax = glm::fvec2(0.0);
   glm::fvec2 clamp = boundingBoxMin;

   for (int i = 0; i < 3; i++)
   {
      boundingBoxMin.x = std::max(
            0.0f,
            std::min(boundingBoxMin.x, v[i].x)
      );
      boundingBoxMin.y = std::max(
            0.0f,
            std::min(boundingBoxMin.y, v[i].y)
      );
      boundingBoxMax.x = std::min(
            clamp.x,
            std::max(boundingBoxMax.x, v[i].x)
      );
      boundingBoxMax.y = std::min(
            clamp.y,
            std::max(boundingBoxMax.y, v[i].y)
      );
   }
}

float getDepth(const glm::fvec4 (&v)[3], const glm::fvec3& bc)
{
   return v[0].z * bc.x + v[1].z * bc.y + v[2].z * bc.z;
}

// We will cut everything that is behind the zNear and everything that
// is in front of zFar.
bool isInClipSpace(const glm::fvec4& clipCoords)
{
   if (std::fabs(clipCoords.x) > 1.0 ||
       std::fabs(clipCoords.y) > 1.0 ||
       std::fabs(clipCoords.z) > 1.0)
      return false;
   return true;
}

void drawTriangle(
   glm::fvec4 (&v)[3],
   glm::fvec2 (&t)[3],
   glm::fvec4 (&n)[3],
   TGAImage& image,
   TGAImage& texture,
   float (&zBuffer)[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT]
) {
   
   // Face-culling
   // If the face if back-facing we'll discard it.
   // Also, if the area == 0 -> the triangle is degenerated.
   float area = edgeFunction(v[0], v[1], v[2]);
   if (area >= 0)
      return;
   
   glm::fvec2 boundingBoxMin, boundingBoxMax;
   getBoundingBoxes(v, boundingBoxMin, boundingBoxMax);

   glm::fvec2 p;
   for (p.x = glm::floor(boundingBoxMin.x); p.x <= glm::floor(boundingBoxMax.x); p.x++)
   {
      for (p.y = glm::floor(boundingBoxMin.y); p.y <= glm::floor(boundingBoxMax.y); p.y++)
      {
         glm::fvec3 bc = getBarycentricCoords(v, area, p);

         // It means that the point is outside of the triangle.
         if (bc.x == -1 || bc.y == -1 || bc.z == -1)
            continue;

         float depth = getDepth(v, bc);
         
         // Acordate que cuando hacemos la camera transformation, los objetos
         // delante nuestros pasan a ser +z(si estan delante de la camara).
         if (depth < zBuffer[int(p.x)][int(p.y)])
         {
            zBuffer[int(p.x)][int(p.y)] = depth;

            float lightIntensity = (
               glm::dot(
                  glm::transpose(sa) * glm::normalize(n[0]),
                  lightDir
               ) * bc.x +
               glm::dot(
                  glm::transpose(sa) * glm::normalize(n[1]),
                  lightDir
               ) * bc.y +
               glm::dot(
                  glm::transpose(sa) * glm::normalize(n[2]),
                  lightDir
               ) * bc.z
            );

            TGAColor texel = textureProcessing::sampleTexture(t, bc, texture);
            TGAColor light = TGAColor(
                  lightIntensity * 250,
                  lightIntensity * 250,
                  lightIntensity * 250
            );

            if (lightIntensity < 0)
               continue;

            texel[0] *= lightIntensity;
            texel[1] *= lightIntensity;
            texel[2] *= lightIntensity;

            image.set(
                  p.x, p.y,
                  texel
            );
         }
      }
   }
}



int main()
{
   TGAImage image(
         config::RESOLUTION_WIDTH,
         config::RESOLUTION_HEIGHT,
         TGAImage::RGB
   );

   TGAImage texture;
   texture.read_tga_file("../assets/texture/head.tga");
   texture.flip_vertically();

   tinyobj::ObjReaderConfig readerConfig;
   readerConfig.mtl_search_path = "./";
   tinyobj::ObjReader reader;

   if (!reader.ParseFromFile(config::MODEL_TO_READ, readerConfig))
   {
      if (!reader.Error().empty())
         std::cerr << "TinyObjReader: " << reader.Error();
   }
   if (!reader.Warning().empty()) {
      std::cout << "TinyObjReader: " << reader.Warning();
   }

   auto& attrib = reader.GetAttrib();
   auto& shapes = reader.GetShapes();

   // ------------------------------------
   const glm::fvec3 eyeP = glm::fvec3(-1.0, 0.0, 3.5);
   const glm::fvec3 targetP = glm::fvec3(0.0, 0.0, -1.0); 
   // Here if we change upV to -upV, the camera will rotate 180 degrees.
   const glm::fvec3 upV = glm::fvec3(0.0, 1.0, 0.0);

   Camera camera(eyeP, targetP, upV);

   const float zNear = 0.01;
   const float zFar = 100.0;
   const float fovY = 0.78;

   camera.createFrustum(zNear, zFar, fovY);
   // ------------------------------------

   TransfMatrices m;
   m.modelM      = vertexProcessing::getModelM();
   m.viewM       = vertexProcessing::getViewM(camera);
   m.projectionM = vertexProcessing::getProjectionM(camera.frustum);
   m.viewportM   = vertexProcessing::getViewportM();
   sa          = glm::inverse(m.viewM);

   float zBuffer[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT];
   for (int i = 0; i < config::RESOLUTION_WIDTH; i++)
      for (int j = 0; j < config::RESOLUTION_HEIGHT; j++)
         zBuffer[i][j] = std::numeric_limits<float>::max();

   for (size_t s = 0; s < shapes.size(); s++)
   {
      // Loop over faces(polygon)
      size_t index_offset = 0;

      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
      {
         glm::fvec4 worldCoords[3];
         glm::fvec2 textureCoords[3];
         glm::fvec4 normals[3];

         size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

         // Loop over vertices in the face.
         for (size_t v = 0; v < fv; v++)
         {
            // access to vertex
            tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

            worldCoords[v].x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
            worldCoords[v].y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
            worldCoords[v].z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
            // w = 0.0 -> for directions.
            // w = 1.0 -> for points/vertices.
            worldCoords[v].w = 1.0;

            if (idx.texcoord_index >= 0)
            {
               textureCoords[int(v)].x = 
                  attrib.texcoords[2*size_t(idx.texcoord_index)+0];
               textureCoords[int(v)].y =
                  attrib.texcoords[2*size_t(idx.texcoord_index)+1];
            }

            if (idx.normal_index >= 0)
            {
               normals[v].x = attrib.normals[3*size_t(idx.normal_index)+0];
               normals[v].y = attrib.normals[3*size_t(idx.normal_index)+1];
               normals[v].z = attrib.normals[3*size_t(idx.normal_index)+2];
               normals[v].w = 0;
            }

         }
         nn = glm::normalize(
               glm::cross(
                  glm::vec3(worldCoords[2].x, worldCoords[2].y, worldCoords[2].z) -
                  glm::vec3(worldCoords[0].x, worldCoords[0].y, worldCoords[0].z),
                  glm::vec3(worldCoords[1].x, worldCoords[1].y, worldCoords[1].z) -
                  glm::vec3(worldCoords[0].x, worldCoords[0].y, worldCoords[0].z)
               )
         );
         glm::fvec4 v[3];
         for (int ss = 0; ss < 3; ss++)
            v[ss] = worldCoords[ss];

         // Transformation Pipeline
         bool clipSp;
         for (int i = 0; i < 3; i++)
         {

            // Transfroms world to clip coordinates.
            v[i] = m.projectionM * m.viewM * m.modelM * v[i];

            // After performing the perspective division, we won't need the
            // w-value anymore.
            vertexProcessing::makePerspectiveDivision(v[i]);

            // improve this
            clipSp = isInClipSpace(v[i]);
            if (clipSp == false)
               break;

            // Transforms clip to screen coordinates.
            v[i] = m.viewportM * v[i];
         }

         if (clipSp == false)
            continue;

         drawTriangle(
               v,
               textureCoords,
               normals,
               image,
               texture,
               zBuffer
         );

         index_offset += fv;
      }
   }

   //image.flip_vertically();
   image.write_tga_file("../output/output.tga");

   return 0;
}
