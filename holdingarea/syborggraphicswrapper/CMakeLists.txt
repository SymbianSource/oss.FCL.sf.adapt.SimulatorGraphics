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
# CMake meta build file for the simulator guest-side API wrapper

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

set(HOSTTHREADADAPTER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../hostsupport/hostthreadadapter/inc)
set(KHRONOSAPIWRAPPER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../serialization/Graphics/KhronosAPIWrapper/inc)
set(GUESTVIDEODRIVERINTERFACECONSTANTS_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../serialization/Graphics)
set(SERIALIZER_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../guestrendering/vghwserialiser/inc)

set(SRC_FILES
    ${INC_DIR}/syborg-graphicswrapper.h
    ${SRC_DIR}/syborg-graphicswrapper.cpp
)

include_directories(
  ${INC_DIR}
  ${COMMON_INC_DIR}
  ${HOSTTHREADADAPTER_INC_DIR}
  ${KHRONOSAPIWRAPPER_INC_DIR}
  ${SERIALIZER_INC_DIR}
  ${GUESTVIDEODRIVERINTERFACECONSTANTS_INC_DIR}
  )

link_directories(${LIBS_DIR} ${GLOBAL_LIB_DIR})

add_definitions(-DSYBORG_GRAPHICSWRAPPER_EXPORTS)

add_library(syborg-graphicswrapper SHARED ${SRC_FILES})

target_link_libraries(syborg-graphicswrapper KhronosAPIWrapper hostthreadadapter)


