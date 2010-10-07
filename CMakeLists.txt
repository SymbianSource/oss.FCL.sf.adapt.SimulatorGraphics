# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# CMake meta build file for the host components

cmake_minimum_required (VERSION 2.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "Config types" FORCE)

project(HostGraphics)

set(LLVM_INSTALL_DIR ${PROJECT_SOURCE_DIR}/holdingarea/llvm)
include(FindLLVM.cmake)

add_definitions(-DUSE_FULL_EGL)

if(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    message("*** Defining profiling flags")
    add_definitions(-DSF_PROFILE)
endif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)

# Some LLVM-definitions:
message("*** LLVM binaries ***")
if(WIN32)
    set(LLVM_CC "${LLVM_INSTALL_DIR}/llvm-gcc4.2-2.7-x86-mingw32/bin/llvm-g++")
    set(LLVM_LINK "${LLVM_INSTALL_DIR}/llvm-2.7/bin/llvm-link")
    set(LLVM_DIS "${LLVM_INSTALL_DIR}/llvm-2.7/bin/llvm-dis")
else(WIN32)
    set(LLVM_CC "llvm-g++")
    set(LLVM_LINK "llvm-link")
    set(LLVM_DIS "llvm-dis")
endif(WIN32)

# Compiler flags before including any subprojects:
if(WIN32)
# Use defaults
else(WIN32)
    set(C_DEFAULT_FLAGS "-Wall -std=c99 -pedantic-errors -Wno-variadic-macros -Wno-long-long -Wextra")
    set(CXX_DEFAULT_FLAGS "-Wall -std=c++98 -pedantic-errors -Wno-variadic-macros -Wno-long-long -Wextra")
    add_definitions(-D_GNU_SOURCE)
endif(WIN32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_DEFAULT_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_DEFAULT_FLAGS}")

if(WIN32)
    set(LIBRARY_NAME_PREFIX lib)
else(WIN32)
    set(LIBRARY_NAME_PREFIX)
endif(WIN32)

set(EXECUTABLE_BASE_DIR ${EPOCROOT}epoc32/release/tools2)
set(GLOBAL_LIB_DIR ${EXECUTABLE_BASE_DIR}/lib)
set(EXPORTED_INC_DIR ${EPOCROOT}epoc32/include/tools)
set(COMMON_INC_DIR ${PROJECT_SOURCE_DIR}/inc)

add_subdirectory(hostsupport/hostegl)                   # EGL for subprojects
add_subdirectory(hostsupport/hostopenvg)                # Optimized OpenVG with RI as starting point
add_subdirectory(hostsupport/hostopengles11)            # GLES to OGL wrapper
add_subdirectory(hostsupport/hostopengles20)            # GLES2 to OGL wrapper

if(WIN32)
    if (SIMULATOR_EXTENSIONS)
        add_subdirectory(vghwinterface/vghwapiwrapper)	# Simulator serialization (host)
        add_dependencies(KhronosAPIWrapper libEGL libGLESv1 libGLESv2)
        add_dependencies(syborg-graphicswrapper libEGL libGLESv1 libGLESv2)
        set(ALL_TARGETS libEGL libOpenVG libGLESv1 libGLESv2 KhronosAPIWrapper syborg-graphicswrapper)
    else(SIMULATOR_EXTENSIONS)
        set(ALL_TARGETS libEGL libOpenVG libGLESv1 libGLESv2)
    endif(SIMULATOR_EXTENSIONS)
else(WIN32)
# Wrapper not implemented
    set(ALL_TARGETS libEGL libOpenVG libGLESv1 libGLESv2)
endif(WIN32)

if(WIN32)
    # These help create the win32 release
    # \todo Use CMake on linux to do the same: Should create zips automatically.
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        set_target_properties(${ALL_TARGETS} PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${GLOBAL_LIB_DIR}
                ARCHIVE_OUTPUT_DIRECTORY ${GLOBAL_LIB_DIR}
                RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_BASE_DIR}/deb)
    else(CMAKE_BUILD_TYPE STREQUAL Debug)
        set_target_properties(${ALL_TARGETS} PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${GLOBAL_LIB_DIR}
                ARCHIVE_OUTPUT_DIRECTORY ${GLOBAL_LIB_DIR}
                RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_BASE_DIR}/rel)
    endif(CMAKE_BUILD_TYPE STREQUAL Debug)
endif(WIN32)
