set(LIBRARY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LIBRARY_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(tga STATIC
   "${LIBRARY_SRC_DIR}/TGAImage.cpp"
)

target_include_directories(tga PUBLIC ${LIBRARY_INCLUDE_DIR})
