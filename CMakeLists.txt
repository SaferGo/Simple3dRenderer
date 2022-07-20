cmake_minimum_required(VERSION 3.9.1)

project(Simple3DRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")
set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(PROJECT_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/libs")

# LIBS

# -GLM
set(GLM_DIR "${PROJECT_LIBRARIES_DIR}/glm")
add_subdirectory(${GLM_DIR} EXCLUDE_FROM_ALL)

# -TGA
set(TGA_DIR "${PROJECT_LIBRARIES_DIR}/tga")
add_subdirectory(${TGA_DIR})

# -ModelReader
set(OBJ_FILE_READER_DIR "${PROJECT_LIBRARIES_DIR}/ObjFileReader")
add_subdirectory(${OBJ_FILE_READER_DIR})

# Executable
add_executable(${PROJECT_NAME} 
   "${PROJECT_SOURCE_DIR}/Simple-3D-Renderer/main.cpp"
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
   ObjFileReader
)
