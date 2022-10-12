#DEV Notes:
#cmake --build ./ --target build_all --config Debug
#cmake --build ./ --target {custom .exe} --config Debug

cmake_minimum_required(VERSION 3.0.0)
project(syndicate VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 23)
include(CTest)
enable_testing()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)")
    set (X86 TRUE)
    set (X64 FALSE)
    message(STATUS "HOST Processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}. Switching to X86 compilation.")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "(x64)|(X64)|(amd64)|(AMD64)")
    set (X64 TRUE)
    set (X86 FALSE)
    message(STATUS "HOST Processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}. Switching to X64 compilation.")
else()
    message(SEND_ERROR "HOST Processor, ${CMAKE_HOST_SYSTEM_PROCESSOR}, not compatible." )
endif ()

#Set Useful Paths
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#Find packeges
message(STATUS "${CMAKE_MODULE_PATH}")
find_package(Spinnaker REQUIRED)
find_package(VimbaCPP REQUIRED)
find_package(VimbaC REQUIRED)
find_package(VimbaImageTransform REQUIRED)
find_package(Npcap REQUIRED)
# set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/build)
# message(STATUS "${OpenCV_DIR}")
find_package(OpenCV REQUIRED)
# find_library(WINDOWS_SDK Strmiids.lib) #required for Dshow functionality
# find_package(realsense2 REQUIRED)

# set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
# set(BOOST_ROOT "C:/Program Files/boost/boost_1_78_0")
find_package(Boost COMPONENTS thread filesystem REQUIRED)
find_package(realsense2 CONFIG REQUIRED)

#Include Path Libraries
include_directories("${PROJECT_SOURCE_DIR}/include"
                    "${PROJECT_SOURCE_DIR}/src"
                    SYSTEM ${Spinnaker_INCLUDE_DIRS}
                    SYSTEM ${VimbaCPP_INCLUDE_DIRS}
                    SYSTEM ${VimbaC_INCLUDE_DIRS}
                    SYSTEM ${VimbaImageTransform_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIR}
                    ${Npcap_INCLUDE_DIRS}
                    ${OpenCV_INCLUDE_DIRS}
                    ${realsense2_INCLUDE_DIRS}
                    )

#Include Libraries
add_subdirectory(Syndicate)
set (LIBS ${Spinnaker_LIBRARIES} ${VimbaCPP_LIBRARIES} ${VimbaC_LIBRARIES} ${VimbaImageTransform_LIBRARIES} ${Boost_LIBRARIES} ${Npcap_LIBRARIES} ${OpenCV_LIBRARIES} ${WINDOWS_SDK} realsense2::realsense2 syndicate )

#Generate Execs
# add_executable(capture ${PROJECT_SOURCE_DIR}/scripts/main.cpp)
add_executable(capture_config ${PROJECT_SOURCE_DIR}/scripts/main.cpp)
# add_executable(config_test  ${PROJECT_SOURCE_DIR}/scripts/test_config.cpp)
# add_executable(dshow_test ${PROJECT_SOURCE_DIR}/scripts/dshow_test.cpp)
# add_executable(acq ${PROJECT_SOURCE_DIR}/scripts/acq.cpp)
# add_executable(packets_acq ${PROJECT_SOURCE_DIR}/scripts/packets_acq.cpp)
# target_link_libraries(capture ${LIBS})
# target_link_libraries(config_test ${LIBS})
target_link_libraries(capture_config ${LIBS})
# target_link_libraries(dshow_test ${LIBS})
# target_link_libraries(acq ${LIBS})
# target_link_libraries(packets_acq ${LIBS})

#Targets
ADD_CUSTOM_TARGET(build_all)
ADD_DEPENDENCIES(build_all capture_config)

#Library Linking
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
