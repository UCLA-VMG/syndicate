# ########################################################################################## WSL v1

# # Find the Spinnaker library

# unset(Spinnaker_FOUND)
# unset(Spinnaker_INCLUDE_DIRS)
# unset(Spinnaker_LIBRARIES)

# find_path(Spinnaker_INCLUDE_DIRS NAMES
#   "Spinnaker.h"
#   PATHS
#   "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/include"
#   )
# message(STATUS "hi anriudh")
# message(STATUS "${Spinnaker_INCLUDE_DIRS}")

# find_library(Spinnaker_LIBRARIES 
#   NAMES Spinnaker_v140
#   PATHS
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib64/vs2015"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib64"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib/vs2015"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib"
# )
# message(STATUS "hi adnan")
# message(STATUS "${Spinnaker_LIBRARIES}")

# if (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)
#   message(STATUS "Spinnaker found in the system")
#   set(Spinnaker_FOUND 1)
# endif (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)

########################################################################################## Windows v1

# # Find the Spinnaker library

# unset(Spinnaker_FOUND)
# unset(Spinnaker_INCLUDE_DIRS)
# unset(Spinnaker_LIBRARIES)

# find_path(Spinnaker_INCLUDE_DIRS NAMES
#   "Spinnaker.h"
#   PATHS
#   "C:/Program Files/FLIR Systems/Spinnaker/include"
#   )
# message(STATUS "hi anriudh")
# message(STATUS "${Spinnaker_INCLUDE_DIRS}")

# find_library(Spinnaker_LIBRARIES 
#   NAMES Spinnaker_v140
#   PATHS
#   "C:/Program Files/FLIR Systems/Spinnaker/lib64/vs2015"
#   "C:/Program Files/FLIR Systems/Spinnaker/lib64"
#   "C:/Program Files/FLIR Systems/Spinnaker/bin64"
#   "C:/Program Files/FLIR Systems/Spinnaker/lib/vs2015"
#   "C:/Program Files/FLIR Systems/Spinnaker/lib"
# )
# message(STATUS "hi adnan")
# message(STATUS "${Spinnaker_LIBRARIES}")

# if (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)
#   message(STATUS "Spinnaker found in the system")
#   set(Spinnaker_FOUND 1)
# endif (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)

# ########################################################################################## WSL v2

# # Find the Spinnaker library
# #
# #  Spinnaker_FOUND        - True if Spinnaker was found.
# #  Spinnaker_LIBRARIES    - The libraries needed to use Spinnaker
# #  Spinnaker_INCLUDE_DIRS - Location of Spinnaker.h

# unset(Spinnaker_FOUND)
# unset(Spinnaker_INCLUDE_DIRS)
# unset(Spinnaker_LIBRARIES)

# find_path(Spinnaker_INCLUDE_DIRS NAMES
#   "Spinnaker.h"
#   PATHS
#   "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/include"
# #   "C:/Program Files/FLIR Systems/Spinnaker/include"
#   )
# message(STATUS "hi anriudh")
# message(STATUS "${Spinnaker_INCLUDE_DIRS}")

# # find_library(Spinnaker_LIBRARIES 
# #   NAMES Spinnaker_v140.dll
# #   PATHS
# #   "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
# #   "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64/vs2015"
# # )

# find_library(Spinnaker_LIBRARIES NAMES Spinnaker_v140.lib
#                 PATHS
#                 "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/lib/vs2015"
#                 "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/lib64/vs2015")

# message(STATUS "hi adnan")
# message(STATUS "${Spinnaker_LIBRARIES}")

# find_library(SPINNAKER_LIBRARYD
#                 NAMES Spinnakerd_v140.dll
#                 PATHS
#                 "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
#                 "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64/vs2015")

# message(STATUS "hi adnan")
# message(STATUS "${SPINNAKER_LIBRARYD}")

# if (SPINNAKER_FOUND)
#     message(STATUS "Spinnaker include: ${SPINNAKER_INCLUDE_DIR}")
#     message(STATUS "Spinnaker library: ${SPINNAKER_LIBRARY}")
#     set(SPINNAKER_INCLUDE_DIRS "${SPINNAKER_INCLUDE_DIR}")
#     set(SPINNAKER_LIBRARIES "${SPINNAKER_LIBRARY}")# "${SPINNAKER_LIBRARYD}")
# endif()

# if (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)
#   message(STATUS "Spinnaker found in the system")
#   set(Spinnaker_FOUND 1)
# endif (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)

# ########################################################################################## Windows v2

# Find the Spinnaker library
#
#  Spinnaker_FOUND        - True if Spinnaker was found.
#  Spinnaker_LIBRARIES    - The libraries needed to use Spinnaker
#  Spinnaker_INCLUDE_DIRS - Location of Spinnaker.h

unset(Spinnaker_FOUND)
unset(Spinnaker_INCLUDE_DIRS)
unset(Spinnaker_LIBRARIES)

find_path(Spinnaker_INCLUDE_DIRS NAMES
  "Spinnaker.h"
  PATHS
  "C:/Program\ Files/FLIR\ Systems/Spinnaker/include"
#   "C:/Program Files/FLIR Systems/Spinnaker/include"
  )
message(STATUS "find_path result:")
message(STATUS "${Spinnaker_INCLUDE_DIRS}")

# find_library(Spinnaker_LIBRARIES 
#   NAMES Spinnaker_v140.dll
#   PATHS
#   "C:/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
#   "C:/Program Files/FLIR Systems/Spinnaker/bin64/vs2015"
# )

find_library(Spinnaker_LIBRARIES NAMES Spinnaker_v140.lib
                PATHS
                "C:/Program\ Files/FLIR\ Systems/Spinnaker/lib/vs2015"
                "C:/Program\ Files/FLIR\ Systems/Spinnaker/lib64/vs2015")

message(STATUS "find_library result:")
message(STATUS "${Spinnaker_LIBRARIES}")

# find_library(SPINNAKER_LIBRARYD
#                 NAMES Spinnakerd_v140.dll
#                 PATHS
#                 "C:/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
#                 "C:/Program Files/FLIR Systems/Spinnaker/bin64/vs2015")

# message(STATUS "hi adnan")
# message(STATUS "${SPINNAKER_LIBRARYD}")

if (SPINNAKER_FOUND)
    message(STATUS "Spinnaker include: ${SPINNAKER_INCLUDE_DIR}")
    message(STATUS "Spinnaker library: ${SPINNAKER_LIBRARY}")
    set(SPINNAKER_INCLUDE_DIRS "${SPINNAKER_INCLUDE_DIR}")
    set(SPINNAKER_LIBRARIES "${SPINNAKER_LIBRARY}")# "${SPINNAKER_LIBRARYD}")
endif()

if (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)
  message(STATUS "Spinnaker found in the system")
  set(Spinnaker_FOUND 1)
endif (Spinnaker_INCLUDE_DIRS AND Spinnaker_LIBRARIES)
