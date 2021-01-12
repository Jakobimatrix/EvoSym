add_library(display_lib STATIC
  src/display/display.cpp
  src/display/displayQt.cpp
  src/display/qSfmlCanvas.cpp
  src/display/sfmlRenderWindow.cpp)

target_link_libraries(display_lib
  warning_guards
  display_elements_lib
  Qt5::Widgets
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  random_generator
  settings_lib
  globals_lib
  world_lib
  timer_lib)

target_include_directories(display_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

