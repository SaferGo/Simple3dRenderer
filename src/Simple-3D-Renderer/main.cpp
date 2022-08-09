#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tga/TGAImage.h>
#include <tiny_obj_loader.h>

#include <Simple-3D-Renderer/Math/util.h>
#include <Simple-3D-Renderer/Settings/config.h>
#include <Simple-3D-Renderer/VertexProcessing/vertexProcessing.h>
#include <Simple-3D-Renderer/TextureProcessing/textureProcessing.h>
#include <Simple-3D-Renderer/MeshLoader.h>
#include <Simple-3D-Renderer/Shaders/GouradShader.h>

#include <iostream>
#include <cstring>
#include <limits>
#include <memory>

#define WHITE TGAColor(255, 255, 255, 255)
#define RED TGAColor(255, 0, 0, 255)
#define GREEN TGAColor(0, 255, 0, 255)

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

   // To use them as boundaries in the for-loop
   boundingBoxMin.x = glm::floor(boundingBoxMin.x);
   boundingBoxMin.y = glm::floor(boundingBoxMin.y);
   boundingBoxMax.x = glm::floor(boundingBoxMax.x);
   boundingBoxMax.y = glm::floor(boundingBoxMax.y);
}

float getDepth(const glm::fvec4 (&v)[3], const glm::fvec3& bc)
{
   return v[0].z * bc.x + v[1].z * bc.y + v[2].z * bc.z;
}

void drawTriangle(
   glm::fvec4 (&v)[3],
   std::unique_ptr<IShader> shader,
   TGAImage& image,
   const TGAImage& texture,
   float (&zBuffer)[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT]
) {
   
   // Face-culling
   // If the face is back-facing we'll discard it.
   // Also, if the area == 0 -> the triangle is degenerated.
   float area = edgeFunction(v[0], v[1], v[2]);
   if (area >= 0)
      return;
   
   glm::fvec2 boundingBoxMin, boundingBoxMax;
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

         TGAColor color;
         shader->fragment(bc, texture, color);

         float depth = getDepth(v, bc);
         
         // Remember that after we made the view transformations the objects
         // in front of us are +z.
         if (depth < zBuffer[p.x][p.y])
         {
            zBuffer[p.x][p.y] = depth;

            image.set(p.x, p.y, color);
         }
      }
   }
}



int main()
{
   MeshLoader mesh(config::MODEL_TO_READ);
   mesh.loadTexture(config::TEXTURE_TO_READ);
   TGAImage image(
         config::RESOLUTION_WIDTH,
         config::RESOLUTION_HEIGHT,
         TGAImage::RGB
   );

   Camera camera(config::eyeP, config::targetP, config::upV);
   camera.createFrustum(config::zNear, config::zFar, config::fovY);

   GouradShader shader;
   shader.uniformModelM      = vertexProcessing::getModelM();
   shader.uniformViewM       = vertexProcessing::getViewM(camera);
   shader.uniformProjectionM = vertexProcessing::getProjectionM(camera.frustum);
   shader.uniformViewportM   = vertexProcessing::getViewportM();
   shader.uniformSa          = glm::transpose(
         glm::inverse(shader.uniformViewM)
   );

   float zBuffer[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT];
   for (int i = 0; i < config::RESOLUTION_WIDTH; i++)
      for (int j = 0; j < config::RESOLUTION_HEIGHT; j++)
         zBuffer[i][j] = std::numeric_limits<float>::max();

   for (size_t s = 0; s < mesh.getNobjects(); s++)
   {
      // Loop over faces(polygon)
      size_t index_offset = 0;

      for (size_t f = 0; f < mesh.getNfaces(s); f++)
      {
         size_t fv = mesh.getNverticesInFace(s, f);

         bool isInClipSpace = true;
         glm::fvec4 screenCoords[3];
         // Loop over vertices in the face.
         for (size_t v = 0; v < fv; v++)
         {
            screenCoords[v] = shader.vertex(s, index_offset, v, mesh);
            if (screenCoords[v] == glm::fvec4(-1))
            {
               isInClipSpace = false;
               break;
            }
         }

         if (isInClipSpace == false)
            continue;

         drawTriangle(
               screenCoords,
               std::make_unique<GouradShader>(shader),
               image,
               mesh.getTexture(),
               zBuffer
         );

         index_offset += fv;
      }
   }

   //image.flip_vertically();
   image.write_tga_file("../output/output.tga");

   return 0;
}
