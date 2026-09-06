#ifndef ARENA_H
#define ARENA_H

#include "Result.h"
#include "Allocator.h"

typedef struct Arena {
    Byte *buffer;
    size_t offset;
    size_t capacity;
} Arena;

Result Arena_init(Arena *arena, size_t capacity, const Allocator *const pAllocator);
void Arena_release(Arena* arena, const Allocator *const pAllocator);

Allocator Arena_getAllocator(Arena *arena);


#endif
