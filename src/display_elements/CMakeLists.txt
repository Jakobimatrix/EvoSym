add_library(pre_display_lib INTERFACE)

target_link_libraries(pre_display_lib INTERFACE
  Qt5::Widgets
  warning_guards
  glm
  glad
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  globals_lib)

target_include_directories(pre_display_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")



add_library(display_elements_lib STATIC
  src/display_elements/mesh.cpp
  src/display_elements/worldMesh.cpp)

target_link_libraries(display_elements_lib
  warning_guards
  pre_display_lib
  glad
  stb_lib
  globals_lib)

target_include_directories(display_elements_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
