#include "Lexer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef const Allocator *const _Allocator;

static const char *const getCharClass(Char_Class cc) {
  switch (cc) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    return #ENUM;
    CHAR_CLASS__
#undef X
  }
  return "CHAR_CLASS_UNKNOWN";
}

static const char *const getLexerState(Lexer_State ls) {
  switch (ls) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    return #ENUM;
    LEXER_STATE__
#undef X
  }
  return "LEXER_STATE_UNKNOWN";
}

Result Lexer_init(Lexer *out, char *fileName, _Allocator pAllocator) {
  Result result = {0};

  // Resources & Resource-Dependent Variables //
  FILE *pFile = NULL;
  void *pBuffer = NULL;
  char *charBuffer = NULL;
  size_t bytesRead = 0;
  long fileSize = 0;
  // ====================== //

  pFile = fopen(fileName, "r+");
  if (pFile == NULL) {
    result.ctx = CTX_LEXER_TYPE;
    result.code = LEXER_TYPE_FAILED_TO_OPEN_FILE;
    goto _result;
  }

  fseek(pFile, 0, SEEK_END);
  fileSize = ftell(pFile);
  fseek(pFile, 0, SEEK_SET);

  if (fileSize < 0) {
    result.ctx = CTX_ERRNO;
    goto _result;
  }

  // + 1 includes null terminator
  pBuffer = TALLOCATE(char, fileSize + 1);
  CHKALLOCG(pBuffer);

  bytesRead = fread(pBuffer, 1, fileSize, pFile);
  if (bytesRead < fileSize && ferror(pFile)) {
    result.ctx = CTX_ERRNO;
    goto _result;
  }

  charBuffer = &pBuffer[0];
  charBuffer[bytesRead] = '\0';

_result:
  if (pFile != NULL) {
    fclose(pFile);
  }

  out->string = String_ownCstr(&charBuffer[0], bytesRead, bytesRead);
  out->cursor = 0;
  out->state = LEXER_STATE_INITIAL;

  return result;
}
void Lexer_release(Lexer *in, const Allocator *const pAllocator) {
  DEALLOCATE(in->string.ptr);
  in->string.ptr = NULL;
  in->string.length = in->string.capacity = 0;
}

static Char_Class ClassTable[128];
static Lexer_State StateTable[CHAR_CLASS_MAX][LEXER_STATE_MAX];

typedef struct ClassStateT {
  Char_Class class;
  Lexer_State input;
  Lexer_State output;
} ClassStateT;

