
set(syndicateSOURCE sensor.cpp)

set(syndicateHEADER sensor.h)

add_library(syndicate ${syndicateSOURCE} ${syndicateHEADER})

target_include_directories(syndicate
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

add_subdirectory(SpinnakerHelp)

target_link_libraries (syndicate SpinnakerHelper)

