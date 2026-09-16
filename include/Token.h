#ifndef TOKEN_H
#define TOKEN_H

#include "Allocator.h"
#include "Result.h"

#define TOKEN_TYPE__                                                           \
  X(TOKEN_TYPE_UNDEFINED)                                                      \
  X(TOKEN_TYPE_IDENTIFIER)                                                     \
  X(TOKEN_TYPE_COLON)                                                          \
  X(TOKEN_TYPE_COLON_COLON)                                                    \
  X(TOKEN_TYPE_COLON_EQUAL)                                                    \
  X(TOKEN_TYPE_LEFT_PARENTHESIS)                                               \
  X(TOKEN_TYPE_RIGHT_PARENTHESIS)                                              \
  X(TOKEN_TYPE_AT)                                                             \
  X(TOKEN_TYPE_STRING_LITERAL)                                                 \
  X(TOKEN_TYPE_COMMA)                                                          \
  X(TOKEN_TYPE_CARET)                                                          \
  X(TOKEN_TYPE_DOLLAR)                                                         \
  X(TOKEN_TYPE_ASTERISK)                                                       \
  X(TOKEN_TYPE_PLUS)                                                           \
  X(TOKEN_TYPE_MINUS)                                                          \
  X(TOKEN_TYPE_FORWARD_SLASH)                                                  \
  X(TOKEN_TYPE_MINUS_GREATER_THAN)                                             \
  X(TOKEN_TYPE_LESS_THAN_MINUS)                                                \
  X(TOKEN_TYPE_LESS_THAN)                                                      \
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
  size_t row;
  size_t column;
  Token_Type type;
} Token;

typedef size_t Token_Index;
typedef struct Token_Stream {
  char **ptr;
  size_t *length;
  size_t *row;
  size_t *column;
  Token_Type *type;

  size_t count;
  size_t capacity;
} Token_Stream;

typedef struct Token_Proxy {
  char **ptr;
  size_t *length;
  size_t *row;
  size_t *column;
  Token_Type *type;
} Token_Proxy;

Result Token_Stream_init(Token_Stream *stream, size_t capacity,
                         const Allocator *const pAllocator);
void Token_Stream_release(Token_Stream *stream,
                          const Allocator *const pAllocator);
char *Token_Stream_getPtr(Token_Stream *stream, size_t index);
size_t Token_Stream_getLength(Token_Stream *stream, size_t index);
Token_Type Token_Stream_getType(Token_Stream *stream, size_t index);

Token_Proxy Token_createProxy(char **ptrRef, size_t *lengthRef, size_t *rowRef,
                              size_t *columnRef, Token_Type *typeRef);

void Token_setProxyPtr(Token_Proxy *proxy, char *ptr);
void Token_setProxyLength(Token_Proxy *proxy, size_t length);
void Token_setProxyType(Token_Proxy *proxy, Token_Type type);
void Token_setProxyRow(Token_Proxy *proxy, size_t row);
void Token_setProxyColumn(Token_Proxy *proxy, size_t column);
void Token_setProxyNull(Token_Proxy *proxy);

char *Token_getProxyPtr(Token_Proxy *proxy);
size_t Token_getProxyLength(Token_Proxy *proxy);
size_t Token_getProxyRow(Token_Proxy *proxy);
size_t Token_getProxyColumn(Token_Proxy *proxy);
Token_Type Token_getProxyType(Token_Proxy *proxy);

const char *const Token_getType(Token_Type type);

#endif
