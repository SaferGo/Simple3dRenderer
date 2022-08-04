#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tga/TGAImage.h>
#include <tiny_obj_loader.h>

#include <Simple-3D-Renderer/Math/util.h>
#include <Simple-3D-Renderer/Settings/config.h>

#include <iostream>
#include <cstring>
#include <limits>

#define WHITE TGAColor(255, 255, 255, 255)
#define RED TGAColor(255, 0, 0, 255)
#define GREEN TGAColor(0, 255, 0, 255)

//Delete
glm::fvec3 nn;
glm::fvec3 lightDir = glm::normalize(glm::fvec3(0, 0, -1));

glm::mat4 getModelM()
{
   return glm::mat4(1.0);
}

glm::mat4 getViewM(const glm::fvec3 (&camera)[3], const glm::fvec3 eyeP)
{
   glm::mat4 V(1.0);
   V[0][0] = camera[0].x;
   V[1][0] = camera[0].y;
   V[2][0] = camera[0].z;

   V[0][1] = camera[1].x;
   V[1][1] = camera[1].y;
   V[2][1] = camera[1].z;

   V[0][2] = camera[2].x;
   V[1][2] = camera[2].y;
   V[2][2] = camera[2].z;

   V[3][0] = (
         -camera[0].x * eyeP.x -
         camera[0].y * eyeP.y -
         camera[0].z * eyeP.z
   );
   V[3][1] = (
         -camera[1].x * eyeP.x -
         camera[1].y * eyeP.y -
         camera[1].z * eyeP.z
   );
   V[3][2] = (
         -camera[2].x * eyeP.x -
         camera[2].y * eyeP.y -
         camera[2].z * eyeP.z
   );

   return V;
}

void getFrustum(
      float& left,
      float& right,
      float& bottom,
      float& top,
      const float zNear,
      const float fovY
) {
   const float tangent = glm::tan(fovY * 0.5);
   const float height = zNear * tangent;
   const float width = height * config::ASPECT;

   left = -width;
   right = width;
   bottom = -height;
   top = height;
}

glm::mat4 getProjectionM(float zNear, float zFar, float fovY)
{
   float left, right, bottom, top;
   getFrustum(left, right, bottom, top, zNear, fovY);
   
   glm::mat4 P(0.0);

   P[0][0] = (2 * zNear) / (right - left);
   P[1][1] = (2 * zNear) / (top - bottom);
   P[2][0] = (right + left) / (right - left);
   P[2][1] = (top + bottom) / (top - bottom);
   P[2][2] = - (zFar + zNear) / (zFar - zNear);
   P[2][3] = -1;
   P[3][2] = -(2 * zFar * zNear) / (zFar - zNear);

   return P;
}


void getCamera(
      glm::fvec3 (&camera)[3],
      const glm::fvec3 eyeP,
      const glm::fvec3 targetP,
      const glm::fvec3 upV
) {
   glm::fvec3 z = glm::normalize(eyeP - targetP);
   glm::fvec3 x = glm::cross(glm::normalize(upV), z);
   glm::fvec3 y = glm::cross(z, x);

   camera[0] = x;
   camera[1] = y;
   camera[2] = z;
}



void transformWorldToClipCoords(glm::fvec4& worldCoords)
{
   glm::fvec3 eyeP = glm::fvec3(0.5, 0.0, 1.8);
   glm::fvec3 targetP = glm::fvec3(0.0, 0.0, -1.0); 
   // Here if we change upV to -upV, the camera will rotate 180 degrees.
   glm::fvec3 upV = glm::fvec3(0.0, 1.0, 0.0);
   glm::fvec3 camera[3];
   getCamera(camera, eyeP, targetP, upV);

   glm::mat4 M  = getModelM();
   glm::mat4 V  = getViewM(camera, eyeP);
   // zFar = 255 for debugging purposes
   glm::mat4 P  = getProjectionM(0.001, 10.0, 1.3);

   std::cout << "ORIGINAL: " << worldCoords.z << std::endl;
   glm::vec4 t = M * worldCoords;
   std::cout << "AFTER M: " << t.z << std::endl;
   t = V * t;
   std::cout << "AFTER V: " << t.z << std::endl;
   t = P * t;
   std::cout << "AFTER P: " << t.z << std::endl;

   worldCoords = P * V * M * worldCoords;
}

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
      const glm::fvec3 (&v)[3],
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
      glm::fvec3 (&v)[3],
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
      const glm::fvec3 (&v)[3],
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
   // make clamp..
}

float getDepth(const float (&zValues)[3], const glm::fvec3& bc)
{
   return zValues[0] * bc.x + zValues[1] * bc.y + zValues[2] * bc.z;
}

glm::fvec3 getCoordsWithPerspectiveDivision(glm::fvec4& v)
{
   return glm::fvec3(
         v.x / v.w,
         v.y / v.w,
         v.z / v.w
   );
}

void transformClipToScreenCoords(glm::fvec3& v)
{
   // It transforms it to [0,2] and then to
   // [0, RESOLUTION_WIDTH], [0, RESOLUTION_HEIGHT], [0, 1]
   v =  glm::fvec3(
         (v.x + 1.0) * config::HALF_RESOLUTION_WIDTH,
         (v.y + 1.0) * config::HALF_RESOLUTION_HEIGHT,
         (v.z + 1.0) * 0.5
   );
}

