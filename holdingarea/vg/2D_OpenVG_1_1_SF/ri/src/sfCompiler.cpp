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
#   include "sfCompiler.h"
#endif

#ifndef __RIPIXELPIPE_H
#   include "riPixelPipe.h"
#endif

#include <iostream>
#include <sstream>
#include <stdio.h>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Type.h"
#include "llvm/Value.h"
#include "llvm/Constant.h"
#include "llvm/Constants.h"
#include "llvm/Argument.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Support/StandardPasses.h"
#include "llvm/Transforms/Utils/BasicInliner.h"

#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Support/ManagedStatic.h"

// This file is found as an output of compilation (in the binary directory).
// Rationale for this is that the output is somewhat platform dependent.
#include "binaryPixelPipe.h"
#include "binaryBlitter.h"

namespace OpenVGRI {

static bool single_compiler = true;

PPCompiler* PPCompiler::s_compiler = NULL;

PPCompiler::PPCompiler() :
    m_executionEngine(NULL),
    m_ppCache(NUM_CACHED_PIXELPIPES),
    m_blitterCache(NUM_CACHED_BLITTERS),
    m_llvmContext()
{
    // \todo Make this class into a singleton?
    RI_ASSERT(single_compiler);

    if (single_compiler)
        single_compiler = false;

    ::llvm::InitializeNativeTarget();
    init();
}

PPCompiler::~PPCompiler()
{
    if (m_executionEngine)
        delete m_executionEngine;
}

PPCompiler::PPCompilerContext::PPCompilerContext() :
    module(NULL),
    llvmFunction(NULL)
{
}

PPCompiler::PPCompilerContext::~PPCompilerContext()
{
    // All the other objects should be owned by the executionengine, and
    // that is owned by the PPCompiler, so do nothing here.

}

bool PPCompiler::initPPContext(
    PPCompilerContext&      context, 
    const unsigned char*    data,
    size_t                  dataSize,
    const char*             functionName)
{
    try {
        std::string err;

        const char *byteCodeStart = (const char*)data;
        const char *byteCodeEnd = (const char*)(data + dataSize);

        ::llvm::MemoryBuffer* memBuffer = ::llvm::MemoryBuffer::getMemBufferCopy(byteCodeStart, byteCodeEnd);
        
        llvmCheckPtrError((void*)memBuffer, err);

        // Make sure the module is fully read:
        //::llvm::Module* ppModule = moduleProvider->materializeModule(&err);
        ::llvm::Module* ppModule = ::llvm::getLazyBitcodeModule(memBuffer, getLLVMContext(), &err);
        ppModule->MaterializeAll();
        
        llvmCheckPtrError(ppModule, err);
        context.module = ppModule;
        //ppModule->dump();

        if (!m_executionEngine)
        {
            m_executionEngine = ::llvm::ExecutionEngine::createJIT(ppModule, &err, NULL, ::llvm::CodeGenOpt::Aggressive);
            llvmCheckPtrError(m_executionEngine, err);
        }
        else
            m_executionEngine->addModule(ppModule);

        //m_executionEngine->runStaticConstructorsDestructors(false);

        ::llvm::Function* originalFunc = findFunctionWithString(ppModule, functionName);
       
        llvmCheckPtrError((const void*)originalFunc, err);
        context.llvmFunction = originalFunc;
        //originalFunc->dump();
    } catch (int err)
    {
        (void)err;
        std::cerr << "*** Failed to initialize a pixelpipeline module." << std::endl;
        return false;
    }

    return true;
}


/**
 * \brief   Loads the modules this compiler supports.
 */
bool PPCompiler::init()
{
    if (!initPPContext(m_ppContext, integerPixelPipe_binary, sizeof(integerPixelPipe_binary), "executePixelPipeline"))
        return false;
    
    if (!initPPContext(m_blitterContext, integerBlitter_binary, sizeof(integerBlitter_binary), "executeBlitter"))
        return false;

    m_ppCache.setLLVMInterface(m_executionEngine, m_ppContext.module);
    m_blitterCache.setLLVMInterface(m_executionEngine, m_blitterContext.module);

    return true;
}

// Helps cast void* to function pointers (may be platform dependent, usually works):
union FuncCaster {
    void*               ptr;
    PixelPipeFunction   pipeFunc;
    BlitterFunction     blitFunc; 
};

PPCompiler::PartialEvalFunc PPCompiler::compilePixelPipeline(::llvm::LLVMContext& llvmContext, PPCompilerContext& compilerContext, ConstantGenFunc constGenFunc, const void* state, const std::string& newFunctionName)
{
    // Get the constant state argument (must be the first in the list):
    ::llvm::Function::arg_iterator argit = compilerContext.llvmFunction->arg_begin();
    ::llvm::Argument& arg = *argit;
    //arg.getType()->dump();

    ::llvm::Constant* constantState = constGenFunc(state, llvmContext, arg.getType()->getContainedType(0));

    RI_ASSERT(constantState);

    //constantState->dump();

    // \todo Even though this variable could be automatically cleaned up by the optimizations,
    // it should be stored in the cache and destroyed along with the function when
    // the cache-entry is dropped out.
    ::llvm::GlobalVariable* constantStateVariable = new ::llvm::GlobalVariable(
        *compilerContext.module, constantState->getType(), true, ::llvm::GlobalValue::PrivateLinkage, constantState, newFunctionName + "_constState");

    // When to delete the global variable?
    // -> When the pixel-pipe is removed from the cache. Also, when the compiler is deleted.

    ::llvm::DenseMap<const ::llvm::Value*, ::llvm::Value*> valueMap;
    std::pair<const ::llvm::Value*, ::llvm::Value*> valueReplacement((::llvm::Value*)&arg, constantStateVariable);
    valueMap.insert(valueReplacement);

    ::llvm::Function* specializedFunc = ::llvm::CloneFunction(compilerContext.llvmFunction, valueMap, NULL);
    specializedFunc->setName(newFunctionName);
    //specializedFunc->dump();

    compilerContext.module->getFunctionList().push_back(specializedFunc);

    // \note Currently this creates the pass manager every time a pipeline is compiled...
    ::llvm::PassManager pm;
    // \todo Seems like the greater the threshold, the more the pass will inline:
    // In practice, everything should be inlineed into the resulting pipe.
    ::llvm::Pass* inliningPass = ::llvm::createFunctionInliningPass(100000);
    ::llvm::createStandardModulePasses(&pm, 3, false, true, true, true, false, inliningPass);
    pm.run(*compilerContext.module);

    //ppModule->dump();

    //compilerContext.llvmFunction->dump();
    PartialEvalFunc ret = {specializedFunc, constantStateVariable};
    return ret;
}

/**
 * \brief   Compile a pipeline if necessary. If the pipeline is cached, return a
 *          handle to it. 
 * \return  NULL handle if operation was not successful, otherwise a handle to the pipeline.
 * \todo    For deferred JITting, it might make sense to return a special value
 *          to indicate that the pipeline is under compilation.
 * \note    LLVM compilation should be thread-safe internally.
 */
PPCompiler::PixelPipeHandle PPCompiler::compilePixelPipeline(const PixelPipe::SignatureState& state)
{
    PixelPipeHash hash;
    calculatePPHash(hash, state);

    PixelPipeEntryHandle entry = m_ppCache.findCachedItemByHash(hash);

    if (!entry)
    {
        PartialEvalFunc pf;
        std::string funcName = "compiledPPFunc_" + PPCompiler::stringOfArray(hash.value, sizeof(hash.value)/sizeof(hash.value[0]));
        pf = compilePixelPipeline(getLLVMContext(), m_ppContext, createPPConstant, (const void*)&state, funcName.c_str());
        entry = m_ppCache.cacheFunction(hash, pf.llvmFunc, pf.llvmConst);
#if defined(RI_DEBUG) && 0
        pf.llvmFunc->dump();
#endif
    }

    if (!entry)
    {
        RI_ASSERT(false);
        // Release the function!
    }

    return (PixelPipeHandle)entry;
}

PPCompiler::BlitterHandle PPCompiler::compileBlitter(const DynamicBlitter::BlitSignatureState& state)
{
    // \todo This function is almost the same as compilePixelPipeline.
    BlitterHash hash;
    DynamicBlitter::calculateHash(hash, state);

    BlitterEntryHandle entry = m_blitterCache.findCachedItemByHash(hash);

    if (!entry)
    {
        PartialEvalFunc pf;
        std::string funcName = "compiledBlitterFunc_" + PPCompiler::stringOfArray(hash.value, sizeof(hash.value)/sizeof(hash.value[0]));
        pf = compilePixelPipeline(getLLVMContext(), m_blitterContext, createBlitterConstant, (const void*)&state, funcName.c_str());
        entry = m_blitterCache.cacheFunction(hash, pf.llvmFunc, pf.llvmConst);
    }

    if (!entry)
    {
        RI_ASSERT(false);
        // Release the function!
    }

    RI_ASSERT(entry);

    return (BlitterHandle)entry;
}

PixelPipeFunction PPCompiler::getPixelPipePtr(PixelPipeHandle handle)
{
    PixelPipeEntryHandle entryHandle = (PixelPipeEntryHandle)handle;
    ::llvm::Function* function = m_ppCache.getFunction(entryHandle); 

    FuncCaster c;
    c.ptr = m_executionEngine->getPointerToFunction(function);
    return c.pipeFunc;
}

BlitterFunction PPCompiler::getBlitterPtr(BlitterHandle handle)
{
    BlitterEntryHandle entryHandle = (BlitterEntryHandle)handle;
    ::llvm::Function* function = m_blitterCache.getFunction(entryHandle);

    FuncCaster c;
    c.ptr = m_executionEngine->getPointerToFunction(function);
    return c.blitFunc;
}

// Release a reference to a pixel-pipeline handle.
void PPCompiler::releasePixelPipeline(PixelPipeHandle handle)
{
    PixelPipeEntryHandle entryHandle = (PixelPipeEntryHandle)handle;
    m_ppCache.releaseEntry(entryHandle);
}

// Release a reference to a blitter handle.
void PPCompiler::releaseBlitter(BlitterHandle handle)
{
    BlitterEntryHandle entryHandle = (BlitterEntryHandle)handle;
    m_blitterCache.releaseEntry(entryHandle);
}

#if 0
// \todo Complete this and make it fully recursive: this way all the C-structs
// can be converted to corresponding LLVM classes at runtime.
::llvm::Constant* PPCompiler::createConstantStruct(const void* structure, size_t structSize, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType) const
{
    // Only copies integer-values (int, bools)
    RIuint8* bytePtr = (RIuint8*)structure; 
    ::llvm::Type::subtype_iterator subtypeBegin, subtypeEnd;

    subtypeBegin = structType->subtype_begin();
    subtypeEnd = structType->subtype_end();

    for(::llvm::Type::subtype_iterator it = subtypeBegin; it != subtypeEnd; ++it)
    {
        ::llvm::Type* elementType = *it;
        RI_ASSERT(elementType->isSized());
        unsigned int elementBitSize = elementType->getPrimitiveSizeInBits();
        RI_ASSERT((elementBitSize % 8) == 0);
        unsigned int elementSize = elementBitSize / 8;
        RI_ASSERT(elementSize > 0 && elementSize <= 4); 
       
        bytePtr += elementSize;
    }
}
#endif
/**
 * \brief   Creates a LLVM constant from a color descriptor.
 * \todo    Create global constants from these for every possible color format!
 */
RI_INLINE static ::llvm::Constant* createDescConstant(const Color::Descriptor& desc, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType)
{
    ::llvm::Constant* constants[] = {
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.redBits, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.redShift, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.greenBits, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.greenShift, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.blueBits, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.blueShift, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.alphaBits, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.alphaShift, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.luminanceBits, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.luminanceShift, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.shape, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.vgFormat, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.internalFormat, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.bitsPerPixel, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.bytesPerPixel, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.maskBits, true),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), desc.maskShift, true),
    };

    std::vector< ::llvm::Constant*> structConsts;

    for (size_t i = 0; i < sizeof(constants)/sizeof(constants[0]); i++)
    {
        structConsts.push_back(constants[i]);
    }

    ::llvm::Constant* constStruct = ::llvm::ConstantStruct::get((::llvm::StructType*)structType, structConsts);

    return constStruct;
}

