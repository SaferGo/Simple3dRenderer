enum SlopeType
{
   POSITIVE,
   NEGATIVE,
   ZERO,
   UNDEFINED
};


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
      float A = p1.y - p2.y;
      float B = p2.x - p1.x;
      float C = p1.x * p2.y - p2.x * p1.y;
      return  ( A * x + B * y + C );
   }

   // Deberia ir en util
   // It also swaps the points if p2.x < p1.x
   // or if p2.y < p1.y in the case of undefined slope.
   SlopeType getSlopeType()
   {
      // P1 will always be the one of the left side.
      if (p2.x < p1.x)
         util::swap(p1, p2);

      if (p1.x == p2.x)
      {
         // P1 will be on the bottom side in this case.
         if (p2.y < p1.y)
            util::swap(p1, p2);
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


// Mid-point line algorithm
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

void drawEdges(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const glm::vec2& p3,
      TGAImage& image,
      const TGAColor color
) {
   drawLine(p1, p2, image, color);
   drawLine(p2, p3, image, color);
   drawLine(p3, p1, image, color);
}
