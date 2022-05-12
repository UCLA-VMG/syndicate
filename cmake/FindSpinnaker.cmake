include(FindPackageHandleStandardArgs)

set(SPINNAKER_DIR "C:/Program\ Files/FLIR\ Systems/Spinnaker")

find_path(SPINNAKER_INCLUDE_DIR
            NAMES Spinnaker.h
            PATHS
            ${SPINNAKER_DIR}/include)

    # if (CMAKE_GENERATOR MATCHES "(Win64|IA64)")
    #     set(LIBDIR_POSTFIX "64")
    # endif()

find_library(SPINNAKER_LIBRARY
                NAMES Spinnakerd_v140
                PATHS
                ${SPINNAKER_DIR}/lib/vs2015)

find_library(SPINNAKER_LIBRARYD
                NAMES Spinnakerd_v140.dll
                PATHS
                ${SPINNAKER_DIR}/bin/vs2015)
message(STATUS "hi")
message(STATUS "${SPINNAKER_LIBRARYD}")


find_package_handle_standard_args(
    Spinnaker
    DEFAULT_MSG
    SPINNAKER_INCLUDE_DIR
    SPINNAKER_LIBRARY
)

if (SPINNAKER_FOUND)
    message(STATUS "Spinnaker include: ${SPINNAKER_INCLUDE_DIR}")
    message(STATUS "Spinnaker library: ${SPINNAKER_LIBRARY}")
    set(SPINNAKER_INCLUDE_DIRS "${SPINNAKER_INCLUDE_DIR}")
    set(SPINNAKER_LIBRARIES "${SPINNAKER_LIBRARY}")# "${SPINNAKER_LIBRARYD}")
endif()
