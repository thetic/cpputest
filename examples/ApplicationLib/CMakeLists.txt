add_library(ApplicationLib
    include/ApplicationLib/CircularBuffer.hpp
    include/ApplicationLib/EventDispatcher.hpp
    include/ApplicationLib/Printer.hpp
    include/ApplicationLib/hello.h
    src/CircularBuffer.cpp
    src/EventDispatcher.cpp
    src/Printer.cpp
    src/hello.c
)

target_include_directories(ApplicationLib PUBLIC include)
