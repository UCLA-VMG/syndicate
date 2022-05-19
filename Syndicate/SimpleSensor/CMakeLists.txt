
set(SimpleSOURCE 
        simpleSensor.cpp)

set(SimpleHEADER
        simpleSensor.h)

add_library(SimpleSensor ${SimpleSOURCE} ${SimpleHEADER})

target_include_directories(SimpleSensor
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(SimpleSensor syndicate)