if(DOS)
    add_subdirectory(DOS)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IAR")
    add_subdirectory(IAR) # TODO
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "TI")
    add_subdirectory(TI) # TODO
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_subdirectory(MSVC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Borland")
    add_subdirectory(Borland) # TODO
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "ARMCC")
    add_subdirectory(ARMCC) # TODO
else()
    add_subdirectory(GNU)
endif()
