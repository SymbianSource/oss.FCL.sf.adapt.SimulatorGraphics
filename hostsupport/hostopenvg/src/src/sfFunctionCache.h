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

#ifndef __SFFUNCTIONCACHE_H
#define __SFFUNCTIONCACHE_H

// (LRU) Cache for compiled pixelpipe functions. Never takes ownership of
// any of the objects.
// \todo LRU might not be the best strategy or the best strategy might
// depend on the use-case -> create more of these.

#include "riArray.h"

#if defined(__unix__)
#   include <pthread.h>
#else
#   include <windows.h>
#endif

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Module.h"

namespace llvm {
    class Function;
}

namespace OpenVGRI {

template<class HashClass> class FunctionCache 
{
private:
    enum { IMPLEMENTATION_MAX_CACHE_ENTRIES = 1024 };
    //enum { MAX_GLOBAL_TIME = 10000};
    enum { MAX_GLOBAL_TIME = RI_UINT32_MAX };

    struct CacheEntry 
    {
        CacheEntry() : refCount(1) {}
        CacheEntry(HashClass aHash, ::llvm::Function* aFunc, ::llvm::GlobalVariable* aConst, RIuint32 theUT) : refCount(1) {hash = aHash; llvmFunction = aFunc; llvmConstant = aConst; ut = theUT;}
        bool operator==(const CacheEntry& rhs) const { return hash == rhs.hash; }
        bool operator<(const CacheEntry& rhs) const { return ut < rhs.ut; } // Sort by time of usage.

        HashClass           hash;
        ::llvm::Function*   llvmFunction;
        ::llvm::GlobalVariable*   llvmConstant;
        RIuint32            ut;
        RIint32             refCount;
    };

public:
    typedef CacheEntry* EntryHandle;

public:
    FunctionCache(int nMaxEntries) :
        m_time(0)
    {
        // Limit so that if the cache is too large, you must optimize the implementation.
        // Also note that the optimized pixel pipes are most likely small, so it would 
        // be better to have a fast cache and a lot of entries!
        // \note A simple optimization is to sort the usage time sort order and remove the last
        // item in the array (instead of the first).
        RI_ASSERT(nMaxEntries > 0 && nMaxEntries < IMPLEMENTATION_MAX_CACHE_ENTRIES); 
        m_nMaxEntries = nMaxEntries;
        m_entries.reserve(nMaxEntries);
    }

    ~FunctionCache() 
    {
        for (int i = 0; i < m_entries.size(); i++)
        {
            clearEntry(m_entries[i]);
        }
    }
    
    // This info is needed for the module to remove functions and deallocate executable
    // functions:
    void setLLVMInterface(::llvm::ExecutionEngine* ee, ::llvm::Module* module)
    {
        m_executionEngine = ee;
        m_module = module;
    }

    // \todo If we never need the entry index, the locking can be
    // simplified a lot.
    // Must lock the cache during this operation!
    EntryHandle findCachedItemByHash(const HashClass& hash)
    {
        acquireMutex();
        int i = findCachedItemIndexByHash(hash, true);
        if (i == -1)
        {
            releaseMutex();
            return NULL;
        }
        EntryHandle handle = &m_entries[i];
        releaseMutex();
        
        return handle;
    }

    /**
     * \brief   Caches a function. Sets the reference count to 1
     * \return  EntryHandle != NULL on success.
     * \todo    The cache must be locked during the operation.
     */
    EntryHandle cacheFunction(HashClass hash, ::llvm::Function* function, ::llvm::GlobalVariable* constant)
    {
        acquireMutex();
        RI_ASSERT(findCachedItemIndexByHash(hash) == -1);

        if (m_entries.size() == m_nMaxEntries)
        {
            if (!removeLRU())
            {
                releaseMutex();
                return NULL;
            }
        }

        m_entries.push_back(CacheEntry(hash, function, constant, m_time));
        
        RI_ASSERT(m_entries.size() > 0);
        EntryHandle ret = &m_entries[m_entries.size()-1];
        incrementGlobalTime();

        releaseMutex();
        return ret;
    }

    ::llvm::Function* getFunction(EntryHandle handle)
    {
        return handle->llvmFunction;
    }

    // \note Does not remove the function from cache!
    void releaseEntry(EntryHandle handle)
    {
        RI_ASSERT(handle->refCount > 0);
        handle->refCount--;
    }

private:
    void incrementGlobalTime()
    {
        m_time++;
        if (m_time == MAX_GLOBAL_TIME)
            rebaseUsageTime();
    }

    void incrementAccessTime(CacheEntry &entry)
    {
        entry.ut = m_time;
        incrementGlobalTime();
    }

    int findCachedItemIndexByHash(const HashClass& hash, bool reserve = false)
    {
        // \note Could just overload operator== from entry and use the Array.find function.
        for (int i = 0; i < m_entries.size(); i++)
        {
            if (m_entries[i].hash == hash)
            {
                if (reserve)
                {
                    incrementAccessTime(m_entries[i]);
                    m_entries[i].refCount++;
                }
                return i;
            }
        }
        return -1;
    }

    void clearEntry(CacheEntry& entry)
    {
        m_executionEngine->freeMachineCodeForFunction(entry.llvmFunction);
        entry.llvmFunction->eraseFromParent();
        //entry.llvmConstant->eraseFromParent();
    }

    /**
     * \return  true if LRU item was successfully removed, false otherwise.
     * \note    Could try other pipes, but it is unlikely that the cache gets filled
     *          so soon that the blit for the least recently used blit has not been
     *          released.
     * \todo    Implement drop of other cache-entries?
     */
    bool removeLRU() 
    {
        // \note This is pretty inefficient for many cache size:
        // After first LRU removal, the cache is almost sorted anyway, so
        // more efficient solution should be implemented.
        //
        m_entries.sort();

        if (m_entries[0].refCount > 0)
            return false;

        clearEntry(m_entries[0]);
        m_entries.remove(m_entries[0]);

        return true;
    }

    void rebaseUsageTime()
    {
        RIuint32 i;
        m_entries.sort();
        RI_ASSERT(m_entries.size() > 0);
        for(i = 0; i < (RIuint32)m_entries.size(); i++)
        {
            m_entries[i].ut = i;
        };
        m_time = i;
    }

    static void acquireMutex();
    static void releaseMutex();

private:
    ::llvm::Module              *m_module;
    ::llvm::ExecutionEngine     *m_executionEngine;

    RIuint32            m_time;
    Array <CacheEntry>  m_entries;
    int                 m_nMaxEntries;

    static bool             s_mutexInitialized;
#if defined(__unix__)
    static pthread_mutex_t  s_mutex;
#else
    static CRITICAL_SECTION s_mutex;
#endif
};

template<class HashClass>
bool FunctionCache<HashClass>::s_mutexInitialized = false;

#if defined(__unix__)
template<class HashClass>
pthread_mutex_t FunctionCache<HashClass>::s_mutex;
#else
template<class HashClass>
CRITICAL_SECTION FunctionCache<HashClass>::s_mutex;
#endif

template<class HashClass>
void FunctionCache<HashClass>::acquireMutex()
{
    if (!s_mutexInitialized)
    {
#if defined(__unix__)
        int ret;
        pthread_mutexattr_t attr;
        ret = pthread_mutexattr_init(&attr);	//initially not locked
        RI_ASSERT(!ret);	//check that there aren't any errors
        ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);	//count the number of recursive locks
        RI_ASSERT(!ret);	//check that there aren't any errors
        ret = pthread_mutex_init(&s_mutex, &attr);
        pthread_mutexattr_destroy(&attr);
        RI_ASSERT(!ret);	//check that there aren't more errors
#else
        ::InitializeCriticalSection(&s_mutex);
#endif
        s_mutexInitialized = true;
    }
#if defined(__unix__)
    int ret = pthread_mutex_lock(&s_mutex);
    RI_ASSERT(!ret);
#else
    ::EnterCriticalSection(&s_mutex);
#endif
}

template<class HashClass>
void FunctionCache<HashClass>::releaseMutex()
{
    RI_ASSERT(s_mutexInitialized);
#if defined(__unix__)
    int ret = pthread_mutex_unlock(&s_mutex);
    RI_ASSERT(!ret);
#else
    ::LeaveCriticalSection(&s_mutex);
#endif
}

}


#endif

