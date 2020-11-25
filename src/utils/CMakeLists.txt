find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_library(utils_lib INTERFACE)

target_link_libraries(utils_lib INTERFACE
    timer_lib
    Eigen3::Eigen)

target_include_directories(utils_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Define the name of the base library and all source files belonging to it
#add_library(utils_lib
#  src/utils/xxxx.cpp)

#target_link_libraries(utils_lib)

# define the target links: specify how the libs shall be included.
#target_include_directories(utils_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
