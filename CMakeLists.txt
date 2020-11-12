
cmake_minimum_required(VERSION 3.10.2)
project(evosym VERSION 1.0.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -lstdc++fs")

# require third party libs
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

cmake_minimum_required(VERSION 2.8.11)

project(testproject)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
#set(QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Gui)
find_package(Qt5Widgets)
SET(QMAKE_CXXFLAGS "${QMAKE_CXXFLAGS} -std=c++17")
SET(QMAKE_LFLAGS "${QMAKE_LFLAGS} -lstdc++fs")
SET(CMAKE_CXX_STANDARD 17)


#submodules
add_subdirectory(src/randomGenerator)
add_subdirectory(src/Settings)
add_subdirectory(src/homography)
add_subdirectory(src/timer)

#code
add_subdirectory(src/display)
add_subdirectory(src/globals)
#add_subdirectory(src/utils)
add_subdirectory(src/world)

#executable
add_subdirectory(src/executable)
