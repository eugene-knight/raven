#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t Byte;

typedef void *(*PFN_allocate)(void *p_userData, size_t size, size_t alignment);
typedef void (*PFN_deallocate)(void *p_userData, void *ptr);

typedef struct Allocator {
  PFN_allocate pfn_allocate;
  PFN_deallocate pfn_deallocate;
  void *p_userData;
} Allocator;

/// Constant pointer to a constant `Allocator`
typedef const Allocator *const cpc_Allocator;

extern const Allocator g_defaultAllocator;

#if defined(__GNUC__) || defined(__clang__)
#define ALIGNOF(type) __alignof__(type)
#elif defined(_MSC_VER)
#define ALIGNOF(type) __alignof(type)
#else
/* Fallback C89/C99 offsetof trick */
#define ALIGNOF(type)                                                          \
  offsetof(                                                                    \
      struct {                                                                 \
        char c;                                                                \
        type member;                                                           \
      },                                                                       \
      member)

#endif
#define ALLOCATE_(SELF, SIZE, ALIGNMENT)                                       \
  (SELF)->pfn_allocate((SELF)->p_userData, (SIZE), (ALIGNMENT))
#define ALLOCATE(SIZE, ALIGNMENT) ALLOCATE_(pAllocator, (SIZE), (ALIGNMENT))

#define TALLOCATE_(SELF, TYPE, COUNT)                                          \
  (SELF)->pfn_allocate((SELF)->p_userData, (sizeof(TYPE) * (COUNT)),           \
                       ALIGNOF(TYPE))
#define TALLOCATE(TYPE, COUNT) TALLOCATE_(pAllocator, TYPE, COUNT)

#define PALLOCATE(SIZE, ALIGNMENT)                                             \
  ALLOCATE_(params->pAllocator, (SIZE), (ALIGNMENT))
#define PTALLOCATE(TYPE, COUNT) TALLOCATE_(params->pAllocator, TYPE, COUNT)

#define DEALLOCATE_(SELF, MEM) SELF->pfn_deallocate(SELF->p_userData, (MEM))
#define DEALLOCATE(MEM) DEALLOCATE_(pAllocator, (MEM))
#define PDEALLOCATE(MEM) DEALLOCATE_(params->pAllocator, (MEM))

#endif
