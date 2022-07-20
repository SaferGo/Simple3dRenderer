#include <glm/glm.hpp>
#include <tga/TGAImage.h>
#include <ObjFileReader/Model.h>

#include <iostream>

#define WHITE TGAColor(255, 255, 255, 255)
#define WIDTH 800
#define HEIGHT 800

enum SlopeType
{
   POSITIVE,
   NEGATIVE,
   ZERO,
   UNDEFINED
};

void swap(glm::vec2& x, glm::vec2& y)
{
   glm::vec2 tmp = x;
   x = y;
   y = tmp;
}

// The function f takes for granted that p1 is on the left side(or bottom
// if the slope is undefined).
struct Line
{
   glm::vec2 p1;
   glm::vec2 p2;
   float m;
   SlopeType slopeType;

   explicit Line(const glm::vec2& otherP1, const glm::vec2& otherP2)
      : p1(otherP1), p2(otherP2)
   {
      slopeType = getSlopeType();
   }

   float f(const float x, const float y)
   {
      // SAVE THIS
      float t1 = p1.y - p2.y;
      float t2 = p2.x - p1.x;
      float t3 = p1.x * p2.y - p2.x * p1.y;
      return  ( t1 * x + t2 * y + t3 );
   }

   // Deberia ir en util
   // It also swaps the points if p2.x < p1.x
   // or if p2.y < p1.y in the case of undefined slope.
   SlopeType getSlopeType()
   {
      // P1 will always be the one of the left side.
      if (p2.x < p1.x)
         swap(p1, p2);

      if (p1.x == p2.x)
      {
         // P1 will be on the bottom side in this case.
         if (p2.y < p1.y)
            swap(p1, p2);
         return SlopeType::UNDEFINED;
      } else
      {
         m = (p2.y - p1.y) / (p2.x - p1.x);
         if (p1.y == p2.y)
            return SlopeType::ZERO;
         else if (p1.y < p2.y)
            return SlopeType::POSITIVE;
         else
            return SlopeType::NEGATIVE;
      }
   }

};

void drawLine(
      const glm::vec2 p1,
      const glm::vec2 p2,
      TGAImage& image,
      const TGAColor color
) {

   Line line(p1, p2);

   if (line.slopeType == SlopeType::POSITIVE)
   {
      if (line.m < 1)
      {
         int y = line.p1.y;
         for (int x = line.p1.x; x <= line.p2.x; x++)
         {
            image.set(x, y, color);

            if (line.f(x + 1, y + 0.5) < 0)
               y += 1;
         }
      } else
      {
         int x = line.p1.x;
         for (int y = line.p1.y; y <= line.p2.y; y++)
         {
            image.set(x, y, color);
            
            if (line.f(x + 0.5, y + 1) > 0)
               x += 1;
         }

      }
   } else if (line.slopeType == SlopeType::NEGATIVE)
   {
      if (line.m > -1)
      {
         int y = line.p1.y;
         for (int x = line.p1.x; x <= line.p2.x; x++)
         {
            image.set(x, y, color);

            if (line.f(x + 1, y - 0.5) > 0)
               y -= 1;
         }
      } else
      {
         int x = line.p1.x;
         for (int y = line.p1.y; y >= line.p2.y; y--)
         {
            image.set(x, y, color);
            
            if (line.f(x + 0.5, y - 1) < 0)
               x += 1;
         }

      }
      
   } else if (line.slopeType == SlopeType::ZERO)
   {
      for (int x = line.p1.x; x < line.p2.x; x++)
         image.set(x, line.p1.y, color);
   } else
   {
      for (int y = line.p1.y; y < line.p2.y; y++)
         image.set(line.p1.x, y, color);
   }
}

int main()
{
   TGAImage image(800, 800, TGAImage::RGB);

   Model model("../obj/FinalBaseMesh.obj");

   for (int i = 0; i < model.nfaces(); i++)
   {
      std::vector<int> face = model.face(i);
      for (int j = 0; j < 3; j++)
      {
         glm::vec3 v0 = model.vert(face[j]);
         glm::vec3 v1 = model.vert(face[(j + 1) % 3]);

         int x0 = (v0.x + 1.0) * WIDTH / 2.0;
         int y0 = (v0.y + 1.0) * HEIGHT / 2.0;
         int x1 = (v1.x + 1.0) * WIDTH / 2.0;
         int y1 = (v1.y + 1.0) * HEIGHT / 2.0;

         drawLine(glm::vec2(x0, y0), glm::vec2(x1, y1), image, WHITE);
      }
   }
   
   //image.flip_vertically();
   image.write_tga_file("output.tga");

   return 0;
}
