set(RealSenseCameraSOURCE 
        RealSenseCamera.cpp)

set(RealSenseCameraHEADER
        RealSenseCamera.h)

add_library(RealSenseCamera ${RealSenseCameraSOURCE} ${RealSenseCameraHEADER})

target_include_directories(RealSenseCamera
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(RealSenseCamera syndicate)
