
set(SpinnakerSOURCE 
        SpinnakerInfo.cpp)

set(SpinnakerHEADER
        SpinnakerInfo.h)

add_library(Spintest ${SpinnakerSOURCE} ${SpinnakerHEADER})
# target_include_directories(SpinnakerLIB PUBLIC .)
# target_link_libraries(SpinnakerLIB PUBLIC ${Spinnaker_LIBRARIES})