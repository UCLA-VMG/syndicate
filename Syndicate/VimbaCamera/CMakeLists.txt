
set(VimbaSOURCE 
VimbaCamera.cpp CameraObserver.cpp FrameObserver.cpp Bitmap.cpp CameraObserver.cpp)

set(VimbaHEADER
VimbaCamera.h CameraObserver.h FrameObserver.h Bitmap.h CameraObserver.h ErrorCodeToMessage.h)

add_library(VimbaCamera ${VimbaSOURCE} ${VimbaHEADER})

target_include_directories(VimbaCamera
PUBLIC
${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(VimbaCamera syndicate)
