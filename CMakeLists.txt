cmake_minimum_required(VERSION 3.0.0)
project(capture VERSION 0.1.0)

include(CTest)
enable_testing()

#Set Useful Paths
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#Include Path Libraries
include_directories("${PROJECT_SOURCE_DIR}/include"
                    )

#Include Libraries
#add_subdirectory(src)

#Generate Execs
add_executable(capture ${PROJECT_SOURCE_DIR}/scripts/main.cpp)

#Library Linking
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
