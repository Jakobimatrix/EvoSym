add_library(pre_display_lib INTERFACE)
target_link_libraries(pre_display_lib INTERFACE
  Qt5::Widgets
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES})

target_include_directories(pre_display_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")


add_library(display_lib
  src/display/display.cpp
  src/display/shape.cpp
  src/display/menu.cpp)

target_link_libraries(display_lib
  pre_display_lib
  Qt5::Widgets
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  random_generator
  settings_lib
  globals_lib
  homography_lib)

# define the target links: specify how the libs shall be included.
target_include_directories(display_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
