# ################################################################################################################# WSL

# cmake_minimum_required(VERSION 3.0.0)
# project(syndicate VERSION 0.1.0)

# include(CTest)
# enable_testing()

# # Setting the variables
# set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# message(STATUS "${CMAKE_MODULE_PATH}")
# # find_package(Spinnaker REQUIRED)

# #Include Path Libraries
# include_directories("${PROJECT_SOURCE_DIR}/include")
# # include_directories("C:/Program Files/FLIR Systems/Spinnaker/include")
# include_directories("/mnt/c/Program Files/FLIR Systems/Spinnaker")
# include_directories("/mnt/c/Program Files/FLIR Systems/Spinnaker/include")
# # set(CMAKE_PREFIX_PATH "C:/Program Files/FLIR Systems/Spinnaker")
# # set(SPINNAKER_DIR "C:/Program Files/FLIR Systems/Spinnaker")
# # find_package(Spinnaker REQUIRED)

# #Include Libraries
# #add_subdirectory(src)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

# add_library(Spinnaker SHARED IMPORTED GLOBAL)
# set_property(TARGET Spinnaker PROPERTY IMPORTED_IMPLIB_RELEASE "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib64/vs2015/Spinnaker_v140.lib")
# set_property(TARGET Spinnaker PROPERTY IMPORTED_LOCATION_RELEASE "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64/vs2015/Spinnaker_v140.dll")
# set_property(TARGET Spinnaker PROPERTY IMPORTED_IMPLIB_DEBUG "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib64/vs2015/Spinnaker_v140.lib")
# set_property(TARGET Spinnaker PROPERTY IMPORTED_LOCATION_DEBUG "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64/vs2015/Spinnaker_v140.dll")
# add_executable(helloworld ${PROJECT_SOURCE_DIR}/src/main.cpp)
# target_link_libraries(helloworld Spinnaker)

################################################################################################################# Windows

# cmake_minimum_required(VERSION 3.0.0)
# project(syndicate VERSION 0.1.0)

# include(CTest)
# enable_testing()

# # Setting the variables
# set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# message(STATUS "${CMAKE_MODULE_PATH}")
# # find_package(Spinnaker REQUIRED)

# #Include Path Libraries
# include_directories("${PROJECT_SOURCE_DIR}/include")
# # include_directories("C:/Program Files/FLIR Systems/Spinnaker/include")
# include_directories("C:/Program Files/FLIR Systems/Spinnaker")
# include_directories("C:/Program Files/FLIR Systems/Spinnaker/include")
# # set(CMAKE_PREFIX_PATH "C:/Program Files/FLIR Systems/Spinnaker")
# # set(SPINNAKER_DIR "C:/Program Files/FLIR Systems/Spinnaker")
# # find_package(Spinnaker REQUIRED)

# #Include Libraries
# #add_subdirectory(src)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

# add_library(Spinnaker SHARED IMPORTED GLOBAL)
# set_property(TARGET Spinnaker PROPERTY IMPORTED_IMPLIB_RELEASE "C:/Program Files/FLIR Systems/Spinnaker/lib64/vs2015/Spinnaker_v140.lib")
# set_property(TARGET Spinnaker PROPERTY IMPORTED_LOCATION_RELEASE "C:/Program Files/FLIR Systems/Spinnaker/bin64/vs2015/Spinnaker_v140.dll")
# set_property(TARGET Spinnaker PROPERTY IMPORTED_IMPLIB_DEBUG "C:/Program Files/FLIR Systems/Spinnaker/lib64/vs2015/Spinnaker_v140.lib")
# set_property(TARGET Spinnaker PROPERTY IMPORTED_LOCATION_DEBUG "C:/Program Files/FLIR Systems/Spinnaker/bin64/vs2015/Spinnaker_v140.dll")
# add_executable(helloworld ${PROJECT_SOURCE_DIR}/src/main.cpp)
# target_link_libraries(helloworld Spinnaker)



