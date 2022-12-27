#include <Simple3dRenderer/Renderer.h>

int main()
{
   Renderer app;

   Model model(config::MODEL_TO_READ);
   Camera camera(
         config::eyeP,
         config::targetP,
         config::upV,
         config::zNear,
         config::zFar,
         config::fovY
   );

   app.render(model, camera);

   return 0;
}
