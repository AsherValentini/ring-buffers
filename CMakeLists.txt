cmake_minimum_required(VERSION 3.14)
project(ring-buffers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

include_directories(include) 
add_executable(prog
	src/bench.cpp
)