#################################################################################################################

# cmake_minimum_required(VERSION 3.0.0)
# project(syndicate VERSION 0.1.0)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# find_package(Spinnaker REQUIRED)
# # find_package(OpenImageIO CONFIG REQUIRED)

# include_directories(  
#     SYSTEM ${Spinnaker_INCLUDE_DIRS}
# )
# link_directories( ${Spinnaker_LIBRARIES} )
# set (LIBS ${Spinnaker_LIBRARIES})

# add_executable(nir ${PROJECT_SOURCE_DIR}/src/nir.cpp)
# # target_link_libraries(nir PRIVATE ${LIBS} OpenImageIO::OpenImageIO OpenImageIO::OpenImageIO_Util)
# target_link_libraries(nir ${LIBS})


#################################################################################################################

# cmake_minimum_required(VERSION 3.0.0)
# project(syndicate VERSION 0.1.0)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# find_package(Spinnaker REQUIRED)
# find_package(SpinVideo REQUIRED)
# # find_package(OpenImageIO CONFIG REQUIRED)

# include_directories(  
#     SYSTEM ${Spinnaker_INCLUDE_DIRS}
#     SYSTEM ${SpinVideo_INCLUDE_DIRS}
# )
# link_directories( ${Spinnaker_LIBRARIES} ${SpinVideo_LIBRARIES})
# set (LIBS ${Spinnaker_LIBRARIES} ${SpinVideo_LIBRARIES})

# #Generate Execs
# add_executable(nir ${PROJECT_SOURCE_DIR}/src/nir.cpp)
# add_executable(nir_continuous ${PROJECT_SOURCE_DIR}/src/nir_continuous.cpp)
# target_link_libraries(nir ${LIBS})
# target_link_libraries(nir_continuous ${LIBS})

# #Targets
# ADD_CUSTOM_TARGET(build_all)
# ADD_DEPENDENCIES(build_all nir nir_continuous)

#################################################################################################################

cmake_minimum_required(VERSION 3.0.0)
project(syndicate VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Spinnaker REQUIRED)
find_package(SpinVideo REQUIRED)
find_package(Vimba REQUIRED)
# find_package(OpenImageIO CONFIG REQUIRED)

include_directories(  
    SYSTEM ${Spinnaker_INCLUDE_DIRS}
    SYSTEM ${SpinVideo_INCLUDE_DIRS}
    SYSTEM ${Vimba_INCLUDE_DIRS}
)
link_directories( ${Spinnaker_LIBRARIES} ${SpinVideo_LIBRARIES})
set (LIBS ${Spinnaker_LIBRARIES} ${SpinVideo_LIBRARIES})

#Generate Execs
add_executable(rgb ${PROJECT_SOURCE_DIR}/src/preset/rgb.cpp)
add_executable(nir ${PROJECT_SOURCE_DIR}/src/preset/nir.cpp)
add_executable(polarized ${PROJECT_SOURCE_DIR}/src/preset/polarized.cpp)
add_executable(rgb_continuous ${PROJECT_SOURCE_DIR}/src/continuous/rgb_continuous.cpp)
add_executable(nir_continuous ${PROJECT_SOURCE_DIR}/src/continuous/nir_continuous.cpp)
add_executable(polarized_continuous ${PROJECT_SOURCE_DIR}/src/continuous/polarized_continuous.cpp)
target_link_libraries(rgb ${LIBS})
target_link_libraries(nir ${LIBS})
target_link_libraries(polarized ${LIBS})
target_link_libraries(rgb_continuous ${LIBS})
target_link_libraries(nir_continuous ${LIBS})
target_link_libraries(polarized_continuous ${LIBS})

#Targets
ADD_CUSTOM_TARGET(build_all)
ADD_DEPENDENCIES(build_all rgb nir polarized rgb_continuous nir_continuous polarized_continuous)
