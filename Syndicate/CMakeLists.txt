
set(syndicateSOURCE sensorStack.cpp sensor.cpp camera.cpp)

set(syndicateHEADER sensorStack.h sensor.h camera.h)

add_library(syndicate ${syndicateSOURCE} ${syndicateHEADER})

target_include_directories(syndicate
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

add_subdirectory(SpinnakerCamera)
add_subdirectory(SimpleSensor)
add_subdirectory(VimbaCamera)
add_subdirectory(OpenCVCamera)

target_link_libraries (syndicate SimpleSensor SpinnakerCamera VimbaCamera OpenCVCamera)

