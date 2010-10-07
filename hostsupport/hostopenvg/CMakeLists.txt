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
# CMake meta build file for host libOpenVG

set(VG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(VG_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

configure_file(${VG_INC_DIR}/VG/openvg.h ${EXPORTED_INC_DIR}/VG/openvg.h COPYONLY)
configure_file(${VG_INC_DIR}/VG/vgext.h ${EXPORTED_INC_DIR}/VG/vgext.h COPYONLY)
configure_file(${VG_INC_DIR}/VG/vgplatform.h ${EXPORTED_INC_DIR}/VG/vgplatform.h COPYONLY)
configure_file(${VG_INC_DIR}/VG/vgu.h ${EXPORTED_INC_DIR}/VG/vgu.h COPYONLY)

# Some of the source files are generated using scripts. Check if the
# scripts were modified and rebuild if necessary.
set(ALPHA_RCP_GEN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/genheader/make_alpha_rcp.py)
set(GAMMA_LUT_GEN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/genheader/make_gamma_lut.py)

set(ALPHA_RCP_FILE ${VG_SRC_DIR}/sfAlphaRcp.h)
set(GAMMA_LUT_FILE ${VG_SRC_DIR}/sfGammaLUT.h)

add_custom_command(
    OUTPUT ${ALPHA_RCP_FILE}
    COMMAND python ${ALPHA_RCP_GEN_FILE} > ${ALPHA_RCP_FILE}
    DEPENDS ${ALPHA_RCP_GEN_FILE})

add_custom_command(
    OUTPUT ${GAMMA_LUT_FILE}
    COMMAND python ${GAMMA_LUT_GEN_FILE} > ${GAMMA_LUT_FILE}
    DEPENDS ${GAMMA_LUT_GEN_FILE})

# Required in Linux and for LLVM compilation:
# \todo LLVM script uses the flags differently, so they must be manually changed!
set(GCC_SSE_FLAGS "-msse2 -mfpmath=sse -march=pentium4")

# Win32 vs. Linux settings
if(WIN32)
	message("*** openvg_sg Win32")
	set(SF_SSE_FLAGS "/arch:SSE2")
	set(SF_PLATFORM_LINK_LIBRARIES "")
    set(C_WARNING_FLAGS "")
	add_definitions(-DOPENVG_DLL_EXPORTS -DNOMINMAX)

    set(LIB_TYPE SHARED)
else(WIN32)
	set(SF_SSE_FLAGS ${GCC_SSE_FLAGS})
	set(SF_PLATFORM_LINK_LIBRARIES m stdc++)
    set(C_WARNING_FLAGS "-Wall -std=c99 -pedantic-errors -Wno-variadic-macros -Wno-long-long -Wextra")
    set(CXX_WARNING_FLAGS "-Wall -std=c++98 -pedantic-errors -Wno-variadic-macros -Wno-long-long -Wextra")

    # Easier debugging on linux
    # \todo Just use BUILD_SHARED_LIBRARIES: This way debug builds can be made with static
    # and release with dynamic linking.
    set(LIB_TYPE SHARED)
endif(WIN32)


set(VG_SF_SRC_FILES
  ${VG_SRC_DIR}/sfEGLInterface.h
  ${VG_SRC_DIR}/sfEGLInterface.cpp
  ${VG_SRC_DIR}/riApi.cpp
  ${VG_SRC_DIR}/riArray.h
  ${VG_SRC_DIR}/riContext.cpp
  ${VG_SRC_DIR}/riContext.h
  ${VG_SRC_DIR}/riDefs.h
  ${VG_SRC_DIR}/riFont.cpp
  ${VG_SRC_DIR}/riUtils.h
  ${VG_SRC_DIR}/riUtils.cpp
  ${VG_SRC_DIR}/riFont.h
  ${VG_SRC_DIR}/riImage.cpp
  ${VG_SRC_DIR}/riImage.h
  ${VG_SRC_DIR}/riMath.cpp
  ${VG_SRC_DIR}/riMath.h
  ${VG_SRC_DIR}/riPath.cpp
  ${VG_SRC_DIR}/riPath.h
  ${VG_SRC_DIR}/riPixelPipe.cpp
  ${VG_SRC_DIR}/riPixelPipe.h
  ${VG_SRC_DIR}/riRasterizer.cpp
  ${VG_SRC_DIR}/riRasterizer.h
  ${VG_SRC_DIR}/riVGU.cpp
  ${VG_SRC_DIR}/sfMask.h
  ${VG_SRC_DIR}/sfDynamicPixelPipe.h
  ${VG_SRC_DIR}/sfDynamicPixelPipe.cpp
  ${VG_SRC_DIR}/sfBlitter.h
  ${VG_SRC_DIR}/sfBlitter.cpp
  ${VG_SRC_DIR}/sfDynamicBlitter.h
  ${VG_SRC_DIR}/sfDynamicBlitter.cpp
  ${VG_SRC_DIR}/sfFunctionCache.h
  ${VG_SRC_DIR}/sfCompiler.h
  ${VG_SRC_DIR}/sfCompiler.cpp
  ${ALPHA_RCP_FILE}
  ${GAMMA_LUT_FILE}
  )

# pthreads on linux, native threads on win32?
# \note Porting...
find_package(Threads)

include_directories(
    ${VG_SRC_DIR}
    ${COMMON_INC_DIR}
    ${EXPORTED_INC_DIR}
    ${LLVM_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

# -fvisibility=hidden?
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SF_SSE_FLAGS} ${C_WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SF_SSE_FLAGS} ${CXX_WARNING_FLAGS}")
# These preprocessor flags were defined in the visual studio project file:
# RI_COMPILE_LLVM_BYTECODE;WIN32;NDEBUG;_WINDOWS;_USRDLL;LIBOPENVG_EXPORTS;OPENVG_DLL_EXPORTS;__STDC_LIMIT_MACROS;__STDC_CONSTANT_MACROS;_CRT_SECURE_NO_DEPRECATE;_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_WARNINGS;CRT_NONSTDC_NO_WARNINGS;_SCL_SECURE_NO_DEPRECATE

# \note This compiles the full version (nothing is generated)
add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -DRI_COMPILE_LLVM_BYTECODE)

# LLVM bitcode files. \todo Create a macro to add these (LLVMTarget has some
# code that needs expansion, especially on the dependencies, which aren't
# handled well enough for our use.
set(BC_CFLAGS ${BC_CFLAGS} -DNDEBUG -O1 -msse2 -mfpmath=sse -march=pentium4 -c -emit-llvm -I${EXPORTED_INC_DIR} )

# This macro assumes that a pixelpipe is generated from one source-code file atm.
macro(make_pixelpipe target_name input_file output_file array_name)
    set(BINARY_PIXELPIPE_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(BINARY_PIXELPIPE_TARGET ${target_name})
    set(BINARY_PIXELPIPE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_PIXELPIPE_TARGET}.bc)
    set(BINARY_PIXELPIPE_HEADER ${BINARY_PIXELPIPE_DIR}/${output_file})

    set_source_files_properties(${BINARY_PIXELPIPE_HEADER}
        PROPERTIES GENERATED true)

    set(LLVM_PIXELPIPE_FILES ${VG_SRC_DIR}/${input_file})

    # Note: Adds all project files as dependency to bitcode file.
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${input_file}.bc
        COMMAND ${LLVM_CC} ARGS ${BC_CFLAGS} -o ${CMAKE_CURRENT_BINARY_DIR}/${input_file}.bc ${LLVM_PIXELPIPE_FILES}
        DEPENDS ${LLVM_PIXELPIPE_FILES} ${VG_SF_SRC_FILES}
    )

    add_custom_command(
        OUTPUT ${BINARY_PIXELPIPE_FILE}
        COMMAND ${LLVM_LINK} ARGS -f -o ${BINARY_PIXELPIPE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${input_file}.bc
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${input_file}.bc
    )

    set(BINARY_PIXELPIPE_HEADER_DEPS ${BINARY_PIXELPIPE_FILE})

    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        add_custom_command(
            OUTPUT ${BINARY_PIXELPIPE_FILE}.disasm
            COMMAND ${LLVM_DIS} ARGS ${BINARY_PIXELPIPE_FILE} -f -o ${BINARY_PIXELPIPE_FILE}.disasm
            DEPENDS ${BINARY_PIXELPIPE_FILE}
        )
        set(BINARY_PIXELPIPE_HEADER_DEPS ${BINARY_PIXELPIPE_DEPS} ${BINARY_PIXELPIPE_FILE}.disasm)
    endif()

    add_custom_command(
        OUTPUT ${BINARY_PIXELPIPE_HEADER}
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/genheader/bin2h.py ${BINARY_PIXELPIPE_FILE} ${array_name} > ${BINARY_PIXELPIPE_HEADER}
        DEPENDS ${BINARY_PIXELPIPE_HEADER_DEPS}
    )

    set(VG_SF_SRC_FILES ${VG_SF_SRC_FILES} ${BINARY_PIXELPIPE_HEADER})
endmacro(make_pixelpipe)

make_pixelpipe(llvm_pixelpipe sfDynamicPixelPipe.cpp binaryPixelPipe.h integerPixelPipe_binary)
make_pixelpipe(llvm_blitter sfDynamicBlitter.cpp binaryBlitter.h integerBlitter_binary)

add_library(${LIBRARY_NAME_PREFIX}OpenVG ${LIB_TYPE} ${VG_SF_SRC_FILES})

set_target_properties(${LIBRARY_NAME_PREFIX}OpenVG PROPERTIES LINK_INTERFACE_LIBRARIES "")

target_link_libraries(${LIBRARY_NAME_PREFIX}OpenVG ${SF_PLATFORM_LINK_LIBRARIES} ${OPENGL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LLVM_JIT_LIBS})
