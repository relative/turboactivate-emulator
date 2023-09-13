cmake_minimum_required(VERSION 3.23)

project(turboactivate-emulator)

add_library(turboactivate-emulator SHARED
  src/main.cpp src/functions.cpp src/functions.h src/inih.h src/TurboActivate.h
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(turboactivate-emulator PRIVATE
    -Wno-microsoft-cast
  )
endif()

set_target_properties(turboactivate-emulator PROPERTIES
  CXX_STANDARD 20
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:DEBUG>:Debug>"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)