# ########################################################################################## WSL v1

# # Find the SpinVideo library

# unset(SpinVideo_FOUND)
# unset(SpinVideo_INCLUDE_DIRS)
# unset(SpinVideo_LIBRARIES)

# find_path(SpinVideo_INCLUDE_DIRS NAMES
#   "SpinVideo.h"
#   PATHS
#   "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/include"
#   )
# message(STATUS "hi anriudh")
# message(STATUS "${SpinVideo_INCLUDE_DIRS}")

# find_library(SpinVideo_LIBRARIES 
#   NAMES SpinVideo_v140
#   PATHS
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib64/vs2015"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib64"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib/vs2015"
#   "/mnt/c/Program Files/FLIR Systems/Spinnaker/lib"
# )
# message(STATUS "hi adnan")
# message(STATUS "${SpinVideo_LIBRARIES}")

# if (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)
#   message(STATUS "SpinVideo found in the system")
#   set(SpinVideo_FOUND 1)
# endif (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)

########################################################################################## Windows v1

# # Find the SpinVideo library

# unset(SpinVideo_FOUND)
# unset(SpinVideo_INCLUDE_DIRS)
# unset(SpinVideo_LIBRARIES)

# find_path(SpinVideo_INCLUDE_DIRS NAMES
#   "SpinVideo.h"
#   PATHS
#   "C:/Program Files/FLIR Systems/Spinnaker/include"
#   )
# message(STATUS "hi anriudh")
# message(STATUS "${SpinVideo_INCLUDE_DIRS}")

# find_library(SpinVideo_LIBRARIES 
#   NAMES SpinVideo_v140
#   PATHS
#   "C:/Program Files/FLIR Systems/Spinnaker/lib64/vs2015"
#   "C:/Program Files/FLIR Systems/Spinnaker/lib64"
#   "C:/Program Files/FLIR Systems/Spinnaker/bin64"
#   "C:/Program Files/FLIR Systems/Spinnaker/lib/vs2015"
#   "C:/Program Files/FLIR Systems/Spinnaker/lib"
# )
# message(STATUS "hi adnan")
# message(STATUS "${SpinVideo_LIBRARIES}")

# if (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)
#   message(STATUS "SpinVideo found in the system")
#   set(SpinVideo_FOUND 1)
# endif (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)

# ########################################################################################## WSL v2

# # Find the SpinVideo library
# #
# #  SpinVideo_FOUND        - True if SpinVideo was found.
# #  SpinVideo_LIBRARIES    - The libraries needed to use SpinVideo
# #  SpinVideo_INCLUDE_DIRS - Location of SpinVideo.h

# unset(SpinVideo_FOUND)
# unset(SpinVideo_INCLUDE_DIRS)
# unset(SpinVideo_LIBRARIES)

# find_path(SpinVideo_INCLUDE_DIRS NAMES
#   "SpinVideo.h"
#   PATHS
#   "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/include"
# #   "C:/Program Files/FLIR Systems/Spinnaker/include"
#   )
# message(STATUS "hi anriudh")
# message(STATUS "${SpinVideo_INCLUDE_DIRS}")

# # find_library(SpinVideo_LIBRARIES 
# #   NAMES SpinVideo_v140.dll
# #   PATHS
# #   "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
# #   "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64/vs2015"
# # )

# find_library(SpinVideo_LIBRARIES NAMES SpinVideo_v140.lib
#                 PATHS
#                 "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/lib/vs2015"
#                 "/mnt/c/Program\ Files/FLIR\ Systems/Spinnaker/lib64/vs2015")

# message(STATUS "hi adnan")
# message(STATUS "${SpinVideo_LIBRARIES}")

# find_library(SpinVideo_LIBRARYD
#                 NAMES SpinVideod_v140.dll
#                 PATHS
#                 "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
#                 "/mnt/c/Program Files/FLIR Systems/Spinnaker/bin64/vs2015")

# message(STATUS "hi adnan")
# message(STATUS "${SpinVideo_LIBRARYD}")

# if (SpinVideo_FOUND)
#     message(STATUS "SpinVideo include: ${SpinVideo_INCLUDE_DIR}")
#     message(STATUS "SpinVideo library: ${SpinVideo_LIBRARY}")
#     set(SpinVideo_INCLUDE_DIRS "${SpinVideo_INCLUDE_DIR}")
#     set(SpinVideo_LIBRARIES "${SpinVideo_LIBRARY}")# "${SpinVideo_LIBRARYD}")
# endif()

# if (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)
#   message(STATUS "SpinVideo found in the system")
#   set(SpinVideo_FOUND 1)
# endif (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)

# ########################################################################################## Windows v2

# Find the SpinVideo library
#
#  SpinVideo_FOUND        - True if SpinVideo was found.
#  SpinVideo_LIBRARIES    - The libraries needed to use SpinVideo
#  SpinVideo_INCLUDE_DIRS - Location of SpinVideo.h

unset(SpinVideo_FOUND)
unset(SpinVideo_INCLUDE_DIRS)
unset(SpinVideo_LIBRARIES)

find_path(SpinVideo_INCLUDE_DIRS NAMES
  "SpinVideo.h"
  PATHS
  "C:/Program\ Files/FLIR\ Systems/Spinnaker/include"
#   "C:/Program Files/FLIR Systems/Spinnaker/include"
  )
message(STATUS "find_path result:")
message(STATUS "${SpinVideo_INCLUDE_DIRS}")

# find_library(SpinVideo_LIBRARIES 
#   NAMES SpinVideo_v140.dll
#   PATHS
#   "C:/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
#   "C:/Program Files/FLIR Systems/Spinnaker/bin64/vs2015"
# )

find_library(SpinVideo_LIBRARIES NAMES SpinVideo_v140.lib
                PATHS
                "C:/Program\ Files/FLIR\ Systems/Spinnaker/lib/vs2015"
                "C:/Program\ Files/FLIR\ Systems/Spinnaker/lib64/vs2015")

message(STATUS "find_library result:")
message(STATUS "${SpinVideo_LIBRARIES}")

# find_library(SpinVideo_LIBRARYD
#                 NAMES SpinVideod_v140.dll
#                 PATHS
#                 "C:/Program Files/FLIR Systems/Spinnaker/bin/vs2015"
#                 "C:/Program Files/FLIR Systems/Spinnaker/bin64/vs2015")

# message(STATUS "hi adnan")
# message(STATUS "${SpinVideo_LIBRARYD}")

if (SpinVideo_FOUND)
    message(STATUS "SpinVideo include: ${SpinVideo_INCLUDE_DIR}")
    message(STATUS "SpinVideo library: ${SpinVideo_LIBRARY}")
    set(SpinVideo_INCLUDE_DIRS "${SpinVideo_INCLUDE_DIR}")
    set(SpinVideo_LIBRARIES "${SpinVideo_LIBRARY}")# "${SpinVideo_LIBRARYD}")
endif()

if (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)
  message(STATUS "SpinVideo found in the system")
  set(SpinVideo_FOUND 1)
endif (SpinVideo_INCLUDE_DIRS AND SpinVideo_LIBRARIES)



