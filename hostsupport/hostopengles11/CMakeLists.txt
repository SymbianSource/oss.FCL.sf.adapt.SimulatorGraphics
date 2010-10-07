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
# CMake meta build file for host libGLESv1

set(GLES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GLES_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

configure_file(${GLES_INC_DIR}/GLES/egl.h ${EXPORTED_INC_DIR}/GLES/egl.h COPYONLY)
configure_file(${GLES_INC_DIR}/GLES/gl.h ${EXPORTED_INC_DIR}/GLES/gl.h COPYONLY)
configure_file(${GLES_INC_DIR}/GLES/glext.h ${EXPORTED_INC_DIR}/GLES/glext.h COPYONLY)
configure_file(${GLES_INC_DIR}/GLES/glplatform.h ${EXPORTED_INC_DIR}/GLES/glplatform.h COPYONLY)
configure_file(${GLES_INC_DIR}/GLES/glextplatform.h ${EXPORTED_INC_DIR}/GLES/glextplatform.h COPYONLY)

if(WIN32)
	message("*** EGL Win32")
	set(GLES_SSE_FLAGS "/arch:SSE2")
	set(GLES_PLATFORM_LINK_LIBRARIES "")
else(WIN32)
	set(GLES_SSE_FLAGS "-msse2 -mfpmath=sse -march=pentium4")
	set(GLES_PLATFORM_LINK_LIBRARIES m stdc++)
endif(WIN32)

set(GLES_SRC_FILES
    ${GLES_INC_DIR}/EGLInterface.h
    ${GLES_INC_DIR}/glDefinitions.h
    ${GLES_INC_DIR}/GLESArray.h
    ${GLES_INC_DIR}/GLESBuffer.h
    ${GLES_INC_DIR}/GLESContext.h
    ${GLES_INC_DIR}/GLESDesktopGL.h
    ${GLES_INC_DIR}/glesGet.h
    ${GLES_INC_DIR}/glesInternal.h
    ${GLES_INC_DIR}/glesOS.h
    ${GLES_INC_DIR}/GLESTexture.h

    ${GLES_SRC_DIR}/EGLInterface.cpp
    ${GLES_SRC_DIR}/glesapi.cpp
    ${GLES_SRC_DIR}/GLESArray.cpp
    ${GLES_SRC_DIR}/GLESContext.cpp
    ${GLES_SRC_DIR}/GLESDesktopGL.cpp
    ${GLES_SRC_DIR}/glesGet.cpp
    ${GLES_SRC_DIR}/glesOS.cpp
    ${GLES_SRC_DIR}/GLESTexture.cpp
  )

include_directories(
  ${GLES_SRC_DIR}
  ${GLES_INC_DIR}
  ${COMMON_INC_DIR}
  ${EXPORTED_INC_DIR}
  )

add_definitions(-DKHRONOS_APICALL_EXPORT)

add_library(${LIBRARY_NAME_PREFIX}GLESv1 SHARED ${GLES_SRC_FILES})

target_link_libraries(${LIBRARY_NAME_PREFIX}GLESv1 ${GLES_PLATFORM_LINK_LIBRARIES})