void Lexer_initTables(void) {
  for (size_t i = 'A'; i <= 'Z'; i += 1) {
    ClassTable[i] = CHAR_CLASS_LETTER;
  }
  for (size_t i = 'a'; i <= 'z'; i += 1) {
    ClassTable[i] = CHAR_CLASS_LETTER;
  }
  for (size_t i = 0; i <= 31; i += 1) {
    ClassTable[i] = CHAR_CLASS_UNDEFINED;
  }
  for (size_t i = '0'; i <= '9'; i += 1) {
    ClassTable[i] = CHAR_CLASS_DIGIT;
  }

  ClassTable[' '] = CHAR_CLASS_WHITESPACE;
  ClassTable['\n'] = CHAR_CLASS_WHITESPACE;
  ClassTable['\t'] = CHAR_CLASS_WHITESPACE;
  ClassTable['\v'] = CHAR_CLASS_WHITESPACE;
  ClassTable['\r'] = CHAR_CLASS_WHITESPACE;

  for (size_t i = '!'; i <= '/'; i += 1) {
    ClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  for (size_t i = ':'; i <= '@'; i += 1) {
    ClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  for (size_t i = '['; i <= '`'; i += 1) {
    ClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  for (size_t i = '{'; i <= '~'; i += 1) {
    ClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  ClassTable[127] = CHAR_CLASS_UNDEFINED;
  ClassTable['_'] = CHAR_CLASS_LETTER;

  ClassTable['\0'] = CHAR_CLASS_NULL_TERMINATOR;

#define CHAR_CLASS_LETTER__                                                    \
  X(LEXER_STATE_INITIAL, LEXER_STATE_IDENTIFIER)                               \
  X(LEXER_STATE_IDENTIFIER, LEXER_STATE_IDENTIFIER)                            \
  X(LEXER_STATE_DIGIT, LEXER_STATE_COMPLETE)                                   \
  X(LEXER_STATE_SYMBOL, LEXER_STATE_COMPLETE)                                  \
  X(LEXER_STATE_WHITESPACE, LEXER_STATE_COMPLETE)                              \
  X(LEXER_STATE_END_OF_FILE, LEXER_STATE_END_OF_FILE)                          \
  X(LEXER_STATE_COMPLETE, LEXER_STATE_INITIAL)

#define CHAR_CLASS_DIGIT__                                                     \
  X(LEXER_STATE_INITIAL, LEXER_STATE_DIGIT)                                    \
  X(LEXER_STATE_IDENTIFIER, LEXER_STATE_COMPLETE)                              \
  X(LEXER_STATE_DIGIT, LEXER_STATE_DIGIT)                                      \
  X(LEXER_STATE_SYMBOL, LEXER_STATE_COMPLETE)                                  \
  X(LEXER_STATE_WHITESPACE, LEXER_STATE_COMPLETE)                              \
  X(LEXER_STATE_END_OF_FILE, LEXER_STATE_END_OF_FILE)                          \
  X(LEXER_STATE_COMPLETE, LEXER_STATE_INITIAL)

#define CHAR_CLASS_SYMBOL__                                                    \
  X(LEXER_STATE_INITIAL, LEXER_STATE_SYMBOL)                                   \
  X(LEXER_STATE_IDENTIFIER, LEXER_STATE_COMPLETE)                              \
  X(LEXER_STATE_DIGIT, LEXER_STATE_COMPLETE)                                   \
  X(LEXER_STATE_SYMBOL, LEXER_STATE_SYMBOL)                                    \
  X(LEXER_STATE_WHITESPACE, LEXER_STATE_COMPLETE)                              \
  X(LEXER_STATE_END_OF_FILE, LEXER_STATE_END_OF_FILE)                          \
  X(LEXER_STATE_COMPLETE, LEXER_STATE_INITIAL)

#define CHAR_CLASS_WHITESPACE__                                                \
  X(LEXER_STATE_INITIAL, LEXER_STATE_WHITESPACE)                               \
  X(LEXER_STATE_IDENTIFIER, LEXER_STATE_COMPLETE)                              \
  X(LEXER_STATE_DIGIT, LEXER_STATE_UNDEFINED)                                  \
  X(LEXER_STATE_SYMBOL, LEXER_STATE_COMPLETE)                                  \
  X(LEXER_STATE_WHITESPACE, LEXER_STATE_WHITESPACE)                            \
  X(LEXER_STATE_COMPLETE, LEXER_STATE_INITIAL)                                 \
  X(LEXER_STATE_END_OF_FILE, LEXER_STATE_END_OF_FILE)

  ClassStateT classStateT[] = {
#define X(OLD, NEW) {CHAR_CLASS_LETTER, OLD, NEW},
      CHAR_CLASS_LETTER__
#undef X
#define X(OLD, NEW) {CHAR_CLASS_DIGIT, OLD, NEW},
          CHAR_CLASS_DIGIT__
#undef X
#define X(OLD, NEW) {CHAR_CLASS_SYMBOL, OLD, NEW},
              CHAR_CLASS_SYMBOL__
#undef X
#define X(OLD, NEW) {CHAR_CLASS_WHITESPACE, OLD, NEW},
                  CHAR_CLASS_WHITESPACE__
#undef X
  };

  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    StateTable[CHAR_CLASS_UNDEFINED][i] = LEXER_STATE_UNDEFINED;
  }
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    StateTable[CHAR_CLASS_NULL_TERMINATOR][i] = LEXER_STATE_END_OF_FILE;
  }

  for (size_t i = 0; i < ARRAY_SIZE(classStateT); i += 1) {
    StateTable[classStateT[i].class][classStateT[i].input] =
        classStateT[i].output;
  }
}

void Lexer_debugTables(void) {
  Lexer_State stateOrder[] = {
#define X(ENUM) ENUM,
      LEXER_STATE__
#undef X
  };
  (void)getCharClass;
  printf("CHAR_CLASS_LETTER:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf("+----| %-25s -> %s\n", getLexerState(stateOrder[i]),
           getLexerState(StateTable[CHAR_CLASS_LETTER][stateOrder[i]]));
  }
  printf("\nCHAR_CLASS_DIGIT:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf("+----| %-25s -> %s\n", getLexerState(stateOrder[i]),
           getLexerState(StateTable[CHAR_CLASS_DIGIT][stateOrder[i]]));
  }
  printf("\nCHAR_CLASS_SYMBOL:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf("+----| %-25s -> %s\n", getLexerState(stateOrder[i]),
           getLexerState(StateTable[CHAR_CLASS_SYMBOL][stateOrder[i]]));
  }
  printf("\nCHAR_CLASS_WHITESPACE:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf("+----| %-25s -> %s\n", getLexerState(stateOrder[i]),
           getLexerState(StateTable[CHAR_CLASS_WHITESPACE][stateOrder[i]]));
  }
}

NODISCARD
static inline char peek(Lexer *lexer) {
  const char *const set = String_data(&lexer->string);
  ///< `set` meaning relative to the start of the file/buffer
  size_t cursor = lexer->cursor;
  return set[cursor];
}
static inline void advance(Lexer *lexer) { lexer->cursor += 1; }
static void updateState(Lexer *lexer) {
  char c = peek(lexer);
  Char_Class cc = ClassTable[(uint8_t)c];
  Lexer_State input = lexer->state;
  Lexer_State output = StateTable[cc][input];
  lexer->state = output;

  assert(cc != CHAR_CLASS_UNDEFINED);
  assert(output != LEXER_STATE_UNDEFINED);
  assert(input != LEXER_STATE_UNDEFINED);
}
static inline void advance_n_updateState(Lexer *lexer) {
  advance(lexer);
  updateState(lexer);
}
static void keyword(Token_Proxy **proxy) {
  const char *const lhs = *((*proxy)->ptr);
  size_t length = *((*proxy)->length);

  if (strncmp(lhs, "end", length) == 0) {
    *((*proxy)->type) = TOKEN_TYPE_KEYWORD_END;
  }
}

