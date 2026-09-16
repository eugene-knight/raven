#include "Token.h"

Result Token_Stream_init(Token_Stream *stream, size_t capacity,
                         const Allocator *const pAllocator) {
  size_t charPtrBytes = sizeof(stream->ptr) * capacity;
  size_t lengthBytes = sizeof(stream->length) * capacity;
  size_t rowBytes = sizeof(stream->row) * capacity;
  size_t columnBytes = sizeof(stream->column) * capacity;
  size_t typeBytes = sizeof(stream->type) * capacity;
  size_t totalBytes =
      charPtrBytes + lengthBytes + rowBytes + columnBytes + typeBytes;

  Byte *pBuffer = TALLOCATE(Byte, totalBytes);
  if (!pBuffer) {
    return Result_create(CONTEXT_GENERAL, GENERAL_NULL_ALLOCATION);
  }

  stream->ptr = (char **)&pBuffer[0];
  stream->length = (size_t *)&pBuffer[charPtrBytes];
  stream->row = (size_t *)&pBuffer[charPtrBytes + lengthBytes];
  stream->column = (size_t *)&pBuffer[charPtrBytes + lengthBytes + rowBytes];
  stream->type = (Token_Type *)&pBuffer[charPtrBytes + lengthBytes + rowBytes +
                                        columnBytes];
  stream->count = 0;
  stream->capacity = capacity;

  uintptr_t charAddress = (uintptr_t)&pBuffer[0];
  uintptr_t lengthAddress = (uintptr_t)&pBuffer[charPtrBytes];
  uintptr_t rowAddress = (uintptr_t)&pBuffer[charPtrBytes + lengthBytes];
  uintptr_t columnAddress =
      (uintptr_t)&pBuffer[charPtrBytes + lengthBytes + rowBytes];
  uintptr_t typeAddresss =
      (uintptr_t)&pBuffer[charPtrBytes + lengthBytes + rowBytes + columnBytes];

  assert((charAddress % ALIGNOF(stream->ptr)) == 0);
  assert((lengthAddress % ALIGNOF(stream->length)) == 0);
  assert((rowAddress % ALIGNOF(stream->row)) == 0);
  assert((columnAddress % ALIGNOF(stream->column)) == 0);
  assert((typeAddresss % ALIGNOF(stream->type)) == 0);

  return Result_createSuccess();
}
void Token_Stream_release(Token_Stream *stream,
                          const Allocator *const pAllocator) {
  DEALLOCATE(stream->ptr);
  stream->ptr = NULL;
  stream->length = NULL;
  stream->type = NULL;
  stream->row = NULL;
  stream->column = NULL;
  stream->count = 0;
  stream->capacity = 0;
}

char *Token_Stream_getPtr(Token_Stream *stream, size_t index) {
  assert(index < stream->count);
  return stream->ptr[index];
}
size_t Token_Stream_getLength(Token_Stream *stream, size_t index) {
  assert(index < stream->count);
  return stream->length[index];
}
Token_Type Token_Stream_getType(Token_Stream *stream, size_t index) {
  assert(index < stream->count);
  return stream->type[index];
}

Token_Proxy Token_createProxy(char **ptrRef, size_t *lengthRef, size_t *rowRef,
                              size_t *columnRef, Token_Type *typeRef) {

  return (Token_Proxy){.ptr = ptrRef,
                       .length = lengthRef,
                       .row = rowRef,
                       .column = columnRef,
                       .type = typeRef};
}

void Token_setProxyNull(Token_Proxy *proxy) {
  static Token null = {
      .ptr = "\0",
      .length = 0,
      .row = 0,
      .column = 0,
      .type = TOKEN_TYPE_END_OF_FILE,
  };
  *(proxy->ptr) = null.ptr;
  *(proxy->length) = null.length;
  *(proxy->row) = null.row;
  *(proxy->column) = null.column;
  *(proxy->type) = null.type;
}
void Token_setProxyPtr(Token_Proxy *proxy, char *ptr) { *(proxy->ptr) = ptr; }
void Token_setProxyLength(Token_Proxy *proxy, size_t length) {
  *(proxy->length) = length;
}
void Token_setProxyType(Token_Proxy *proxy, Token_Type type) {
  *(proxy->type) = type;
}
void Token_setProxyRow(Token_Proxy *proxy, size_t row) { *(proxy->row) = row; }
void Token_setProxyColumn(Token_Proxy *proxy, size_t column) {
  *(proxy->column) = column;
}

char *Token_getProxyPtr(Token_Proxy *proxy) { return *(proxy->ptr); }
size_t Token_getProxyLength(Token_Proxy *proxy) { return *(proxy->length); }
size_t Token_getProxyRow(Token_Proxy *proxy) { return *(proxy->row); }
size_t Token_getProxyColumn(Token_Proxy *proxy) { return *(proxy->column); }
Token_Type Token_getProxyType(Token_Proxy *proxy) { return *(proxy->type); }

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
