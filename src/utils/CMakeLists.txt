find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_library(utils_lib INTERFACE)

target_link_libraries(utils_lib INTERFACE
    timer_lib
    Eigen3::Eigen)

target_include_directories(utils_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(utils_func_lib
  src/utils/functions.cpp)

target_link_libraries(utils_func_lib
  stb_lib
  warning_guards)

# define the target links: specify how the libs shall be included.
target_include_directories(utils_func_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
