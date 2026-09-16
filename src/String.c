#include "String.h"
#include <string.h>

Result String_init(String *string, size_t capacity,
                   const Allocator *const pAllocator) {
  Result result = {0};
  Byte *pMem = TALLOCATE(char, capacity);
  if (!pMem) {
    result.code = GENERAL_NULL_ALLOCATION;
    result.context = CONTEXT_GENERAL;
    return result;
  }
  string->ptr = (char *)&pMem[0];
  string->length = 0;
  string->capacity = capacity;
  return result;
}

static inline Bool isExceedCapacity(String *string, size_t length) {
  return !(length <= string->capacity);
}
/// @brief Memory copy at start (i.e Index 0) with size N
static inline void copySN(String *string, char *src, size_t size) {
  memcpy(&string->ptr[0], src, size);
}

Result String_copyS(String *string, char *src, size_t length) {
  if (isExceedCapacity(string, length)) {
    return Result_create(CONTEXT_STRING_TYPE, STRING_TYPE_COPY_EXCEED_CAPACITY);
  }
  copySN(string, src, length);
  string->length = length;
  assert(string->length <= string->capacity);
  return Result_createSuccess();
}

void String_release(String *string, const Allocator *const pAllocator) {
  DEALLOCATE(string->ptr);
  string->ptr = NULL;
  string->length = string->capacity = 0;
}

const char *String_getPtr(String *string) { return string->ptr; }
size_t String_getLength(String *string) { return string->length; }
size_t String_getCapacity(String *string) { return string->capacity; }
