#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t Byte;

typedef void *(*PFN_allocate)(void *pUserData, size_t size, size_t alignment);
typedef void (*PFN_deallocate)(void *pUserData, void *ptr);

typedef struct Allocator {
  PFN_allocate pfnAllocate;
  PFN_deallocate pfnDeallocate;
  void *pUserData;
} Allocator;

const Allocator *getDefaultAllocator(void); 

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
  (SELF)->pfnAllocate((SELF)->pUserData, (SIZE), (ALIGNMENT))
#define ALLOCATE(SIZE, ALIGNMENT) ALLOCATE_(pAllocator, (SIZE), (ALIGNMENT))

#define TALLOCATE_(SELF, TYPE, COUNT)                                          \
  (SELF)->pfnAllocate((SELF)->pUserData, (sizeof(TYPE) * (COUNT)),             \
                      ALIGNOF(TYPE))
#define TALLOCATE(TYPE, COUNT) TALLOCATE_(pAllocator, TYPE, COUNT)

#define PALLOCATE(SIZE, ALIGNMENT)                                             \
  ALLOCATE_(params->pAllocator, (SIZE), (ALIGNMENT))
#define PTALLOCATE(TYPE, COUNT) TALLOCATE_(params->pAllocator, TYPE, COUNT)

#define DEALLOCATE_(SELF, MEM) SELF->pfnDeallocate(SELF->pUserData, (MEM))
#define DEALLOCATE(MEM) DEALLOCATE_(pAllocator, (MEM))
#define PDEALLOCATE(MEM) DEALLOCATE_(params->pAllocator, (MEM))

#endif
