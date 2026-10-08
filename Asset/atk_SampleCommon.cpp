/*--------------------------------------------------------------------------------*
  Copyright (C)Nintendo All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *--------------------------------------------------------------------------------*/

#include "atk_SampleCommon.h"

#include <cstdio>
#include <nn/atk/atk_SoundSystem.h>
#include <nn/audio/audio_MemoryPool.h>
#include <nn/audio/audio_AudioRenderer.h>
#include <nn/os.h>
#include <nn/fs.h>
#include <nn/util/util_FormatString.h>

#include <nn/mem/mem_StandardAllocator.h>

#if defined(NN_BUILD_TARGET_PLATFORM_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <nn/nn_Windows.h>
#endif

#include <nn/hid/hid_KeyboardKey.h>
#include <nn/settings/settings_DebugPad.h>

namespace {

    // 拡張ヒープ
    const int MemoryHeapSize = 32 * 1024 * 1024;
    static char g_HeapMemory[ MemoryHeapSize ];
    nn::mem::StandardAllocator g_Allocator;

    // メモリプール用ヒープ
    const int MemoryPoolHeapSize = 32 * 1024 * 1024;
    NN_AUDIO_ALIGNAS_MEMORY_POOL_ALIGN char g_PoolHeapMemory[MemoryPoolHeapSize];
    nn::mem::StandardAllocator g_PoolHeapAllocator;

    // ファイルシステム関連
    const char MountName[] = "content";
    const int AbsolutePathMax = 128;
    char g_AbsolutePath[AbsolutePathMax];
    void* g_MountRomCacheBuffer = NULL;
}

namespace nns { namespace atk {

    void* Allocate(std::size_t size) NN_NOEXCEPT
    {
        return Allocate(size, g_Allocator);
    }

    void* Allocate(std::size_t size, nn::mem::StandardAllocator& allocator) NN_NOEXCEPT
    {
        void* pMemory = allocator.Allocate(size);
        NN_ABORT_UNLESS_NOT_NULL(pMemory);
        return pMemory;
    }

    void* Allocate(std::size_t size, int alignment) NN_NOEXCEPT
    {
        return Allocate(size, alignment, g_Allocator);
    }

    void* Allocate(std::size_t size, int alignment, nn::mem::StandardAllocator& allocator) NN_NOEXCEPT
    {
        void* pMemory = allocator.Allocate(size, alignment);
        NN_ABORT_UNLESS_NOT_NULL(pMemory);
        return pMemory;
    }

    void Free( void* pMemory ) NN_NOEXCEPT
    {
        Free(pMemory, g_Allocator);
    }

    void Free(void* pMemory, std::size_t size) NN_NOEXCEPT
    {
        NN_UNUSED(size);
        Free(pMemory, g_Allocator);
    }

    void Free( void* pMemory, nn::mem::StandardAllocator& allocator ) NN_NOEXCEPT
    {
        if (pMemory == nullptr)
        {
            return;
        }

        allocator.Free(pMemory);
        pMemory = nullptr;
    }

    void InitializeHeap() NN_NOEXCEPT
    {
        g_Allocator.Initialize(g_HeapMemory, sizeof(g_HeapMemory));
        g_PoolHeapAllocator.Initialize(g_PoolHeapMemory, sizeof(g_PoolHeapMemory));
    }

    void FinalizeHeap() NN_NOEXCEPT
    {
        g_PoolHeapAllocator.Finalize();
        g_Allocator.Finalize();
    }

    void InitializeFileSystem() NN_NOEXCEPT
    {
        nn::fs::SetAllocator(Allocate, Free);

        size_t cacheSize = 0;
        NN_ABORT_UNLESS_RESULT_SUCCESS(nn::fs::QueryMountRomCacheSize(&cacheSize));
        g_MountRomCacheBuffer = Allocate(cacheSize);
        NN_ABORT_UNLESS_NOT_NULL(g_MountRomCacheBuffer);

        NN_ABORT_UNLESS_RESULT_SUCCESS(
            nn::fs::MountRom( MountName, g_MountRomCacheBuffer, cacheSize )
        );
    }

    void FinalizeFileSystem() NN_NOEXCEPT
    {
        nn::fs::Unmount(MountName);

        Free(g_MountRomCacheBuffer);
        g_MountRomCacheBuffer = NULL;
    }

    const char* GetAbsolutePath(const char* relativePath) NN_NOEXCEPT
    {
        nn::util::SNPrintf(g_AbsolutePath, AbsolutePathMax, "%s:/%s", MountName, relativePath);
        return g_AbsolutePath;
    }


    void* GetPoolHeapAddress() NN_NOEXCEPT
    {
        return g_PoolHeapMemory;
    }

    size_t GetPoolHeapSize() NN_NOEXCEPT
    {
        return sizeof(g_PoolHeapMemory);
    }

    void* AllocateForMemoryPool(std::size_t size) NN_NOEXCEPT
    {
        return Allocate(size, g_PoolHeapAllocator);
    }

    void* AllocateForMemoryPool(std::size_t size, int alignment) NN_NOEXCEPT
    {
        return Allocate(size, alignment, g_PoolHeapAllocator);
    }

    void FreeForMemoryPool(void* pMemory) NN_NOEXCEPT
    {
        return Free(pMemory, g_PoolHeapAllocator);
    }
}}
