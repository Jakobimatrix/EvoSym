find_package(Eigen3 3.3 REQUIRED NO_MODULE)

find_package(Boost REQUIRED COMPONENTS system)
include_directories( ${Boost_INCLUDE_DIRS} )

# "Detect" and add SFML
## sudo find . -name "FindSFML.cmake" -type f
set(CMAKE_MODULE_PATH "/usr/share/SFML/cmake/Modules")
#Find any version 2.X of SFML
# IF THIS FAILS, YOU HAVE A DIFFERENT <CMAKE_MODULE_PATH>. FIND FindSFML.cmake AND CHANGE THE VARIABLE ABOVE.
find_package(SFML 2 COMPONENTS system window graphics network audio)

include_directories(${SFML_INCLUDE_DIR})

add_executable(evosym_start src/main.cpp)

list(APPEND evosym_start_SOURCES
  Eigen3::Eigen
  ${Boost_LIBRARIES}
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  settings_lib
  timer_lib
  homography_lib
  random_generator
)
target_link_libraries(evosym_start ${evosym_start_SOURCES})
