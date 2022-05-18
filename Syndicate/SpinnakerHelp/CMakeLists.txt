
set(SpinnakerSOURCE 
        SpinnakerInfo.cpp)

set(SpinnakerHEADER
        SpinnakerInfo.h)

add_library(SpinnakerHelper ${SpinnakerSOURCE} ${SpinnakerHEADER})

target_include_directories(SpinnakerHelper
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )