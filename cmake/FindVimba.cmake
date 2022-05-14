# Find the Vimba library
#
#  Vimba_FOUND        - True if Vimba was found.
#  Vimba_LIBRARIES    - The libraries needed to use Vimba
#  Vimba_INCLUDE_DIRS - Location of Vimba.h

unset(Vimba_FOUND)
unset(Vimba_INCLUDE_DIRS)
unset(Vimba_LIBRARIES)

find_path(Vimba_INCLUDE_DIRS
  NAMES
  "VimbaCPP.h"
  PATHS
  "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Source/VimbaCPP/Include"
  # "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Source"
  # "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Examples"
  # "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Source/VimbaCPP"
  # "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Examples/VimbaCPP"
  # "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Examples/VimbaCPP/Include"
  )
message(STATUS "find_path result:")
message(STATUS "${Vimba_INCLUDE_DIRS}")

find_library(Vimba_LIBRARIES
  NAMES
  VimbaCPP.lib
  PATHS
  "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Source/Build"
  "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Source/Build/VS2010/x64/Debug"
  # "C:/Users/Public/Documents/Allied Vision/Vimba_6.0/VimbaCPP_Source/VimbaCPP/Include"
  )

message(STATUS "find_library result:")
message(STATUS "${Vimba_LIBRARIES}")

if (Vimba_FOUND)
    message(STATUS "Vimba include: ${Vimba_INCLUDE_DIR}")
    message(STATUS "Vimba library: ${Vimba_LIBRARY}")
    set(Vimba_INCLUDE_DIRS "${Vimba_INCLUDE_DIR}")
    set(Vimba_LIBRARIES "${Vimba_LIBRARY}")# "${Vimba_LIBRARYD}")
endif()

if (Vimba_INCLUDE_DIRS AND Vimba_LIBRARIES)
  message(STATUS "Vimba found in the system")
  set(Vimba_FOUND 1)
endif (Vimba_INCLUDE_DIRS AND Vimba_LIBRARIES)