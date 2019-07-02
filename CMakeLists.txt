
cmake_minimum_required(VERSION 3.0)

project(midi_to_freq)

set(CMAKE_CXX_STANDARD 11)

add_executable(main
	main.cpp
	Note.h
	Note.cpp
	Scale.h
	Scale.cpp
	Synthesis.h
	Synthesis.cpp
)
