
set(MX800SOURCE 
        MX800.cpp)

set(MX800HEADER
        MX800.h)

add_library(MX800 ${MX800SOURCE} ${MX800HEADER})

target_include_directories(MX800
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

target_link_libraries(MX800 PUBLIC syndicate)
