#include "String.h"
#include <string.h>

// Constant pointer to a constant allocator
typedef const Allocator *const _Allocator;

Result String_init(String *out, size_t capacity, _Allocator pAllocator) {
  Result result = {0};

  void *mem = TALLOCATE(char, capacity + 1);
  CHKALLOCR(mem);

  out->ptr = (char *)mem;
  out->length = 0;
  out->capacity = capacity;

  out->ptr[out->capacity] = '\0';

  return result;
}

String String_ownCstr(char *src, size_t length, size_t capacity) {
    return (String){
        .ptr = &src[0],
        .length = length,
        .capacity = capacity
    };
}

void String_release(String *in, _Allocator pAllocator) {
  DEALLOCATE(in->ptr);
  in->ptr = NULL;
  in->length = in->capacity = 0;
}

Result String_copys(String *in, char *src, size_t length) {
  Result result = {0};

  if (length > in->capacity) {
    result.ctx = CTX_STRING_TYPE;
    result.code = STRING_TYPE_COPY_OUT_OF_BOUNDS_S;
    goto _result;
  }

  memcpy(in->ptr, src, length);
  in->length = length;

_result:
  return result;
}

Result String_copyi(String *in, char *src, size_t length, size_t index) {
  Result result = {0};

  if (index >= in->capacity) {
    result.ctx = CTX_STRING_TYPE;
    result.code = STRING_TYPE_COPY_OUT_OF_BOUNDS_I;
    goto _result;
  }
  size_t newLength = in->length + length;
  if (newLength > in->capacity) {
    result.ctx = CTX_STRING_TYPE;
    result.code = STRING_TYPE_COPY_OUT_OF_BOUNDS_S;
    goto _result;
  }

  memcpy(&in->ptr[index], src, length);

_result:
  return result;
}

Result String_copya(String *in, char *src, size_t length) {
  Result result = {0};

  size_t newLength = in->length + length;
  if (newLength > in->capacity) {
    result.ctx = CTX_STRING_TYPE;
    result.code = STRING_TYPE_COPY_OUT_OF_BOUNDS_S;
    goto _result;
  }

  memcpy(&in->ptr[in->length], src, length);

_result:
  return result;
}

void String_move(String *dst, String *src) {
  dst->ptr = src->ptr;
  dst->length = src->length;
  dst->capacity = src->capacity;
  src->ptr = NULL;
  src->length = 0;
  src->capacity = 0;
}
const char *const String_data(String *in) { return &in->ptr[0]; }
size_t String_length(String *in) { return in->length; }
size_t String_capacity(String *in) { return in->capacity; }

String_View String_View_fromString(String *dst) {
    return (String_View){
        .ptr = dst->ptr,
        .length = dst->length,
    };
}

String_View String_View_fromChar(char *src, size_t length) {
    return (String_View){
        .ptr = src,
        .length = length,
    };
}
