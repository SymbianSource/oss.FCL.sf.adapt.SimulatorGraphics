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
# Temporary CMake meta build file for hostthreadadapter and holdingarea

cmake_minimum_required (VERSION 2.8)

set(GLOBAL_LIB_DIR ${PROJECT_SOURCE_DIR}/holdingarea/lib)

add_subdirectory(hostsupport/hostthreadadapter)
add_subdirectory(holdingarea)
