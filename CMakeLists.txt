cmake_minimum_required(VERSION 3.0)

project(mk)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Library

set(MK_LIBRARY_NAME  ${CMAKE_PROJECT_NAME})

# Headers
set(LIB_HEADERS include/Endian.h
				include/Note.h
				include/Scale.h
				include/Synthesis.h
				include/AIFF.h
				include/IEEEExtended.h
				include/Util.h
)

# sources
set(LIB_SOURCES src/Note.cpp
				src/Scale.cpp
				src/Synthesis.cpp
				src/AIFF.cpp
				src/IEEEExtended.cpp
				src/Util.cpp
)

find_library(LIBSNDFILE
    NAMES sndfile
)

if(LIBSNDFILE-NOTFOUND)
message(FATAL_ERROR "libsndfile not found")
endif()

message(DEBUG "libsndfile = ${LIBSNDFILE}")

add_library(${MK_LIBRARY_NAME} ${LIB_HEADERS} ${LIB_SOURCES})

# Library dependencies
target_include_directories(${MK_LIBRARY_NAME}
	PRIVATE "/usr/local/include"
)
target_link_libraries(${MK_LIBRARY_NAME}
	PRIVATE ${LIBSNDFILE}
)

# Test program
set(TEST_SOURCES test/main.cpp)

add_executable(test ${TEST_SOURCES})

add_dependencies(test ${MK_LIBRARY_NAME})

target_link_libraries(test ${MK_LIBRARY_NAME})
