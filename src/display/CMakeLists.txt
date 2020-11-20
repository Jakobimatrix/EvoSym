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

add_library(display_lib STATIC
  src/display/sfmlRenderWindow.cpp
  src/display/qSfmlCanvas.cpp
  src/display/display.cpp
  src/display/displayQt.cpp)

target_link_libraries(display_lib
  warning_guards
  pre_display_lib
  glad
  Qt5::Widgets
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  random_generator
  settings_lib
  globals_lib
  homography_lib
  world_lib
  timer_lib)

# define the target links: specify how the libs shall be included.
target_include_directories(display_lib PUBLIC STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

#target_include_directories(display_lib PUBLIC STATIC
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
#  $<INSTALL_INTERFACE:src>)