::llvm::Constant* PPCompiler::createPPConstant(const void* signatureState, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType)
{
    // Get the actual object. This is a messy way to abstract setting the structure, but
    // there is little sense in making more classes/adapters/whatever at this point.
    const PixelPipe::SignatureState& state = *(const PixelPipe::SignatureState*)signatureState;
    // \todo There seems to be no way to track the structure member names once
    // LLVM has generated the bitcode (only offsets and references to those
    // offsets remain). Means to track proper setting of struct members in
    // DEBUG build must be implemented in some other way.
    // 
    // For now, the state and this function must be kept carefully in sync!
    
    const int cDescriptorElementIndex = 8;
    const ::llvm::Type* descType = structType->getContainedType(cDescriptorElementIndex);

    ::llvm::Constant* constants[] = {
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.blendMode, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.imageMode, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.paintType, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.maskOperation, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.paintTilingMode, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.paintSampler, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.imageSampler, false),

        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.imageGradientType, false),

        createDescConstant(state.dstDesc, llvmContext, descType),
        createDescConstant(state.maskDesc, llvmContext, descType),
        createDescConstant(state.imageDesc, llvmContext, descType),
        createDescConstant(state.patternDesc, llvmContext, descType),

        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.hasMasking, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.hasImage, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.hasColorTransform, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.isRenderToMask, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.fillColorTransparent, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.unsafeImageInput, false),
    };

    std::vector< ::llvm::Constant*> structConsts;

    for (size_t i = 0; i < sizeof(constants)/sizeof(constants[0]); i++)
    {
        structConsts.push_back(constants[i]);
    }

    ::llvm::Constant* constStruct = ::llvm::ConstantStruct::get((::llvm::StructType*)structType, structConsts);

    return constStruct;
}

