
set(BosonSOURCE 
        AssemblyInfo.cpp
        graphHelpers.cpp
        ir16dump.cpp
        ir16filter.cpp
        IR16FrameEventAdapter.cpp
        ir16pin.cpp
        ir16preview.cpp
        ir16queuePin.cpp
        ManagedIR16Filters.cpp
        Stdafx.cpp
)

set(BosonHEADER
        graphHelpers.h
        ir16dump.h
        ir16filter.h
        IR16FrameEventAdapter.h
        ir16pin.h
        ir16preview.h
        ir16queuePin.h
        ManagedIR16Filters.h
        resource.h
        Stdafx.h
        streams.h
)

add_library(BosonThermal ${BosonSOURCE} ${BosonHEADER})

target_include_directories(BosonThermal
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(BosonThermal syndicate)
