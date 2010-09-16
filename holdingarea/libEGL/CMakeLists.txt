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
# CMake meta build file for host libEGL

set(EGL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EGL_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

find_package(OpenGL)

if(WIN32)
	message("*** EGL Win32")
	set(EGL_SSE_FLAGS "/arch:SSE2")
	set(EGL_PLATFORM_LINK_LIBRARIES "")
	set(EGL_PLATFORM_SOURCE_DIR "win32")
else(WIN32)
	set(EGL_SSE_FLAGS "-msse2 -mfpmath=sse -march=pentium4")
	set(EGL_PLATFORM_LINK_LIBRARIES m stdc++)
	set(EGL_PLATFORM_SOURCE_DIR "linux")
endif(WIN32)

set(EGL_SRC_FILES
    ${COMMON_INC_DIR}/SurfaceDescriptor.h
    ${COMMON_INC_DIR}/ColorDescriptor.h
    ${COMMON_INC_DIR}/GLESInterface.h
    ${COMMON_INC_DIR}/GLES2Interface.h
    ${COMMON_INC_DIR}/VGInterface.h
    ${COMMON_INC_DIR}/BufferContainer.h
    ${COMMON_INC_DIR}/ImageTarget.h
    ${EGL_INC_DIR}/EGLConfig.h
    ${EGL_INC_DIR}/EGLContext.h
    ${EGL_INC_DIR}/eglDefinitions.h
    ${EGL_INC_DIR}/EGLDisplay.h
    ${EGL_INC_DIR}/eglInternal.h
    ${EGL_INC_DIR}/EGLPbufferSurface.h
    ${EGL_INC_DIR}/EGLProcess.h
    ${EGL_INC_DIR}/EGLState.h
    ${EGL_INC_DIR}/EGLSurface.h
    ${EGL_INC_DIR}/EGLThread.h
    ${EGL_INC_DIR}/EGLWindowSurface.h
    ${EGL_INC_DIR}/TransparentColor.h
    ${EGL_INC_DIR}/EGLUtils.h
    ${EGL_INC_DIR}/EGLImage.h
    ${EGL_INC_DIR}/EGLExtension.h
    ${EGL_INC_DIR}/EGLOs.h
    ${EGL_INC_DIR}/EGLStructs.h

    ${EGL_SRC_DIR}/ColorDescriptor.cpp
    ${EGL_SRC_DIR}/eglapi.cpp
    ${EGL_SRC_DIR}/EGLConfig.cpp
    ${EGL_SRC_DIR}/EGLContext.cpp
    ${EGL_SRC_DIR}/EGLDisplay.cpp
    ${EGL_SRC_DIR}/EGLPbufferSurface.cpp
    ${EGL_SRC_DIR}/EGLProcess.cpp
    ${EGL_SRC_DIR}/EGLState.cpp
    ${EGL_SRC_DIR}/EGLSurface.cpp
    ${EGL_SRC_DIR}/EGLThread.cpp
    ${EGL_SRC_DIR}/EGLWindowSurface.cpp
    ${EGL_SRC_DIR}/TransparentColor.cpp
    ${EGL_SRC_DIR}/EGLUtils.cpp
    ${EGL_SRC_DIR}/EGLImage.cpp
    ${EGL_SRC_DIR}/EGLExtension.cpp
    ${EGL_SRC_DIR}/${EGL_PLATFORM_SOURCE_DIR}/EGLOs.cpp
  )

include_directories(
  ${EGL_SRC_DIR}
  ${EGL_INC_DIR}
  ${COMMON_INC_DIR}
  )

# \todo MSVC:
# Probably not needed for EGL, but it might be a good idea to define these:
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2 -mfpmath=sse -march=pentium4")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mfpmath=sse -march=pentium4")

if(PLATSIM_EXTENSIONS)
   add_definitions(-DKHRONOS_APICALL_EXPORT -DEGLI_USE_PLATSIM_EXTENSIONS)
else(PLATSIM_EXNTENSIONS)
   add_definitions(-DKHRONOS_APICALL_EXPORT)
endif(PLATSIM_EXTENSIONS)

add_library(${LIBRARY_NAME_PREFIX}EGL SHARED ${EGL_SRC_FILES})

set_target_properties(${LIBRARY_NAME_PREFIX}EGL PROPERTIES LINK_INTERFACE_LIBRARIES "")

target_link_libraries(${LIBRARY_NAME_PREFIX}EGL ${EGL_PLATFORM_LINK_LIBRARIES} ${OPENGL_LIBRARIES})