void drawTriangle(
   glm::fvec4 (&coords)[3],
   glm::fvec2 (&t)[3],
   float (&li)[3],
   TGAImage& image,
   TGAImage& texture,
   float (&zBuffer)[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT]
) {

   for (int i = 0; i < 3; i++)
   {
      transformWorldToClipCoords(coords[i]);

      //if (isInClipSpace(v[i]) == false)
      //   return;
   }

   // After performing the perspective division, we won't need the w-value
   // anymore, so we'll use a 3d vector.
   glm::fvec3 v[3];
   for (int i = 0; i < 3; i++)
      v[i] = getCoordsWithPerspectiveDivision(coords[i]);

   std::cout << "COORDS PD: " << v[0].x << " " << v[0].y << std::endl;
   
   for (int i = 0; i < 3; i++)
   {
      transformClipToScreenCoords(v[i]);
      if (v[i].x > config::RESOLUTION_WIDTH ||
          v[i].x < 0 ||
          v[i].y > config::RESOLUTION_HEIGHT ||
          v[i].y < 0)
         return;
   }
   
   //std::cout << "COORDS SC: " << v[0].x << " " << v[0].y << v[0].z << std::endl;

   // Determines if the polygon is back-facing or front-facing
   // Also if the area == 0 -> the triangle is degenerated
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
         if (int(p.x) > config::RESOLUTION_WIDTH ||
             int(p.y) > config::RESOLUTION_HEIGHT||
             int(p.x) < 0 ||
             int(p.y) < 0)
            continue;
         glm::fvec3 bc = getBarycentricCoords(v, area, p);

         // It means that the point is outside of the triangle.
         if (bc.x == -1 || bc.y == -1 || bc.z == -1)
            continue;

         //float depth = getDepth(zValues, bc);
         
         //if (depth > zBuffer[p.x][p.y])
         //{
         //   zBuffer[p.x][p.y] = depth;

            //float u = t[0].x * bc.x + t[1].x * bc.y + t[2].x * bc.z;
            //float v = t[0].y * bc.x + t[1].y * bc.y + t[2].y * bc.z;

         //   //image.set(p.x, p.y, texture.get(tt.x, tt.y));
         //   //----------------------------------
         //   float lightIntensity = li[0] * bc.x + li[1] * bc.y + li[2] * bc.z;

         //   //if (lightIntensity < 0)
         //   //{
         //   //   image.set(
         //   //         p.x,
         //   //         p.y,
         //   //         //TGAColor(lightIntensity * 255, lightIntensity * 255, lightIntensity * 255, 255) //texture.get(u * config::RESOLUTION_WIDTH, v * config::RESOLUTION_HEIGHT) //WHITE
         //   //   );
         //   //}
         //   //image.set(p.x, p.y, TGAColor(depth * 255, depth * 255, depth * 255, 255));
         //   image.set(p.x, p.y, WHITE);
         //}
         float lightIntensity = glm::dot(nn,lightDir);
         image.set(
               p.x, p.y,
               TGAColor(
                  lightIntensity * 255,
                  lightIntensity * 255,
                  lightIntensity * 255
               )
         );
      }
   }
}


bool isInClipSpace(const glm::ivec3& clipCoords)
{
   //if (std::abs(clipCoords[i].x) >= config::RESOLUTION_WIDTH  ||
   //    std::abs(clipCoords[i].y) >= config::RESOLUTION_HEIGHT)
   //   return false;
   return true;
}


glm::ivec2 getTexelCoords(
      const float u,
      const float v,
      const TGAImage& texture
) {
   return glm::ivec2(u * texture.width(), v * texture.height());
}

int main()
{
   TGAImage image(
         config::RESOLUTION_WIDTH,
         config::RESOLUTION_HEIGHT,
         TGAImage::RGB
   );

   TGAImage texture;
   texture.read_tga_file("../assets/texture/cube.tga");
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

   float zBuffer[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT];
   for (int i = 0; i < config::RESOLUTION_WIDTH; i++)
      for (int j = 0; j < config::RESOLUTION_HEIGHT; j++)
         zBuffer[i][j] = std::numeric_limits<float>::min();

   for (size_t s = 0; s < shapes.size(); s++)
   {
      // Loop over faces(polygon)
      size_t index_offset = 0;

      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
      {
         glm::fvec4 worldCoords[3];
         glm::fvec2 textureCoords[3];
         float lightIntensity[3];

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
               textureCoords[int(v)] = glm::fvec2(
                     attrib.texcoords[2*size_t(idx.texcoord_index)+0],
                     attrib.texcoords[2*size_t(idx.texcoord_index)+1]
               );
            }

            if (idx.normal_index >= 0)
            {
               glm::fvec3 normal;
               normal.x = attrib.normals[3*size_t(idx.normal_index)+0];
               normal.y = attrib.normals[3*size_t(idx.normal_index)+1];
               normal.z = attrib.normals[3*size_t(idx.normal_index)+2];

               lightIntensity[v] = getLightIntensity(normal, lightDir);
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
         drawTriangle(
               worldCoords,
               textureCoords,
               lightIntensity,
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
