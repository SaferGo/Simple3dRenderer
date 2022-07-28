#include <glm/glm.hpp>
#include <tga/TGAImage.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <Simple-3D-Renderer/Math/util.h>
#include <Simple-3D-Renderer/Settings/config.h>

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <limits>

#define WHITE TGAColor(255, 255, 255, 255)
#define RED TGAColor(255, 0, 0, 255)
#define GREEN TGAColor(0, 255, 0, 255)

// .It tells us:
//    1) Which side of the line the point is on(in counter-clockwise form):
//       e(p) < 0 -> left side
//       e(p) = 0 -> on the edge
//       e(p) > 0 -> right side
//    2) The area of the parallelogram formed by these 3 vertices.
//
// .Also, this is the same as the 2D cross product of the vectors:
//
//    A = (p - v1)
//    B = (v2 - v1)
//
float edgeFunction(
      const glm::vec3& v1,
      const glm::vec3& v2,
      const glm::vec2& p
) {
   return (p.x - v1.x) * (v2.y - v1.y) - (p.y - v1.y) * (v2.x - v1.x);
}

// In addition to telling us if the point is in the triangle, It also
// gives us the area of the parallelograms(or the area of the triangles
// multiplied by 2) forms by V1V2P, V1V3P and V2V3P.
bool isPointInTriangle(
      const glm::ivec3 (&v)[3],
      const glm::ivec2& p,
      float (&area)[3]
) {
   for (int i = 0; i < 3; i++)
   {
      area[i] = edgeFunction(v[i], v[(i + 1) % 3], p);
      if (area[i] > 0)
         return false;
   }

   return true;
}

