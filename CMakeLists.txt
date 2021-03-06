cmake_minimum_required(VERSION 3.10.2)
project(evosym VERSION 1.0.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

# needed by glad
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl")

message("-----------------------------")
message(${CMAKE_CXX_COMPILER_ID})
message("-----------------------------")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Werror=suggest-override")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror=header-hygiene -Wno-c++98-compat -Wno-shadow -Wno-c++98-compat-pedantic -Wno-date-time -Wno-missing-prototypes -Wno-exit-time-destructors -Wno-global-constructors -Wno-conversion -Wno-sign-conversion -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-abstract-vbase-init -Wno-potentially-evaluated-expression -Wno-documentation-unknown-command")
endif()



if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

message("############### BUILD TYPE: ${CMAKE_BUILD_TYPE} ###############")

# require third party libs
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

find_package(Boost REQUIRED COMPONENTS system)
include_directories( ${Boost_INCLUDE_DIRS} )

cmake_minimum_required(VERSION 2.8.11)

project(testproject)

# Find includes in corresponding build directories
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
SET(QMAKE_CXXFLAGS "${QMAKE_CXXFLAGS} -std=c++17")
SET(QMAKE_LFLAGS "${QMAKE_LFLAGS} -lstdc++fs")
SET(CMAKE_CXX_STANDARD 17) # otherwise compiler will not be c++17
# needed? its in the tutorial qt5 with CMake
## We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
#add_definitions(${Qt5Widgets_DEFINITIONS})
## Executables fail to build with Qt 5 in the default configuration
## without -fPIE. We add that here.
#SET(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")


find_package(OpenGL REQUIRED)

#thirdparty
add_subdirectory(src/glm)
add_subdirectory(src/stb)
add_subdirectory(src/warning_guards)

#submodules (my code)
add_subdirectory(src/randomGenerator)
add_subdirectory(src/Settings)
add_subdirectory(src/homography)
add_subdirectory(src/timer)

#code
add_subdirectory(src/globals)
add_subdirectory(src/utils)
add_subdirectory(src/display_elements)
add_subdirectory(src/display)
add_subdirectory(src/world)

#executable
add_subdirectory(src/executable)
