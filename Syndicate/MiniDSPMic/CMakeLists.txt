
set(MiniDSPMicSOURCE 
        MiniDSPMic.cpp)

set(MiniDSPMicHEADER
        MiniDSPMic.h)

add_library(MiniDSPMic ${MiniDSPMicSOURCE} ${MiniDSPMicHEADER})

target_include_directories(MiniDSPMic
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

target_link_libraries(MiniDSPMic syndicate)
find_package(portaudio CONFIG REQUIRED)
target_link_libraries(MiniDSPMic PRIVATE portaudio)