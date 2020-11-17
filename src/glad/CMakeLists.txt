add_library(glad_lib_pre INTERFACE)
target_link_libraries(glad_lib_pre INTERFACE)

target_include_directories(glad_lib_pre INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")




add_library(glad_lib
  src/glad.c)

target_link_libraries(glad_lib
    glad_lib_pre)

# define the target links: specify how the libs shall be included.
target_include_directories(glad_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
