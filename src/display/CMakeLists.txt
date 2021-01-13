add_library(display_lib STATIC
  src/display/display.cpp
  src/display/displayQt.cpp
  src/display/openGLWidget.cpp
  src/display/renderWindow.cpp)

target_link_libraries(display_lib
  warning_guards
  Qt5::Widgets
  ${OPENGL_LIBRARIES}
  display_elements_lib
  random_generator
  settings_lib
  globals_lib
  world_lib
  timer_lib)

target_include_directories(display_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
