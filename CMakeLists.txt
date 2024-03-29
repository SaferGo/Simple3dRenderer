cmake_minimum_required(VERSION 3.9.1)

project(Simple3DRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")
set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(PROJECT_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/libs")

#################################### LIBS #####################################

# -GLM
set(GLM_DIR "${PROJECT_LIBRARIES_DIR}/glm")
add_subdirectory(${GLM_DIR} EXCLUDE_FROM_ALL)

# -TGA
set(TGA_DIR "${PROJECT_LIBRARIES_DIR}/tga")
add_subdirectory(${TGA_DIR})

# -TinyObjLoader
set(TINY_OBJ_LOADER_DIR "${PROJECT_LIBRARIES_DIR}/tinyobjloader")
add_subdirectory(${TINY_OBJ_LOADER_DIR})

############################### Executable ####################################

add_executable(${PROJECT_NAME} 
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/main.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Math/mathUtils.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Camera/Camera.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Camera/Frustum.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Vertex/vertexProcessing.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Texture/textureProcessing.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Shaders/GouradShader.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Shaders/NormalMapping.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Renderer/Renderer.cpp"
   "${PROJECT_SOURCE_DIR}/Simple3dRenderer/Model/Model.cpp"
)

target_include_directories(
   ${PROJECT_NAME}
   PUBLIC
      "${PROJECT_INCLUDE_DIR}"
   PRIVATE
      "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
   glm
   tga
   tinyobjloader
)
