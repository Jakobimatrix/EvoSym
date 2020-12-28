add_library(pre_display_lib INTERFACE)

target_link_libraries(pre_display_lib INTERFACE
  Qt5::Widgets
  warning_guards
  glm
  glad
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  globals_lib
  settings_lib
  utils_lib
  stb_lib
  Eigen3::Eigen)

target_include_directories(pre_display_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")



add_library(display_elements_lib STATIC
  src/display_elements/worldMesh.cpp
  src/display_elements/arrowMesh.cpp)

target_link_libraries(display_elements_lib
  pre_display_lib
  glad
  globals_lib
  utils_lib
  Eigen3::Eigen)

target_include_directories(display_elements_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