::llvm::Constant* PPCompiler::createBlitterConstant(const void* signatureState, ::llvm::LLVMContext& llvmContext, const ::llvm::Type* structType)
{
    const DynamicBlitter::BlitSignatureState& state = *(const DynamicBlitter::BlitSignatureState*)signatureState;
    
    const int cDescriptorElementIndex = 4;
    const ::llvm::Type* descType = structType->getContainedType(cDescriptorElementIndex);

    ::llvm::Constant* constants[] = {
        ::llvm::ConstantInt::get(::llvm::Type::getInt32Ty(llvmContext), state.maskOperation, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.incompatibleStrides, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.isMaskOperation, false),
        ::llvm::ConstantInt::get(::llvm::Type::getInt8Ty(llvmContext), state.unsafeInput, false),

        createDescConstant(state.srcDesc, llvmContext, descType),
        createDescConstant(state.dstDesc, llvmContext, descType),
    };

    std::vector< ::llvm::Constant*> structConsts;

    for (size_t i = 0; i < sizeof(constants)/sizeof(constants[0]); i++)
    {
        structConsts.push_back(constants[i]);
    }

    ::llvm::Constant* constStruct = ::llvm::ConstantStruct::get((::llvm::StructType*)structType, structConsts);

    return constStruct;
}

