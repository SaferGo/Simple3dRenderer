#include <cstring>
#include <limits>
#include <memory>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <tga/TGAImage.h>
#include <tiny_obj_loader.h>

#include <Simple3dRenderer/Math/mathUtils.h>
#include <Simple3dRenderer/Settings/config.h>
#include <Simple3dRenderer/Vertex/vertexProcessing.h>
#include <Simple3dRenderer/Texture/textureProcessing.h>
#include <Simple3dRenderer/Model/Model.h>
#include <Simple3dRenderer/Shaders/GouradShader.h>
#include <Simple3dRenderer/Shaders/NormalMapping.h>

class Renderer
{

public:

   Renderer();
   ~Renderer();
   void render(const Model& model, const Camera& camera);

private:

   void draw(const Model& model, const std::shared_ptr<IShader>& shader);
   void drawTriangle(
         glm::fvec4 (&v)[3],
         const std::shared_ptr<IShader>& shader,
         const Model& model
   );

   TGAImage m_outputImage;
   float m_zBuffer[config::RESOLUTION_WIDTH][config::RESOLUTION_HEIGHT];

};
