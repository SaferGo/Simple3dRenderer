#include <Simple3dRenderer/Renderer.h>

#include <memory>
#include <iostream>

Renderer::Renderer()
{
   m_outputImage = TGAImage(
         config::RESOLUTION_WIDTH,
         config::RESOLUTION_HEIGHT,
         TGAImage::RGB
   );

   // init zBuffer
   for (int i = 0; i < config::RESOLUTION_WIDTH; i++)
      for (int j = 0; j < config::RESOLUTION_HEIGHT; j++)
         m_zBuffer[i][j] = std::numeric_limits<float>::max();
}

Renderer::~Renderer() {}

void Renderer::draw(
      const Model& model,
      const std::shared_ptr<IShader>& shader
) {
   for (size_t s = 0; s < model.getCountOfMeshes(); s++)
   {
      // Loop over faces(polygon)
      size_t indexOffset = 0;

      for (size_t f = 0; f < model.getCountOfFaces(s); f++)
      {
         size_t fv = model.getCountOfVerticesInFace(s, f);

         bool isInClipSpace = true;
         glm::fvec4 vertices[3];

         // Loop over vertices in the face.
         for (size_t v = 0; v < fv; v++)
         {
            
            vertices[v] = shader->vertex(
                  s,
                  indexOffset + v,
                  v,
                  model
            );

            // If it's in clip space.
            if (vertices[v] == glm::fvec4(-1))
            {
               isInClipSpace = false;
               break;

            }

         }

         if (isInClipSpace == false)
            continue;

         drawTriangle(vertices, shader, model);

         indexOffset += fv;
      }
   }
}


void Renderer::drawTriangle(
   glm::fvec4 (&v)[3],
   const std::shared_ptr<IShader>& shader,
   const Model& model
) {

   // Makes the perspec. division and transforms the vertices to 
   // screen space coords.
   glm::mat4 uniformViewportM4 = vertexProcessing::getViewportM4();
   for (uint8_t i = 0; i < 3; i++)
   {
      vertexProcessing::makePerspectiveDivision(v[i]);
      v[i] = uniformViewportM4 * v[i];
   }

   // Face-culling
   // If the face is back-facing we'll discard it.
   // Also, if the area == 0 -> the triangle is degenerated.
   float area = mathUtils::edgeFunction(v[0], v[1], v[2]);
   if (area >= 0)
      return;
   
   glm::fvec2 boundingBoxMin, boundingBoxMax;
   mathUtils::getBoundingBoxes(v, boundingBoxMin, boundingBoxMax);

   glm::ivec2 p;
   for (p.x = boundingBoxMin.x; p.x <= boundingBoxMax.x; p.x++)
   {
      for (p.y = boundingBoxMin.y; p.y <= boundingBoxMax.y; p.y++)
      {
         glm::fvec3 bc = mathUtils::getBarycentricCoords(v, area, p);

         // Point outside of triangle.
         if (bc.x == -1 || bc.y == -1 || bc.z == -1)
            continue;

         TGAColor color;
         shader->fragment(bc, model, color);

         float depth = mathUtils::getDepth(v, bc);
         
         // Remember that after we made the view transformations the objects
         // in front of us are +z.
         if (depth < m_zBuffer[p.x][p.y])
         {
            m_zBuffer[p.x][p.y] = depth;

            m_outputImage.set(p.x, p.y, color);
         }
      }
   }
}

void Renderer::render(const Model& model, const Camera& camera)
{
   std::shared_ptr<IShader> shader = std::make_shared<GouradShader>();
   shader->uniformModelM4      = vertexProcessing::getModelM4();
   shader->uniformViewM4       = vertexProcessing::getViewM4(camera);
   shader->uniformViewM3       = vertexProcessing::getViewM3(camera);
   shader->uniformProjectionM4 = vertexProcessing::getProjectionM4(
         camera.frustum
   );
   shader->uniformSa           = glm::transpose(
         glm::inverse(shader->uniformViewM4)
   );

   draw(model, shader);

   m_outputImage.write_tga_file("../output/output.tga");
}

