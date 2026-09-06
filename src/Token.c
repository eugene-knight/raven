#include "Token.h"

const char *const Token_getType(Token_Type type) {
  switch (type) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    return #ENUM;
    TOKEN_TYPE__
#undef X
  }
  return "TOKEN_TYPE_UNKNOWN";
}

Result Token_Stream_init(Token_Stream *stream, size_t capacity,
                         const Allocator *const pAllocator) {
  Result result = {0};
  size_t charPtrBytes = capacity * sizeof(char *);
  size_t lengthBytes = capacity * sizeof(size_t);
  size_t typeBytes = capacity * sizeof(Token_Type);

  ///< No alignment done since a multiple of `sizeof(char*)` already
  ///< ensures that alignment is always satisfied
  size_t totalBytes = charPtrBytes + lengthBytes + typeBytes;

  void *buffer = TALLOCATE(Byte, totalBytes);
  CHKALLOCR(buffer);

  stream->ptr = &buffer[0];
  stream->length = &buffer[charPtrBytes];
  stream->type = &buffer[charPtrBytes + lengthBytes];
  stream->capacity = capacity;
  stream->count = 0;

  return result;
}
void Token_Stream_release(Token_Stream *stream,
                          const Allocator *const pAllocator) {
  DEALLOCATE(stream->ptr);
  stream->ptr = NULL;
  stream->length = NULL;
  stream->type = NULL;
}

String_View String_View_fromTokenStream(Token_Stream *stream, size_t index) {
  return (String_View){
      .ptr = stream->ptr[index],
      .length = stream->length[index],
  };
}
