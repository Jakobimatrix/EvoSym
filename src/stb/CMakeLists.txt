add_library(stb_lib STATIC
  stb_image.cpp)

set_property(TARGET stb_lib PROPERTY POSITION_INDEPENDENT_CODE ON)


target_link_libraries(stb_lib
  ${OPENSSL_LIBRARIES} dl
  warning_guards)

target_include_directories(stb_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
