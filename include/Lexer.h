#ifndef LEXER_H
#define LEXER_H

#include "Allocator.h"
#include "Result.h"
#include "Token.h"

#define CHAR_CLASS__                                                           \
  X(CHAR_CLASS_UNDEFINED)       /* Used for assertions */                      \
  X(CHAR_CLASS_LETTER)          /* [A-z][a-z] and '_'*/                        \
  X(CHAR_CLASS_DIGIT)           /* [0-9] */                                    \
  X(CHAR_CLASS_SYMBOL)          /* All punctuations and symbols (except '_')*/ \
  X(CHAR_CLASS_WHITESPACE)      /* All whitespace characters */                \
  X(CHAR_CLASS_NULL_TERMINATOR) /* Null terminator */

#define LEXER_STATE__                                                          \
  X(LEXER_STATE_UNDEFINED)  /* Used for assertions */                          \
  X(LEXER_STATE_INITIAL)    /* Starting point for determining next state*/     \
  X(LEXER_STATE_IDENTIFIER) /* Tokenize identifiers made of [A-z][a-z] include \
                               '_' that may be keywords*/                      \
  X(LEXER_STATE_DIGIT)      /* Tokenizes [0-9]*/                               \
  X(LEXER_STATE_SYMBOL)     /* Tokenizes all punctuations that may be variable \
                               length (e.g. <=) */                             \
  X(LEXER_STATE_WHITESPACE) /* All whitespace */                               \
  X(LEXER_STATE_COMPLETE)   /* Signal end of token */                          \
  X(LEXER_STATE_END_OF_FILE) /* Reached end of buffer, further tokenization    \
                                returns empty tokens pointing to NULL*/

typedef uint32_t Char_Class;
enum {
#define X(ENUM) ENUM,
  CHAR_CLASS__
#undef X
      CHAR_CLASS_MAX
};
typedef uint32_t Lexer_State;
enum {

#define X(ENUM) ENUM,
  LEXER_STATE__
#undef X
      LEXER_STATE_MAX
};

typedef struct Lexer {
  char *pContent;
  size_t length;
  size_t cursor;
  size_t row;
  size_t column;
  Lexer_State state;
} Lexer;

extern Lexer_State Lexer_stateTable[CHAR_CLASS_MAX][LEXER_STATE_MAX];
extern Char_Class Lexer_charClassTable[128];

void Lexer_initTables(void);

NODISCARD
Result Lexer_init(Lexer *lexer, char *pFileName,
                  const Allocator *const pAllocator);
void Lexer_release(Lexer *lexer, const Allocator *const pAllocator);

NODISCARD
Result Lexer_tokenize(Lexer *lexer, Token_Proxy *pProxy);

NODISCARD
const char *const Lexer_getCharClass(Char_Class cc);
NODISCARD
const char *const Lexer_getState(Lexer_State ls);

#endif
