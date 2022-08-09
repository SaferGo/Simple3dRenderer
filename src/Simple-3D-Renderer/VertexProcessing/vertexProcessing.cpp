#include <Simple-3D-Renderer/VertexProcessing/vertexProcessing.h>

#include <Simple-3D-Renderer/Camera/Camera.h>
#include <Simple-3D-Renderer/Camera/Frustum.h>
#include <Simple-3D-Renderer/Settings/config.h>

#include <glm/glm.hpp> 

glm::mat4 vertexProcessing::getModelM()
{
   return glm::mat4(1.0);
}

glm::mat4 vertexProcessing::getViewM(const Camera& camera)
{
   glm::mat4 V(1.0);
   V[0][0] = camera.axis[0].x;
   V[1][0] = camera.axis[0].y;
   V[2][0] = camera.axis[0].z;

   V[0][1] = camera.axis[1].x;
   V[1][1] = camera.axis[1].y;
   V[2][1] = camera.axis[1].z;

   V[0][2] = camera.axis[2].x;
   V[1][2] = camera.axis[2].y;
   V[2][2] = camera.axis[2].z;

   V[3][0] = (
         -camera.axis[0].x * camera.position.x -
          camera.axis[0].y * camera.position.y -
          camera.axis[0].z * camera.position.z
   );
   V[3][1] = (
         -camera.axis[1].x * camera.position.x -
          camera.axis[1].y * camera.position.y -
          camera.axis[1].z * camera.position.z
   );
   V[3][2] = (
         -camera.axis[2].x * camera.position.x -
          camera.axis[2].y * camera.position.y -
          camera.axis[2].z * camera.position.z
   );

   return V;
}

glm::mat4 vertexProcessing::getProjectionM(const Frustum& frustum)
{
   glm::mat4 P(0.0);

   P[0][0] = (2 * frustum.zNear) / (frustum.right - frustum.left);
   P[1][1] = (2 * frustum.zNear) / (frustum.top - frustum.bottom);
   P[2][0] = (frustum.right + frustum.left) / (frustum.right - frustum.left);
   P[2][1] = (frustum.top + frustum.bottom) / (frustum.top - frustum.bottom);
   P[2][2] = - (frustum.zFar + frustum.zNear) / (frustum.zFar - frustum.zNear);
   P[2][3] = -1;
   P[3][2] = (
      -(2 * frustum.zFar * frustum.zNear) /
      (frustum.zFar - frustum.zNear)
   );

   return P;
}

glm::mat4 vertexProcessing::getViewportM()
{
   // It transforms it to [0,2] and then to
   // [0, RESOLUTION_WIDTH], [0, RESOLUTION_HEIGHT], [0, 1]

   glm::mat4 VP(0.0);

   VP[0][0] = config::HALF_RESOLUTION_WIDTH;
   VP[1][1] = config::HALF_RESOLUTION_HEIGHT;
   VP[2][2] = 0.5;
   VP[3] = glm::fvec4(
         config::HALF_RESOLUTION_WIDTH,
         config::HALF_RESOLUTION_HEIGHT,
         0.5,
         0.0
   );

   return VP;
}

void vertexProcessing::makePerspectiveDivision(glm::fvec4& v)
{
   v = glm::fvec4(
         v.x / glm::max(0.0001f, v.w),
         v.y / glm::max(0.0001f, v.w),
         v.z / glm::max(0.0001f, v.w),
         1.0
   );
}


// We will cut everything that is behind the zNear and everything that
// is in front of zFar.
bool vertexProcessing::isInClipSpace(const glm::fvec4& clipCoords)
{
   if (std::fabs(clipCoords.x) > 1.0 ||
       std::fabs(clipCoords.y) > 1.0 ||
       std::fabs(clipCoords.z) > 1.0)
      return false;
   return true;
}