static void identifier(Lexer *lexer, Token_Proxy *proxy, size_t i) {
  const char *const set = &String_data(&lexer->string)[0];
  size_t cursor = lexer->cursor;

  char *start = (char *)&set[cursor];
  char *end = start;

  for (; lexer->state == LEXER_STATE_IDENTIFIER;) {
    advance_n_updateState(lexer);
    end += 1;
  }
  ptrdiff_t length = end - start;

  *(proxy->ptr) = start;
  *(proxy->length) = length;
  *(proxy->type) = TOKEN_TYPE_IDENTIFIER;
  keyword(&proxy);
}

static void digit(Lexer *lexer, Token_Proxy *proxy, size_t i) {
  const char *const set = &String_data(&lexer->string)[0];
  size_t cursor = lexer->cursor;

  char *start = (char *)&set[cursor];
  char *end = start;

  for (; lexer->state == LEXER_STATE_DIGIT;) {
    advance_n_updateState(lexer);
    end += 1;
  }
  ptrdiff_t length = end - start;
  *(proxy->ptr) = start;
  *(proxy->length) = length;
  *(proxy->type) = TOKEN_TYPE_DIGIT;
}
static void symbol(Lexer *lexer, Token_Proxy *proxy, size_t i) {
  const char *const set = &String_data(&lexer->string)[0];
  size_t cursor = lexer->cursor;

  char *start = (char *)&set[cursor];
  char *end = start;

  Token_Type type = TOKEN_TYPE_UNDEFINED;

  switch (*end) {
  case '+': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_PLUS;
    break;
  }
  case '"': {
    advance_n_updateState(lexer);
    end += 1;
    for (; *end != '"';) {
      advance_n_updateState(lexer);
      end += 1;
      if (*end == '\n') {
        assert(0 && "unterminated string handling unimplemented");
      }
    }
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_STRING_LITERAL;
    break;
  }
  case '@': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_AT;
    break;
  }
  case '^': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_CARET;
    break;
  }
  case ':': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_COLON;
    if (*end == ':') {
      advance_n_updateState(lexer);
      end += 1;
      type = TOKEN_TYPE_COLON_COLON;
      break;
    } else if (*end == '=') {
      advance_n_updateState(lexer);
      end += 1;
      type = TOKEN_TYPE_COLON_EQUAL;
      break;
    }
    break;
  }
  case '(': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_LEFT_PARENTHESIS;
    break;
  }
  case ')': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_RIGHT_PARENTHESIS;
    break;
  }
  case ',': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_COMMA;
    break;
  }
  case '-': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_MINUS;
    if (*end == '>') {
      advance_n_updateState(lexer);
      end += 1;
      type = TOKEN_TYPE_MINUS_GREATER_THAN;
      break;
    }
    break;
  }
  case '=': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_EQUAL;
    break;
  }
  case ';': {
    advance_n_updateState(lexer);
    end += 1;
    type = TOKEN_TYPE_SEMICOLON;
    break;
  }
  }

  ptrdiff_t length = end - start;

  if (type == TOKEN_TYPE_UNDEFINED) {
    printf("Char: %c\n", *start);
    printf("String: %.*s\n", (int)length, start);
    printf("Token_Type: %s\n", Token_getType(type));
    assert(0 && type != TOKEN_TYPE_UNDEFINED);
  }

  *(proxy->ptr) = start;
  *(proxy->length) = length;
  *(proxy->type) = type;
}

void Lexer_populateTokenStream(Lexer *lexer, Token_Stream *stream) {

  for (size_t i = 0; lexer->state != LEXER_STATE_END_OF_FILE;
       i += 1, stream->count += 1) {
    Token_Proxy proxy = {.ptr = &stream->ptr[i],
                         .length = &stream->length[i],
                         .type = &stream->type[i]};

  _restart:
    switch (lexer->state) {
    case LEXER_STATE_INITIAL: {
      updateState(lexer);
      goto _restart;
    }
    case LEXER_STATE_IDENTIFIER: {
      identifier(lexer, &proxy, i);
      break;
    }
    case LEXER_STATE_DIGIT: {
      digit(lexer, &proxy, i);
      break;
    }
    case LEXER_STATE_SYMBOL: {
      symbol(lexer, &proxy, i);
      break;
    }
    case LEXER_STATE_END_OF_FILE: {
      *proxy.type = TOKEN_TYPE_END_OF_FILE;
      break;
    }
    case LEXER_STATE_COMPLETE: {
      updateState(lexer);
      goto _restart;
    }
    case LEXER_STATE_WHITESPACE: {
      advance(lexer);
      updateState(lexer);
      goto _restart;
    }
    case LEXER_STATE_UNDEFINED: {
      assert(0 && "Lexer State: Reached LEXER_STATE_UNDEFINED");
    }
    }
  }
}
