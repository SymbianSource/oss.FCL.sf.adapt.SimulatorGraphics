# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and /or associated documentation files
# (the "Materials "), to deal in the Materials without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Materials,
# and to permit persons to whom the Materials are furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Materials.
#
# THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
# THE USE OR OTHER DEALINGS IN THE MATERIALS.
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# CMake meta build file for host libGLESv2

set(GLES2_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/GLES2)
set(GLES2_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

if(WIN32)
	message("*** EGL Win32")
	set(GLES2_SSE_FLAGS "/arch:SSE2")
	set(GLES2_PLATFORM_LINK_LIBRARIES "")
else(WIN32)
	set(GLES2_SSE_FLAGS "-msse2 -mfpmath=sse -march=pentium4")
	set(GLES2_PLATFORM_LINK_LIBRARIES m stdc++)
endif(WIN32)

set(GLES2_SRC_FILES
    ${GLES2_INC_DIR}/common.h
    
    ${GLES2_SRC_DIR}/buffer.h
    ${GLES2_SRC_DIR}/context.h
    ${GLES2_SRC_DIR}/degl.h
    ${GLES2_SRC_DIR}/get.h
    ${GLES2_SRC_DIR}/half.h
    ${GLES2_SRC_DIR}/hgl.h
    ${GLES2_SRC_DIR}/object.h
    ${GLES2_SRC_DIR}/platform.h
    ${GLES2_SRC_DIR}/program.h
    ${GLES2_SRC_DIR}/shader.h
    ${GLES2_SRC_DIR}/vertex.h
    ${GLES2_SRC_DIR}/texture.h
    ${GLES2_SRC_DIR}/util.h
    ${GLES2_SRC_DIR}/fbo.h	
    
    ${GLES2_SRC_DIR}/blend.c
    ${GLES2_SRC_DIR}/buffer.c
    ${GLES2_SRC_DIR}/clear.c
    ${GLES2_SRC_DIR}/context.c
    ${GLES2_SRC_DIR}/depth.c
    ${GLES2_SRC_DIR}/draw.c
    ${GLES2_SRC_DIR}/fbo.c
    ${GLES2_SRC_DIR}/get.c
    ${GLES2_SRC_DIR}/half.c
    ${GLES2_SRC_DIR}/hgl.c
    ${GLES2_SRC_DIR}/object.c
    ${GLES2_SRC_DIR}/pixel.c
    ${GLES2_SRC_DIR}/platform.c
    ${GLES2_SRC_DIR}/program.c
    ${GLES2_SRC_DIR}/shader.c
    ${GLES2_SRC_DIR}/stencil.c
    ${GLES2_SRC_DIR}/texture.c
    ${GLES2_SRC_DIR}/vertex.c
    ${GLES2_SRC_DIR}/util.c
    ${GLES2_SRC_DIR}/degl.cpp
  )

include_directories(
  ${GLES2_SRC_DIR}
  ${GLES2_INC_DIR}
  ${COMMON_INC_DIR}
  )

if (WIN32)
    add_definitions(-DCONFIG_WIN32 -DBUILD_GLES2)
else(WIN32)
    add_definitions(-DCONFIG_X11 -DBUILD_GLES2)
endif(WIN32)

add_library(${LIBRARY_NAME_PREFIX}GLESv2 SHARED ${GLES2_SRC_FILES})

target_link_libraries(${LIBRARY_NAME_PREFIX}GLESv2 ${GLES2_PLATFORM_LINK_LIBRARIES})
