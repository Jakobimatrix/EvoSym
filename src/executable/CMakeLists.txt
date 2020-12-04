add_executable(evosym_start src/main.cpp)

list(APPEND evosym_start_SOURCES
  PRIVATE globals_lib
  PRIVATE display_lib
  Qt5::Widgets
)

target_link_libraries(evosym_start ${evosym_start_SOURCES} ${LIBS})
