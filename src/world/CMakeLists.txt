find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Define the name of the base library and all source files belonging to it
add_library(world_lib
  src/world/layer.cpp)

target_link_libraries(world_lib
  random_generator
  homography_lib
  settings_lib)

# define the target links: specify how the libs shall be included.
target_include_directories(world_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
