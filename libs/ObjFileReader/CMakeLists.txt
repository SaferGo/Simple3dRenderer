set(LIBRARY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LIBRARY_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(ObjFileReader STATIC
   "${LIBRARY_SRC_DIR}/Model.cpp"
)

target_include_directories(ObjFileReader PUBLIC ${LIBRARY_INCLUDE_DIR})

# Connect with GLM
target_link_libraries(ObjFileReader PUBLIC
   glm
)