glm::fvec3 getBarycentricCoords(
      glm::ivec3 (&v)[3],
      const float area,
      const glm::vec2& p
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
      const glm::ivec3 (&v)[3],
      glm::ivec2& boundingBoxMin,
      glm::ivec2& boundingBoxMax
) {
   boundingBoxMin = glm::ivec2(
         config::RESOLUTION_WIDTH - 1,
         config::RESOLUTION_HEIGHT - 1
   );
   boundingBoxMax = glm::ivec2(0, 0);
   glm::ivec2 clamp = boundingBoxMin;

   for (int i = 0; i < 3; i++)
   {
      boundingBoxMin.x = std::max(
            0,
            std::min(boundingBoxMin.x, v[i].x)
      );
      boundingBoxMin.y = std::max(
            0,
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

bool isTriangleDegenerated(const glm::ivec3 (&v)[3])
{
   return (v[0].y == v[1].y && v[1].y == v[2].y);
}

float getDepth(const glm::ivec3 (&v)[3], const glm::fvec3& bc)
{
   return v[0].z * bc.x + v[1].z * bc.y + v[2].z * bc.z;
}

void drawTriangle(
   glm::ivec3 (&v)[3],
   TGAImage& image,
   const TGAColor color,
   float (&zBuffer)[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT]
) {

   if (isTriangleDegenerated(v))
      return;
   
   float area = edgeFunction(v[0], v[1], v[2]);
   if (area >= 0)
      return;
   
   glm::ivec2 boundingBoxMin, boundingBoxMax;
   getBoundingBoxes(v, boundingBoxMin, boundingBoxMax);

   glm::ivec2 p;
   for (p.x = boundingBoxMin.x; p.x <= boundingBoxMax.x; p.x++)
   {
      for (p.y = boundingBoxMin.y; p.y <= boundingBoxMax.y; p.y++)
      {
         glm::fvec3 bc = getBarycentricCoords(v, area, p);

         // It means that the point is outside of the triangle.
         if (bc.x == -1 || bc.y == -1 || bc.z == -1)
            continue;

         float depth = getDepth(v, bc);
         
         if (depth < zBuffer[p.x][p.y])
         {
            zBuffer[p.x][p.y] = depth;
            image.set(p.x, p.y, color);
         }
      }
   }
}

float getLightIntensity(const glm::fvec3 (&v)[3], const glm::fvec3& lightDir)
{
   glm::vec3 normal = glm::normalize(
      glm::cross(
         v[0] - v[1],
         v[2] - v[1]
      )
   );

   return glm::dot(normal, lightDir);
}

bool isInClipSpace(const glm::ivec3 (&screenCoords)[3])
{
   for (int i = 0; i < 3; i++)
   {
      if (std::abs(screenCoords[i].x) > config::RESOLUTION_WIDTH ||
          std::abs(screenCoords[i].y) > config::RESOLUTION_HEIGHT)
         return false;
   }
   return true;
}

// Transform world coordinates to screen coordinates.
glm::ivec3 getScreenCoords(const glm::fvec3& worldCoords)
{
   return glm::vec3(
         (worldCoords.x + 1.0) * config::HALF_RESOLUTION_WIDTH,
         (worldCoords.y + 1.0) * config::HALF_RESOLUTION_HEIGHT,
         worldCoords.z
   );
}

int main()
{
   TGAImage image(
         config::RESOLUTION_WIDTH,
         config::RESOLUTION_HEIGHT,
         TGAImage::RGB
   );

   //TGAImage texture;
   //texture.read_tga_file(("../obj/" + config::MODEL_TO_READ + ".tga").c_str());

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

   glm::fvec3 lightDir(0, 0, -1);

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
         glm::fvec3 worldCoords[3];
         glm::ivec3 screenCoords[3];
         glm::ivec3 textureCoords[3];

         size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

         // Loop over vertices in the face.
         for (size_t v = 0; v < fv; v++) {
            // access to vertex
            tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

            worldCoords[v].x = attrib.vertices[3*size_t(idx.vertex_index)+0];
            worldCoords[v].y = attrib.vertices[3*size_t(idx.vertex_index)+1];
            worldCoords[v].z = attrib.vertices[3*size_t(idx.vertex_index)+2];
            
            screenCoords[v] = getScreenCoords(worldCoords[v]);

            // Check if `texcoord_index` is zero or positive. negative = no texcoord data
            if (idx.texcoord_index >= 0) {
              tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
              tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
            }

            
         }
         if (isInClipSpace(screenCoords))
         {
            float intensity = getLightIntensity(worldCoords, lightDir);

            // En este caso justo coincide que lo usamos como face culling
            // porq la camara esta en la cordenada (0,0,0)
            if (intensity >= 0)
            {
               drawTriangle(
                     screenCoords,
                     image,
                     TGAColor(
                        intensity * 255,
                        intensity * 255,
                        intensity * 255,
                        intensity * 255
                     ),
                     zBuffer
               );
            }
         }


         index_offset += fv;

      }
   }

   ////////////

   //for (int i = 0; i < shapes[0].mesh.num_face_vertices.size(); i++)
   //{
   //   std::cout << " ESA " << shapes[0].mesh.num_face_vertices.size() << std::endl;
   //   glm::fvec3 worldCoords[3];
   //   glm::ivec3 screenCoords[3];
   //   glm::ivec3 textureCoords[3];

   //   for (int j = 0; j < 3; j++)
   //   {
   //      // access to vertex
   //      tinyobj::index_t idx = shapes[0].mesh.indices[j];
   //      worldCoords[j].x = attrib.vertices[3*size_t(idx.vertex_index)];
   //      worldCoords[j].y = attrib.vertices[3*size_t(idx.vertex_index)+1];
   //      worldCoords[j].z = attrib.vertices[3*size_t(idx.vertex_index)+2];
   //      screenCoords[j] = getScreenCoords(worldCoords[j]);
   //      // Check if `texcoord_index` is zero or positive. negative = no texcoord data
   //      //std::cout << "TENEMOS: " << worldCoords[j].x << " " << worldCoords[j].y << " " << worldCoords[j].z << std::endl;
   //      if (idx.texcoord_index >= 0) {
   //         tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
   //         tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
   //      }
   //   }

   //   
   //   if (isInClipSpace(screenCoords))
   //   {
   //      float intensity = getLightIntensity(worldCoords, lightDir);

   //      // En este caso justo coincide que lo usamos como face culling
   //      // porq la camara esta en la cordenada (0,0,0)
   //      if (intensity >= 0)
   //      {
   //         drawTriangle(
   //               screenCoords,
   //               image,
   //               TGAColor(
   //                  intensity * 255,
   //                  intensity * 255,
   //                  intensity * 255,
   //                  intensity * 255
   //               ),
   //               zBuffer
   //         );
   //      }
   //   }
   //}

   //image.flip_vertically();
   image.write_tga_file("../output/output.tga");

   return 0;
}
