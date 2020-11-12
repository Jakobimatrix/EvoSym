add_executable(evosym_start src/main.cpp)

list(APPEND evosym_start_SOURCES
  PRIVATE globals_lib
  PRIVATE display_lib
  PRIVATE world_lib
  PRIVATE settings_lib
  PRIVATE timer_lib
  PRIVATE homography_lib
  PRIVATE random_generator
  Eigen3::Eigen
  ${Boost_LIBRARIES}
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES}
  Qt5::Widgets
)
target_link_libraries(evosym_start ${evosym_start_SOURCES} ${LIBS})
