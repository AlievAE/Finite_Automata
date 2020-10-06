cmake_minimum_required(VERSION 3.1)

set(header automaton.h)
set(source automaton.cpp)
add_library(library ${source} ${header})
