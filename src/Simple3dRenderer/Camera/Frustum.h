#pragma once

struct Frustum
{
   float left;
   float right;
   float bottom;
   float top;

   float zNear;
   float zFar;
   float fovY;
   
   Frustum();
   Frustum(
         const float otherZnear,
         const float otherZfar,
         const float otherFovY
   );
};
