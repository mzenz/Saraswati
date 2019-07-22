cmake_minimum_required(VERSION 3.0)

project(muse)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

# MUSE library

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
)

find_library(LIBSNDFILE
    NAMES sndfile
)

if(LIBSNDFILE-NOTFOUND)
message(FATAL_ERROR "libsndfile not found")
endif()

message(DEBUG "libsndfile = ${LIBSNDFILE}")

add_library(muse ${LIB_HEADERS} ${LIB_SOURCES})

# MUSE dependencies
target_include_directories(muse
	PRIVATE "/usr/local/include"
)
target_link_libraries(muse
	PRIVATE ${LIBSNDFILE}
)

# Test program
set(TEST_SOURCES test/main.cpp)

add_executable(test ${TEST_SOURCES})

add_dependencies(test muse)

target_link_libraries(test muse)
