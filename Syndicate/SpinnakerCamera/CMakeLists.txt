
set(SpinnakerSOURCE 
        SpinnakerInfo.cpp SpinnakerCamera.cpp)

set(SpinnakerHEADER
        SpinnakerInfo.h SpinnakerCamera.h)

add_library(SpinnakerCamera ${SpinnakerSOURCE} ${SpinnakerHEADER})

target_include_directories(SpinnakerCamera
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(SpinnakerCamera syndicate)
