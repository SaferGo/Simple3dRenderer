#pragma once

#include <Simple-3D-Renderer/Camera/Camera.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>

namespace vertexProcessing
{
   glm::mat4 getModelM();
   glm::mat4 getViewM(const Camera& camera);
   glm::mat4 getProjectionM(const Frustum& frustum);
   glm::mat4 getViewportM();

   void makePerspectiveDivision(glm::fvec4& v);
};
