cmake_minimum_required(VERSION 3.0)

##################################################
# Muzik project
##################################################

project(muzik)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

##################################################
# Muzik library
##################################################

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

add_library(${MK_LIBRARY_NAME} ${LIB_HEADERS} ${LIB_SOURCES})

# Library dependencies
target_include_directories(${MK_LIBRARY_NAME}
	PRIVATE "/usr/local/include"
)
target_link_libraries(${MK_LIBRARY_NAME}
	PRIVATE ${LIBSNDFILE}
)

##################################################
# Test program
##################################################

set(TEST_SOURCES test/main.cpp)

add_executable(test ${TEST_SOURCES})

add_dependencies(test ${MK_LIBRARY_NAME})

target_link_libraries(test ${MK_LIBRARY_NAME})

##################################################
# audio2Text utility program
##################################################

add_executable(audio2Text src/utility/audio2Text.cpp)

add_dependencies(audio2Text ${MK_LIBRARY_NAME})

target_link_libraries(audio2Text ${MK_LIBRARY_NAME})

##################################################
# normalize utility program
##################################################

add_executable(normalize src/utility/normalize.cpp)

add_dependencies(normalize ${MK_LIBRARY_NAME})

target_link_libraries(normalize ${MK_LIBRARY_NAME})

##################################################
# amplify utility program
##################################################

add_executable(amplify src/utility/amplify.cpp)

add_dependencies(amplify ${MK_LIBRARY_NAME})

target_link_libraries(amplify ${MK_LIBRARY_NAME})

##################################################
# mix utility program
##################################################

add_executable(mix src/utility/mix.cpp)

add_dependencies(mix ${MK_LIBRARY_NAME})

target_link_libraries(mix ${MK_LIBRARY_NAME})

##################################################
# Install targets
##################################################

# Install Muzik library
install(TARGETS ${MK_LIBRARY_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE})

# Install binaries and utilities
install(TARGETS test DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
install(TARGETS audio2Text DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
install(TARGETS normalize DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
install(TARGETS amplify DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
install(TARGETS mix DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
