
set(syndicateSOURCE sensorStack.cpp sensor.cpp)

set(syndicateHEADER sensorStack.h sensor.h)

add_library(syndicate ${syndicateSOURCE} ${syndicateHEADER})

target_include_directories(syndicate
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

add_subdirectory(SpinnakerHelp)
add_subdirectory(SimpleSensor)

target_link_libraries (syndicate SimpleSensor SpinnakerHelper)

