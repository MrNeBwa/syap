cmake_minimum_required(VERSION 3.21)

# clangd (and conform.nvim, nvim-lint) read this to know real flags/includes.
# Must be set before project(): CMake captures it during language enablement,
# so a plain set() further down this file is silently ignored.
# Only meaningful for single-config generators (Ninja/Makefiles), not MSVC.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(MyCpp20Project
  VERSION 1.0.0
  DESCRIPTION "A small C++20 project"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to an optimized build with debug symbols for single-config generators.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel)
endif()

# List sources explicitly: file(GLOB) does not re-run configure when files change.
# Headers are listed so IDEs and build tools show them as part of the target;
# they are not compiled on their own.
add_executable(MyProgram
  src/main.cpp
  src/user.cpp
  src/regUser.cpp
  include/user.h
  include/regUser.h
  )

target_include_directories(MyProgram PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(MSVC)
  target_compile_options(MyProgram PRIVATE /W4 /permissive-)
else()
  target_compile_options(MyProgram PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS MyProgram RUNTIME DESTINATION bin)

# clangd looks for compile_commands.json at the project root, but CMake writes
# it into the build dir. Bridge the two with a symlink.
file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json"
     "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC)
