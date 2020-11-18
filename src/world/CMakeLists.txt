# Define the name of the base library and all source files belonging to it
add_library(world_lib
  src/world/layer.cpp
  src/world/world.cpp)

target_link_libraries(world_lib
  random_generator
  homography_lib
  settings_lib
  pre_display_lib
  globals_lib
  Eigen3::Eigen)

# define the target links: specify how the libs shall be included.
target_include_directories(world_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
