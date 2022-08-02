add_library(ApplicationLib
    include/CircularBuffer.h
    include/EventDispatcher.h
    include/ExamplesNewOverrides.h
    include/Printer.h
    include/hello.h
    src/CircularBuffer.cpp
    src/EventDispatcher.cpp
    src/Printer.cpp
    src/hello.c
)

target_include_directories(ApplicationLib PUBLIC include)
