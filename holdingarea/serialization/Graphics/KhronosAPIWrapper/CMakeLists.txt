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
# CMake meta build file for the simulator host-side API wrapper

set(WRAPPER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(WRAPPER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SERIALIZER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../guestrendering/vghwserialiser/inc)
set(SERIALIZER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../guestrendering/vghwserialiser/src)
set(SERIALIZATION_COMMON_INC ${CMAKE_CURRENT_SOURCE_DIR}/../../inc)
# For guestvideodriverinterfaceconstants.h file
set(VIDEODRIVER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(HOSTTHREADADAPTER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../hostsupport/hostthreadadapter/inc)

set(WRAPPER_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

if(WIN32)
	message("*** KhronosAPIWrapper Win32")
	set(WRAPPER_SSE_FLAGS "/arch:SSE2")
	set(WRAPPER_PLATFORM_LINK_LIBRARIES libEGL libOpenVG libGLESv1)
	set(WRAPPER_PLATFORM_SOURCE_DIR "win32")
else(WIN32)
	set(WRAPPER_SSE_FLAGS "-msse2 -mfpmath=sse -march=pentium4")
	set(WRAPPER_PLATFORM_LINK_LIBRARIES m stdc++)
	set(WRAPPER_PLATFORM_SOURCE_DIR "linux")
endif(WIN32)

set(WRAPPER_SRC_FILES
    ${WRAPPER_INC_DIR}/apiwrapper.h
    ${WRAPPER_INC_DIR}/eglapiwrapper.h
    ${WRAPPER_INC_DIR}/graphicsvhwcallback.h
    ${WRAPPER_INC_DIR}/KhronosAPIWrapper.h
    ${WRAPPER_INC_DIR}/khronosapiwrapperdefs.h
    ${WRAPPER_INC_DIR}/openvgapiwrapper.h
    ${WRAPPER_INC_DIR}/driverapiwrapper.h
    ${WRAPPER_INC_DIR}/opengles11apiwrapper.h

    ${SERIALIZER_INC_DIR}/remotefunctioncall.h
    ${SERIALIZER_INC_DIR}/requestbuffer.h
    ${SERIALIZER_INC_DIR}/serializedfunctioncall.h
    ${SERIALIZER_INC_DIR}/serializerplatform.h
    ${SERIALIZER_INC_DIR}/driverrfc.h
    ${SERIALIZER_INC_DIR}/opengles11rfc.h

    ${WRAPPER_SRC_DIR}/apiwrapper.cpp
    ${WRAPPER_SRC_DIR}/eglapiwrapper.cpp
    ${WRAPPER_SRC_DIR}/KhronosAPIWrapper.cpp
    ${WRAPPER_SRC_DIR}/openvgapiwrapper.cpp
    ${WRAPPER_SRC_DIR}/driverapiwrapper.cpp
    ${WRAPPER_SRC_DIR}/opengles11apiwrapper.cpp    

    ${SERIALIZER_SRC_DIR}/eglrfc.cpp
    ${SERIALIZER_SRC_DIR}/openvgrfc.cpp
    ${SERIALIZER_SRC_DIR}/remotefunctioncall.cpp
    ${SERIALIZER_SRC_DIR}/serializedfunctioncall.cpp
    ${SERIALIZER_SRC_DIR}/driverrfc.cpp
    ${SERIALIZER_SRC_DIR}/opengles11rfc.cpp
  )

include_directories(
  ${WRAPPER_INC_DIR}
  ${SERIALIZER_INC_DIR}
  ${KHRONOS_API_INC}
  ${VIDEODRIVER_INC_DIR}
  ${COMMON_INC_DIR}
  ${SERIALIZATION_COMMON_INC}
  ${HOSTTHREADADAPTER_INC_DIR}
  )

link_directories(${WRAPPER_LIBS_DIR} ${GLOBAL_LIB_DIR})

add_definitions(-DKHRONOSAPIWRAPPER_EXPORTS)

add_library(KhronosAPIWrapper SHARED ${WRAPPER_SRC_FILES})

target_link_libraries(KhronosAPIWrapper hostthreadadapter ${WRAPPER_PLATFORM_LINK_LIBRARIES})
