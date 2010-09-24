# Copyright (C) 1994-2008 Lua.org, PUC-Rio.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# - Find libev
# Find the native LLVM includes and library
#
#  LLVM_INCLUDE_DIR - where to find ev.h, etc.
#  LLVM_LIBRARIES   - List of libraries when using libev.
#  LLVM_FOUND       - True if libev found.

find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config DOC "llvm-config executable")

if(NOT WIN32)
    execute_process(
	    COMMAND ${LLVM_CONFIG_EXECUTABLE} --cppflags
	    OUTPUT_VARIABLE LLVM_CFLAGS
	    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(NOT WIN32)
    set(LLVM_CFLAGS "-I${PROJECT_SOURCE_DIR}/llvm/llvm-gcc4.2-2.7-x86-mingw32/include -D_DEBUG -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS")
endif(NOT WIN32)

if(NOT WIN32)
    execute_process(
	    COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
	    OUTPUT_VARIABLE LLVM_LFLAGS
	    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(NOT WIN32)
	set(LLVM_LIB_PATH "${PROJECT_SOURCE_DIR}/llvm/llvm-2.7/x86-win32-vs${VISUAL_STUDIO_VERSION}/${CMAKE_BUILD_TYPE}/lib")
	message("LLVM_LIB_PATH ${LLVM_LIB_PATH}")
	link_directories(${LLVM_LIB_PATH})
endif(NOT WIN32)

if(NOT WIN32)
    execute_process(
	    COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs core jit native linker bitreader bitwriter ipo
	    OUTPUT_VARIABLE LLVM_JIT_LIBS
	    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(NOT WIN32)
    set(LLVM_JIT_LIBS "LLVMInstCombine.lib LLVMTransformUtils.lib LLVMipo.lib LLVMSupport.lib LLVMBitWriter.lib LLVMLinker.lib LLVMArchive.lib LLVMBitReader.lib LLVMTarget.lib LLVMX86AsmParser.lib LLVMX86Info.lib LLVMAsmPrinter.lib LLVMX86AsmPrinter.lib LLVMCodeGen.lib LLVMX86CodeGen.lib LLVMSelectionDAG.lib LLVMJIT.lib LLVMExecutionEngine.lib LLVMScalarOpts.lib LLVMTransformUtils.lib LLVMipa.lib LLVMAnalysis.lib LLVMMC.lib LLVMCore.lib LLVMSystem.lib")
endif(NOT WIN32)

if(NOT WIN32)
    execute_process(
	    COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs all
	    OUTPUT_VARIABLE LLVM_ALL_LIBS
	    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif(NOT WIN32)

if(NOT WIN32)
    execute_process(
        COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
        OUTPUT_VARIABLE LLVM_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(NOT WIN32)
    set(LLVM_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/llvm/llvm-2.7/include")
    message("llvm include ${LLVM_INCLUDE_DIR}")
endif(NOT WIN32)

message("llvm jit libs: " ${LLVM_JIT_LIBS})
message("llvm all libs: " ${LLVM_ALL_LIBS})

