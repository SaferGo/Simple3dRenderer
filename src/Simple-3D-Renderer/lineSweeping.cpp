// Line swapping algorithm
// 4 cases:
//    1) All the vertices have the same 'y' value.
//    2) The lowest 2 vertices have the same 'y' value.
//    2) The top 2 vertices have the same 'y' value.
//    3) The 3 vertices have different 'y' values.
void drawTriangle(
   glm::vec2& p1,
   glm::vec2& p2,
   glm::vec2& p3,
   TGAImage& image,
   const TGAColor color
) {
   orderInAscendingOrdY(p1, p2 ,p3);

   int largeHeight = p3.y - p1.y;
   int smallHeight = p2.y - p1.y;
   glm::vec2 largeDirection = p3 - p1;
   glm::vec2 smallDirection = p2 - p1;

   if (p1.y == p2.y && p2.y == p3.y)
   {
      orderInAscendingOrdX(p1, p2, p3);
      drawLine(p1, p3, image, color);

      return;
   }
   
   // If p2 and p1 have the same height it means that we don't
   // need to fill the first part of the triangle.
   if (smallHeight != 0)
   {
      // Draws first part of the triangle
      for (int i = p1.y; i <= p2.y; i++)
      {
         float bigMagnitude   = float(i - p1.y) / largeHeight;
         float smallMagnitude = float(i - p1.y) / smallHeight;

         glm::vec2 newP1 = p1 + (largeDirection * bigMagnitude);
         glm::vec2 newP2 = p1 + (smallDirection * smallMagnitude);

         for (int j = newP1.x; j <= newP2.x; j++)
             image.set(j, p1.y + i, color);
      }
   }

   // Draws second part of the triangle
   smallDirection = p3 - p2;
   smallHeight    = p3.y - p2.y;

   if (smallHeight != 0)
   {
      for (int i = p2.y; i <= p3.y; i++)
      {
         float bigMagnitude   = float(i - p1.y) / largeHeight;
         float smallMagnitude = float(i - p2.y) / smallHeight;

         glm::vec2 newP1 = p1 + (largeDirection * bigMagnitude);
         glm::vec2 newP2 = p2 + (smallDirection * smallMagnitude);

         for (int j = newP1.x; j <= newP2.x; j++)
            image.set(j, p1.y + i, color);
      }
   }
}

