cmake_minimum_required(VERSION 3.13)

#define build tools
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

#define project
project(lain_vision VERSION 0.0.1 DESCRIPTION "extra sensory perception for cs2" LANGUAGES CXX)

#add sources
set (SOURCES_LAIN_VISION
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/radar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/state.cpp
    )

set (HEADERS_LAIN_VISION
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/draw.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/radar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/state.h
    )

#define targets
add_executable(lain_vision ${SOURCES_LAIN_VISION} ${HEADERS_LAIN_VISION})
add_executable(lain_vision_debug ${SOURCES_LAIN_VISION} ${HEADERS_LAIN_VISION})


#find all other dependencies
find_package(SDL2 REQUIRED)
find_package(X11 REQUIRED)

find_library(XEXT_LIB Xext)
if(NOT XEXT_LIB)
    message(FATAL_ERROR "Xext not found")
endif()

find_library(XFIXES_LIB Xfixes)
if(NOT XFIXES_LIB)
    message(FATAL_ERROR "Xfixes not found")
endif()

find_library(CONFIG_LIB config++)
if(NOT CONFIG_LIB)
    message(FATAL_ERROR "config++ not found")
endif()

find_library(PWU_LIB pwu)
if(NOT PWU_LIB)
    message(FATAL_ERROR "libpwu not found")
endif()


#lain_vision options
target_compile_features(lain_vision PUBLIC cxx_std_17)
target_compile_options(lain_vision PUBLIC -O2 -Wall)
target_link_libraries(lain_vision PUBLIC "${PWU_LIB}" PUBLIC "${CONFIG_LIB}" PUBLIC "${X11_LIBRARIES}" PUBLIC "${XEXT_LIB}" PUBLIC "${XFIXES_LIB}" PUBLIC "${SDL2_LIBRARIES}")

#lain_vision_debug options
target_compile_features(lain_vision_debug PUBLIC cxx_std_17)
target_compile_options(lain_vision_debug PUBLIC -ggdb -O0 -Wall)
target_link_libraries(lain_vision_debug PUBLIC "${PWU_LIB}" PUBLIC "${CONFIG_LIB}" PUBLIC "${X11_LIBRARIES}" PUBLIC "${XEXT_LIB}" PUBLIC "${XFIXES_LIB}" PUBLIC "${SDL2_LIBRARIES}")
