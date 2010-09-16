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
# CMake meta build file for hostthreadadapter

set(HTA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HTA_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(HTA_SRC_FILES
    ${HTA_INC_DIR}/hostthreadadapter.h
    ${HTA_INC_DIR}/platformthreading.h
    ${HTA_INC_DIR}/platformtypes.h

    ${HTA_SRC_DIR}/platformthreading.cpp
  )

include_directories(
  ${HTA_INC_DIR}
  )

link_directories(${GLOBAL_LIB_DIR})

add_library(hostthreadadapter STATIC ${HTA_SRC_FILES})
