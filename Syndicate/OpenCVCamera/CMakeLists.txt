set(OpenCVCameraSOURCE 
        OpenCVCamera.cpp)

set(OpenCVCameraHEADER
        OpenCVCamera.h)

add_library(OpenCVCamera ${OpenCVCameraSOURCE} ${OpenCVCameraHEADER})

target_include_directories(OpenCVCamera
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(OpenCVCamera syndicate)
