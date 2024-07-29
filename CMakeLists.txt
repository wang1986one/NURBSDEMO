cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(NURBS VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/Extern)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_definitions(-DIGL_STATIC_LIBRARY)

if(NOT DEFINED EIGEN3_INCLUDE_DIR)
  set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Extern/eigen-3.4.0")
endif()
find_package(Eigen3 3.4.0 REQUIRED)

add_subdirectory(Source)
add_subdirectory(Extern)
