/*--------------------------------------------------------------------------------*
  Copyright (C)Nintendo All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *--------------------------------------------------------------------------------*/

#pragma once

#include <nn/hid.h>
#include <nn/nn_Log.h>
#include <nn/nn_Abort.h>
#include <nn/hid/hid_Npad.h>

namespace nn { namespace mem {
    class StandardAllocator;
}}

namespace nn { namespace audio {
    struct MemoryPoolType;
    struct AudioRendererConfig;
}}

namespace nns { namespace atk {

    void* Allocate(std::size_t size) NN_NOEXCEPT;
    void* Allocate(std::size_t size, nn::mem::StandardAllocator& allocator) NN_NOEXCEPT;
    void* Allocate(std::size_t size, int alignment, nn::mem::StandardAllocator& allocator) NN_NOEXCEPT;
    void* Allocate( std::size_t size, int alignment ) NN_NOEXCEPT;

    void Free(void* pMemory) NN_NOEXCEPT;
    void Free(void* pMemory, std::size_t size) NN_NOEXCEPT;
    void Free(void* pMemory, nn::mem::StandardAllocator& allocator) NN_NOEXCEPT;

    void InitializeHeap() NN_NOEXCEPT;
    void FinalizeHeap() NN_NOEXCEPT;

    void InitializeFileSystem() NN_NOEXCEPT;
    void FinalizeFileSystem() NN_NOEXCEPT;
    const char* GetAbsolutePath(const char* relativePath) NN_NOEXCEPT;

    void InitializeHidDevices() NN_NOEXCEPT;
    void FinalizeHidDevices() NN_NOEXCEPT;
    void UpdateHidDevices() NN_NOEXCEPT;


    void* GetPoolHeapAddress() NN_NOEXCEPT;
    size_t GetPoolHeapSize() NN_NOEXCEPT;
    void* AllocateForMemoryPool(std::size_t size) NN_NOEXCEPT;
    void* AllocateForMemoryPool(std::size_t size, int alignment) NN_NOEXCEPT;
    void FreeForMemoryPool(void* pMemory) NN_NOEXCEPT;

}}

