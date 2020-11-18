# suggested here: https://github.com/Dav1dde/glad/issues/186

cmake_minimum_required(VERSION 3.0)
project(Glad)

add_library(glad STATIC
  include/glad/glad.h
  src/glad.c)

# requires linking against libdl
# https://stackoverflow.com/questions/48136263/makefile-for-opengl-with-glad-on-linux
# https://stackoverflow.com/questions/58740456/how-to-link-with-libdl-library-in-cmake
target_link_libraries(glad
  ${OPENSSL_LIBRARIES} dl)

# using SYSTEM to silence compiler warnings
target_include_directories(glad PUBLIC STATIC SYSTEM include/)

