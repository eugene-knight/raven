#ifndef STRING_H
#define STRING_H

#include "Allocator.h"
#include "Result.h"

typedef struct String {
  char *ptr;
  size_t length;
  size_t capacity;
} String;
NODISCARD
Result String_init(String *string, size_t capacity,
                   const Allocator *const pAllocator);
void String_release(String *string, const Allocator *const pAllocator);

NODISCARD
Result String_copyS(String *string, char *src, size_t length);
NODISCARD
Result String_copyA(String *string, char *src, size_t length);

NODISCARD
const char *String_getPtr(String *string);
NODISCARD
size_t String_getLength(String *string);
NODISCARD
size_t String_getCapacity(String *string);

#endif
