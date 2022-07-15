#include <glm/glm.hpp>
#include <tga/TGAImage.h>

int main()
{
   TGAImage image(100, 100, TGAImage::RGB);
   image.set(52, 41, TGAColor(255, 0, 0, 255));
   image.flip_vertically();
   image.write_tga_file("output.tga");

   return 0;
}
