/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef __SFCOMPILER_H
#define __SFCOMPILER_H

#include <string>

#ifndef __SFDYNAMICPIXELPIPE_H
#   include "sfDynamicPixelPipe.h"
#endif

#ifndef __SFDYNAMICBLITTER_H
#   include "sfDynamicBlitter.h"
#endif

#ifndef __RIPIXELPIPE_H
#   include "riPixelPipe.h"
#endif

#ifndef __SFFUNCTIONCACHE_H
#   include "sfFunctionCache.h"
#endif

#include "llvm/LLVMContext.h"

// \note PPCompiler class also caches a certain amount of compiled functions.
// It may make sense to move the cache into a separate container.

// LLVM forward declarations
namespace llvm { 
    class Type;
    class Constant;
    class Function;
    class Module;
    class ExecutionEngine;
}

namespace OpenVGRI {

// Pixel-pipeline function with constant state removed:
typedef void (*PixelPipeFunction)(const PixelPipe::PPUniforms&, PixelPipe::PPVariants&, const Span*, int);
// Image-blitting function with constant state removed:
typedef void (*BlitterFunction)(const DynamicBlitter::BlitUniforms&);

class PPCompiler
{
public:
    typedef int PixelPipeHandle;
    typedef int BlitterHandle;
private:
    // Function that generates LLVM-constant from a pixel-pipeline:
    typedef ::llvm::Constant* (*ConstantGenFunc)(const void* structure, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType);

    struct PPCompilerContext {
        PPCompilerContext();
        ~PPCompilerContext();
        // Stores persistent objects related to each component (pixelpipe or blitter).
        // Note that the execution engine must be a per-process singleton for LLVM before
        // version 2.7!
        ::llvm::Module* module;
        ::llvm::Function* llvmFunction; 
    };

    struct PartialEvalFunc
    {
        ::llvm::Function*   llvmFunc;
        ::llvm::GlobalVariable*   llvmConst;
    };

public:
    PPCompiler();
    ~PPCompiler();

    static PartialEvalFunc compilePixelPipeline(::llvm::LLVMContext& llvmContext, PPCompilerContext& compilerContext, ConstantGenFunc constGenFunc, const void* state, const std::string& newFuntionName);

    // These functions get an reserve a handle to a pixelpipe/blitter. MUST use release
    // after done with the function.
    PixelPipeHandle     compilePixelPipeline(const PixelPipe::SignatureState& state);
    BlitterHandle       compileBlitter(const DynamicBlitter::BlitSignatureState& state);

    PixelPipeFunction   getPixelPipePtr(PixelPipeHandle handle);
    BlitterFunction     getBlitterPtr(BlitterHandle handle);

    void releasePixelPipeline(PixelPipeHandle handle);
    void releaseBlitter(BlitterHandle handle);

    bool init();

    // It seems that under VS, the static init order is not correct so the compiler has to be created
    // during run-time.
    static PPCompiler&  getCompiler() { if(!s_compiler) { s_compiler = new PPCompiler(); } return *s_compiler; }

private:
    bool initPPContext(PPCompilerContext& context, const unsigned char* data, size_t dataSize, const char* functionName);
    //void* compileRenderingFunction(const void* signatureState, RenderingFunctionType type);

    static ::llvm::Constant* createConstantStruct(const void* structure, size_t structureSize, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType);
    static ::llvm::Constant* createPPConstant(const void* signatureState, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType);
    static ::llvm::Constant* createBlitterConstant(const void* signatureState, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType);
    static ::llvm::Function* findFunctionWithString(::llvm::Module* module, const char* namepart);
    static void llvmCheckPtrError(const void* ptr, std::string& err);

    static std::string stringOfArray(const RIuint32* arr, int nElems);

private:
    //::llvm::LLVMContext& getLLVMContext() { return ::llvm::getGlobalContext(); }
    ::llvm::LLVMContext& getLLVMContext() { return m_llvmContext; }

    // The order is important atm. because llvm context must be destroyed last:
    //::llvm::LLVMContext& m_llvmContext;
    PPCompilerContext m_blitterContext;
    PPCompilerContext m_ppContext;
    ::llvm::ExecutionEngine* m_executionEngine;

    // \note Loading a system with LLVM already consumes a lot of memory, so
    // the amount of cached functions can be grown substantially depending on
    // requirements.
    enum { NUM_CACHED_PIXELPIPES = 64 };
    enum { NUM_CACHED_BLITTERS = NUM_CACHED_PIXELPIPES };
    
    FunctionCache<PixelPipeHash> m_ppCache;
    FunctionCache<BlitterHash> m_blitterCache;
    typedef FunctionCache<PixelPipeHash>::EntryHandle PixelPipeEntryHandle;
    typedef FunctionCache<BlitterHash>::EntryHandle BlitterEntryHandle;
    //std::vector<CacheEntry<BlitterHash> > blitterCache;
    
    ::llvm::LLVMContext m_llvmContext;

    static PPCompiler* s_compiler;
};

}


#endif

