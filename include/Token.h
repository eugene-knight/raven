#ifndef TOKEN_H
#define TOKEN_H

#include "Allocator.h"
#include "Result.h"
#include "String.h"

#define TOKEN_TYPE__                                                           \
  X(TOKEN_TYPE_UNDEFINED)                                                      \
  X(TOKEN_TYPE_IDENTIFIER)                                                     \
  X(TOKEN_TYPE_COLON)                                                          \
  X(TOKEN_TYPE_COLON_COLON)                                                    \
  X(TOKEN_TYPE_LEFT_PARENTHESIS)                                               \
  X(TOKEN_TYPE_RIGHT_PARENTHESIS)                                              \
  X(TOKEN_TYPE_COMMA)                                                          \
  X(TOKEN_TYPE_MINUS)                                                          \
  X(TOKEN_TYPE_MINUS_GREATER_THAN)                                             \
  X(TOKEN_TYPE_EQUAL)                                                          \
  X(TOKEN_TYPE_DIGIT)                                                          \
  X(TOKEN_TYPE_SEMICOLON)                                                      \
  X(TOKEN_TYPE_KEYWORD_END)                                                    \
  X(TOKEN_TYPE_END_OF_FILE)

typedef uint32_t Token_Type;
enum {
#define X(ENUM) ENUM,
  TOKEN_TYPE__
#undef X
      TOKEN_TYPE_MAX,
};

typedef struct Token {
  char *ptr;
  size_t length;
  Token_Type type;
} Token;

typedef struct Token_Proxy {
  char **ptr;
  size_t *length;
  Token_Type *type;
} Token_Proxy;

typedef struct Token_Stream {
  char **ptr;
  size_t *length;
  Token_Type *type;
  size_t count;
  size_t capacity;
} Token_Stream;
Result Token_Stream_init(Token_Stream *stream, size_t capacity,
                         const Allocator *const pAllocator);
void Token_Stream_release(Token_Stream *stream,
                          const Allocator *const pAllocator);

const char *const Token_getType(Token_Type type);

String_View String_View_fromTokenStream(Token_Stream *stream, size_t index);

#endif
