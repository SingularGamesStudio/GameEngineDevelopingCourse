project(inih LANGUAGES CXX C)

add_library(inih src/INIReader.cpp src/INIReader.h src/ini.h src/ini.c)

include_directories(src)