#include "Arena.h"

Result Arena_init(Arena *arena, size_t capacity,
                  const Allocator *const pAllocator) {
  Result result = {0};
  uint8_t *buffer = TALLOCATE(uint8_t, capacity);
  if (!buffer) {
    return Result_create(CONTEXT_GENERAL, GENERAL_NULL_ALLOCATION);
  }

  arena->buffer = &buffer[0];
  arena->offset = 0;
  arena->capacity = capacity;
  return result;
}
void Arena_release(Arena *arena, const Allocator *const pAllocator) {
  DEALLOCATE(arena->buffer);
  arena->buffer = NULL;
  arena->offset = 0;
  arena->capacity = 0;
}

void *arena_allocate(void *p_userData, size_t size, size_t alignment) {
  Arena *arena = (Arena *)p_userData;

  uintptr_t currentAddress = (uintptr_t)(arena->buffer + arena->offset);
  uintptr_t alignedAddress =
      (currentAddress + (alignment - 1)) & ~(alignment - 1);

  size_t padding = alignedAddress - currentAddress;

  size_t totalSize = size + padding;

  if (arena->offset + totalSize > arena->capacity) {
    return NULL;
  }

  void *ptr = (void *)alignedAddress;
  arena->offset += totalSize;

  return ptr;
}
void arena_deallocate(void *p_userData, void *ptr) {}

Allocator Arena_getAllocator(Arena *arena) {
  return (Allocator){
      .pfnAllocate = arena_allocate,
      .pfnDeallocate = arena_deallocate,
      .pUserData = (void *)arena,
  };
}
