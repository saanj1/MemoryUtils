#pragma once

/**
 * @file ObjectsMemoryPool.hpp
 * 
 * This module declares the ObjectsMemoryPool::ObjectsMemoryPool class.
 * © 2020 by Sanjeev
 */

#include <memory>
#include <MemoryUtils/BlockMemoryAllocator.hpp>
namespace MemoryUtils {
  /**
   * This class serves as a memory allocator at 
   * runtime to support zero-alloc in the hotpath.
   */
  template<class ObjectType, size_t NumberOfObjects = 16>
  class ObjectPool {
   public:
    //Life cycle methods
    ~ObjectPool() = default;
    MAKE_NONCOPYABLE(ObjectPool);
    MAKE_NONMOVABLE(ObjectPool);
   public:
    /**
     * This is the constructor
     */
    ObjectPool()
      : _m_mem_block_size(NumberOfObjects * sizeof(ObjectType)),
        _m_mem_block_allocator(
          std::make_unique<MemoryUtils::BlockMemoryAllocator>())				
    {
      static_assert(sizeof(ObjectType) > sizeof(ObjectType*));
      replenish();
    }
            
    /**
     * This function returns pointer to an already
     * allocated object
     * 
     * @return 
     *      pointer to an allocated object
     */
    FORCE_INLINE(ObjectType*) alloc() {
      if (_m_object_ptr == nullptr) {
          replenish();
      }
      ObjectType* retval = reinterpret_cast<ObjectType*>(_m_object_ptr);
      _m_object_ptr = _m_object_ptr->next;
      return retval;
    }

    /**
     * This function deallocates an object pointed by the
     * object pointer, technically it reuses the object
     * 
     * @param[in] __ptr
     *      Pointer to an object which needs to be deallocated
     * 
     */
    FORCE_INLINE(void) dealloc(ObjectType* __ptr) {
      reinterpret_cast<ObjectLink*>(__ptr)->next = _m_object_ptr;
      _m_object_ptr = reinterpret_cast<ObjectLink*>(__ptr);
    }

    /**
     * This function requests block memory allocator to allocate
     * block of memory and builds the list of objects
     */
    FORCE_NOINLINE(void) replenish() {
      char* start_addr = (char*)_m_mem_block_allocator->allocate(_m_mem_block_size);
      ObjectLink* curr_link = _m_object_ptr = reinterpret_cast<ObjectLink*>(start_addr);
      for(size_t sz = sizeof(ObjectType); sz < _m_mem_block_size; sz += sizeof(ObjectType)) {
        curr_link->next = reinterpret_cast<ObjectLink*>(start_addr + sz);
        curr_link = curr_link->next;
      }
    }

   private:
    /**
     * This is used stores the list of allocated objects pointers.
     */
    struct ObjectLink {
      /**
       * This is the pointet to the 
       * next object
       */
      ObjectLink *next;
    };
    
    /**
     * This is the size is requested from the block mem allocator
     */
    size_t _m_mem_block_size;

    /**
     * This stores the pointer which can be returned
     * for the next alloc call
     */
    ObjectLink *_m_object_ptr;

    /**
     * This is the pointer to the MemoryUtils::BlockMemoryAllocator
     */
    std::unique_ptr< BlockMemoryAllocator > _m_mem_block_allocator;
  };
}
