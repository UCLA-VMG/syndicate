
set(RespBeltSOURCE 
        RespBelt.cpp)

set(RespBeltHEADER
        RespBelt.h)

add_library(RespBelt ${RespBeltSOURCE} ${RespBeltHEADER})

target_include_directories(RespBelt
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

target_link_libraries(RespBelt PUBLIC syndicate)
