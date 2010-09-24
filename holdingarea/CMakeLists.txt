# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http:#www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# CMake meta build file for the host components in the holdingarea

cmake_minimum_required (VERSION 2.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "Config types" FORCE)

project(PlatsimGraphics)

include(cmake/FindLLVM.cmake)

option(
    USE_MINI_EGL
    "Use the small EGL implementation that came with the RI."
    OFF)

option(
    DEV_EXTRAS
    "Compile internally used components. Not available in public release."
    OFF)

if(NOT USE_MINI_EGL)
    message("*** Using proper EGL")
    add_definitions(-DUSE_FULL_EGL)
else(NOT USE_MINI_EGL)
    message("*** Using riMiniEGL")
endif(NOT USE_MINI_EGL)

if(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    message("*** Defining profiling flags")
    add_definitions(-DSF_PROFILE)
endif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)

# Some LLVM-definitions:
message("*** LLVM binaries ***")
if(WIN32)
    set(LLVM_CC "${PROJECT_SOURCE_DIR}/llvm/llvm-gcc4.2-2.7-x86-mingw32/bin/llvm-g++")
    set(LLVM_LINK "${PROJECT_SOURCE_DIR}/llvm/llvm-2.7/bin/llvm-link")
    set(LLVM_DIS "${PROJECT_SOURCE_DIR}/llvm/llvm-2.7/bin/llvm-dis")
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

set(GLOBAL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(COMMON_INC_DIR ${PROJECT_SOURCE_DIR}/inc)

add_subdirectory(libEGL)					# EGL for subprojects
add_subdirectory(vg/2D_OpenVG_1_1_SF)                 	      	# Optimized OpenVG with RI as starting point
# Any project using OpenVG can use this to get access to VG/ include directory.
# \note If OpenVG was to be made into a module, this should be in, e.g., FindOpenVG.
set(OPENVG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vg/2D_OpenVG_1_1_SF/ri/include/)

add_subdirectory(libGLESv1)					# GLES to OGL wrapper
add_subdirectory(libGLESv2)					# GLES2 to OGL wrapper

if(DEV_EXTRAS)
    set(DEV_EXTRA_TARGETS conformes11-conform conformes11-covegl conformes11-covgl conformes11-primtest expat conformes20)
    add_subdirectory(vg/tiger)                          # Initial benchmark for solid-fill changes, aa.
    add_subdirectory(conformes11)
    add_subdirectory(conformes20)
    if(NOT WIN32)
        # Disabled from win32-build since we should use binaries there:
        add_subdirectory(spandex/parser)                       # Parser generator from customer
        add_subdirectory(spandex/vg)                           # Test suite from customer
    endif(NOT WIN32)
endif(DEV_EXTRAS)

if(WIN32)
    if (PLATSIM_EXTENSIONS)
        add_subdirectory(serialization/Graphics/KhronosAPIWrapper)	# Simulator serialization (host)
        add_dependencies(KhronosAPIWrapper libEGL libGLESv1 libGLESv2 hostthreadadapter)
		add_subdirectory(syborggraphicswrapper) # Simulator serialization (guest)
        add_dependencies(syborg-graphicswrapper KhronosAPIWrapper hostthreadadapter)
        set(ALL_TARGETS libEGL libOpenVG libGLESv1 libGLESv2 hostthreadadapter KhronosAPIWrapper syborg-graphicswrapper ${DEV_EXTRA_TARGETS})
    else(PLATSIM_EXTENSIONS)
        set(ALL_TARGETS libEGL libOpenVG libGLESv1 libGLESv2 ${DEV_EXTRA_TARGETS})
    endif(PLATSIM_EXTENSIONS)
else(WIN32)
# Wrapper not implemented
    set(ALL_TARGETS libEGL libOpenVG libGLESv1 libGLESv2 ${DEV_EXTRA_TARGETS})
    if(DEV_EXTRAS)
        add_subdirectory(vg/2D_OpenVG_1_1)                  # Accelerated reference provided by customer ("s60")
        add_subdirectory(vg/s60_tiger)                      # "s60" build of the tiger benchmark
        add_subdirectory(vg/original_sf)                    # Original OpenVG before optimizations
        add_subdirectory(vg/2D_OpenVG_tests/vg_gremlin)     # Fuzzer
    endif(DEV_EXTRAS)
endif(WIN32)

if(WIN32)
    # These help create the win32 release
    # \todo Use CMake on linux to do the same: Should create zips automatically.
    set_target_properties(${ALL_TARGETS} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
            ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif(WIN32)
