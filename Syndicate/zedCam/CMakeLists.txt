
set(zedCamSOURCE 
        zedCam.cpp)

set(zedCamHEADER
        zedCam.h)

add_library(zedCam ${zedCamSOURCE} ${zedCamHEADER})

target_include_directories(zedCam
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(zedCam syndicate)