/**
 * \brief   Find a function whose name containst ``namepart''. Useful for finding c++
 *          decorated names from modules (if they are unique).
 * \note    Maybe should use C-functions only in the final product.
 */
::llvm::Function* PPCompiler::findFunctionWithString(::llvm::Module* module, const char* namepart)
{
    // Find a function whose name contains 'namepart'
    llvm::Module::iterator it, end;
    it = module->begin();
    end = module->end();
    for (; it != end; ++it)
    {
        if (!it->isDeclaration())
            if (it->getNameStr().find(namepart) != std::string::npos)
                return it;
    }
    return NULL;
}

void PPCompiler::llvmCheckPtrError(const void* ptr, std::string& err)
{
#if defined(RI_DEBUG)
    (void)err;
    //std::cout << "LLVM returned: " << ptr << ". Error string: \"" << err << "\"" << std::endl;
#else
    (void)err;
#endif
    if (!ptr)
        throw(-1);
}

/*static*/ std::string PPCompiler::stringOfArray(const RIuint32* arr, int nElems)
{
    char tempBuffer[9];
    std::stringstream out;
    // Ambiquities resolved by using a constant width:
    RI_ASSERT(nElems > 0);
    int i = nElems-1;
    do {
        snprintf(tempBuffer, sizeof(tempBuffer), "%08x", arr[i]);
        out << tempBuffer;
        i--;
    } while(i >= 0);

    return out.str();
}

}

