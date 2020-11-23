add_library(stb_lib STATIC
  stb_image.cpp)

target_include_directories(stb_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
