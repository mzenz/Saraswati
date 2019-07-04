cmake_minimum_required(VERSION 3.0)

project(muse)

set(CMAKE_CXX_STANDARD 11)

add_executable(main
	src/main.cpp
	src/Note.h
	src/Note.cpp
	src/Scale.h
	src/Scale.cpp
	src/Synthesis.h
	src/Synthesis.cpp
)
