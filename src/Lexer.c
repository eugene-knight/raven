#include "Lexer.h"
#include <stdio.h>
#include <string.h>

Lexer_State Lexer_stateTable[CHAR_CLASS_MAX][LEXER_STATE_MAX];
Char_Class Lexer_charClassTable[128];

typedef struct ClassStateT {
  Char_Class class;
  Lexer_State input;
  Lexer_State output;
} ClassStateT;

const char *const Lexer_getCharClass(Char_Class cc) {
  switch (cc) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    return #ENUM;
    CHAR_CLASS__
#undef X
  }
  return "CHAR_CLASS_UNKNOWN";
}

const char *const Lexer_getState(Lexer_State ls) {
  switch (ls) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    return #ENUM;
    LEXER_STATE__
#undef X
  }
  return "LEXER_STATE_UNKNOWN";
}

void Lexer_initTables(void) {
  for (size_t i = 'A'; i <= 'Z'; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_LETTER;
  }
  for (size_t i = 'a'; i <= 'z'; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_LETTER;
  }
  for (size_t i = 0; i <= 31; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_UNDEFINED;
  }
  for (size_t i = '0'; i <= '9'; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_DIGIT;
  }

  Lexer_charClassTable[' '] = CHAR_CLASS_WHITESPACE;
  Lexer_charClassTable['\n'] = CHAR_CLASS_WHITESPACE;
  Lexer_charClassTable['\t'] = CHAR_CLASS_WHITESPACE;
  Lexer_charClassTable['\v'] = CHAR_CLASS_WHITESPACE;
  Lexer_charClassTable['\r'] = CHAR_CLASS_WHITESPACE;

  for (size_t i = '!'; i <= '/'; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  for (size_t i = ':'; i <= '@'; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  for (size_t i = '['; i <= '`'; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  for (size_t i = '{'; i <= '~'; i += 1) {
    Lexer_charClassTable[i] = CHAR_CLASS_SYMBOL;
  }
  Lexer_charClassTable[127] = CHAR_CLASS_UNDEFINED;
  Lexer_charClassTable['_'] = CHAR_CLASS_LETTER;

  Lexer_charClassTable['\0'] = CHAR_CLASS_NULL_TERMINATOR;

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
  X(LEXER_STATE_DIGIT, LEXER_STATE_COMPLETE)                                   \
  X(LEXER_STATE_SYMBOL, LEXER_STATE_COMPLETE)                                  \
  X(LEXER_STATE_WHITESPACE, LEXER_STATE_WHITESPACE)                            \
  X(LEXER_STATE_COMPLETE, LEXER_STATE_INITIAL)                                 \
  X(LEXER_STATE_END_OF_FILE, LEXER_STATE_END_OF_FILE)

#define CHAR_CLASS_NULL_TERMINATOR__                                           \
  X(LEXER_STATE_INITIAL, LEXER_STATE_END_OF_FILE)                              \
  X(LEXER_STATE_IDENTIFIER, LEXER_STATE_END_OF_FILE)                           \
  X(LEXER_STATE_DIGIT, LEXER_STATE_END_OF_FILE)                                \
  X(LEXER_STATE_SYMBOL, LEXER_STATE_END_OF_FILE)                               \
  X(LEXER_STATE_WHITESPACE, LEXER_STATE_END_OF_FILE)                           \
  X(LEXER_STATE_COMPLETE, LEXER_STATE_END_OF_FILE)                             \
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
#define X(OLD, NEW) {CHAR_CLASS_NULL_TERMINATOR, OLD, NEW},
                      CHAR_CLASS_NULL_TERMINATOR__
#undef X
  };

  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    Lexer_stateTable[CHAR_CLASS_UNDEFINED][i] = LEXER_STATE_UNDEFINED;
  }
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    Lexer_stateTable[CHAR_CLASS_NULL_TERMINATOR][i] = LEXER_STATE_END_OF_FILE;
  }

  for (size_t i = 0; i < ARRAY_SIZE(classStateT); i += 1) {
    Lexer_stateTable[classStateT[i].class][classStateT[i].input] =
        classStateT[i].output;
  }
}

void Lexer_debugTables(void) {
  Lexer_State stateOrder[] = {
#define X(ENUM) ENUM,
      LEXER_STATE__
#undef X
  };
  (void)Lexer_getCharClass;
  printf("CHAR_CLASS_LETTER:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf("+----| %-25s -> %s\n", Lexer_getState(stateOrder[i]),
           Lexer_getState(Lexer_stateTable[CHAR_CLASS_LETTER][stateOrder[i]]));
  }
  printf("\nCHAR_CLASS_DIGIT:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf("+----| %-25s -> %s\n", Lexer_getState(stateOrder[i]),
           Lexer_getState(Lexer_stateTable[CHAR_CLASS_DIGIT][stateOrder[i]]));
  }
  printf("\nCHAR_CLASS_SYMBOL:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf("+----| %-25s -> %s\n", Lexer_getState(stateOrder[i]),
           Lexer_getState(Lexer_stateTable[CHAR_CLASS_SYMBOL][stateOrder[i]]));
  }
  printf("\nCHAR_CLASS_WHITESPACE:\n");
  for (size_t i = 0; i < LEXER_STATE_MAX; i += 1) {
    printf(
        "+----| %-25s -> %s\n", Lexer_getState(stateOrder[i]),
        Lexer_getState(Lexer_stateTable[CHAR_CLASS_WHITESPACE][stateOrder[i]]));
  }
}

Result Lexer_init(Lexer *lexer, char *pFileName,
                  const Allocator *const pAllocator) {
  static Bool isInitTable = TRUE;
  if (isInitTable) {
    Lexer_initTables();
    isInitTable = FALSE;
  }

  Result result = Result_createSuccess();
  FILE *pFile = fopen(pFileName, "r+");
  if (!pFile) {
    return Result_create(CONTEXT_GENERAL, GENERAL_ERRNO);
  }

  fseek(pFile, 0, SEEK_END);
  long fileSize = ftell(pFile);
  if (fileSize < 0) {
    result = Result_create(CONTEXT_GENERAL, GENERAL_ERRNO);
    goto cleanup;
  }
  fseek(pFile, 0, SEEK_SET);

  Byte *pBuffer = TALLOCATE(char, fileSize + 1);
  if (!pBuffer) {
    result = Result_create(CONTEXT_GENERAL, GENERAL_NULL_ALLOCATION);
    goto cleanup;
  }

  size_t bytesRead = fread(pBuffer, sizeof(char), fileSize, pFile);
  pBuffer[bytesRead] = '\0';

  lexer->pContent = (char *)&pBuffer[0];
  lexer->length = bytesRead;
  lexer->cursor = 0;
  lexer->row = lexer->column = 1;
  lexer->state = LEXER_STATE_INITIAL;

cleanup:
  fclose(pFile);
  return result;
}
void Lexer_release(Lexer *lexer, const Allocator *const pAllocator) {
  DEALLOCATE(lexer->pContent);
  lexer->pContent = NULL;
  lexer->length = 0;
  lexer->cursor = 0;
  lexer->row = 1;
  lexer->column = 1;
  lexer->state = LEXER_STATE_END_OF_FILE;
}

static inline char *Lexer_pointsAt(Lexer *lexer) {
  return &lexer->pContent[lexer->cursor];
}
static inline Lexer_State Lexer_currentState(Lexer *lexer) {
  return lexer->state;
}
static inline Bool Lexer_isState(Lexer *lexer, Lexer_State state) {
  return lexer->state == state;
}
static inline char Lexer_charClass(char c) {
  return Lexer_charClassTable[(u8)c];
}
static inline void Lexer_complete(Lexer *lexer) {
  lexer->state = LEXER_STATE_COMPLETE;
}
static inline void Lexer_restart(Lexer *lexer) {
  lexer->state = LEXER_STATE_INITIAL;
}
static inline void Lexer_advanceCursor(Lexer *lexer) {
  lexer->cursor += 1;
  lexer->column += 1;
}
static inline void Lexer_newline(Lexer *lexer) {
  lexer->row += 1;
  lexer->column = 0;
}

/// @brief Reducer
/// Function *always* takes a character class and old state to
/// and returns a new state
static inline void Lexer_updateState(Lexer *lexer, Char_Class cc,
                                     Lexer_State oldState) {
  Lexer_State newState = Lexer_stateTable[cc][oldState];
  lexer->state = newState;
}
/// @brief Updates `lexer` to a next state
/// Function *only* updates state based on the currently pointed character
static inline void Lexer_nextState(Lexer *lexer) {
  char *ptr = Lexer_pointsAt(lexer);
  Char_Class cc = Lexer_charClass(*ptr);
  Lexer_State oldState = Lexer_currentState(lexer);
  Lexer_updateState(lexer, cc, oldState);
}

/// @brief Advances and synchronize state with the current character
/// Function *always* advance before updating state
static inline void Lexer_advanceNSyncState(Lexer *lexer) {
  Lexer_advanceCursor(lexer);
  Lexer_nextState(lexer);
}

static void Lexer_isKeyword(Lexer *lexer, Token_Proxy *pProxy) {
  char *lhs = Token_getProxyPtr(pProxy);
  // INFO: Replace if/else chain with perfect hash (260918)
  if (strncmp(lhs, "end", 3) == 0) {
    Token_setProxyType(pProxy, TOKEN_TYPE_KEYWORD_END);
  } else if (strncmp(lhs, "struct", 6) == 0) {
    Token_setProxyType(pProxy, TOKEN_TYPE_KEYWORD_STRUCT);
  } else if (strncmp(lhs, "enum", 4) == 0) {
    Token_setProxyType(pProxy, TOKEN_TYPE_KEYWORD_ENUM);
  } else if (strncmp(lhs, "data", 4) == 0) {
    Token_setProxyType(pProxy, TOKEN_TYPE_KEYWORD_DATA);
  } else if (strncmp(lhs, "union", 5) == 0) {
    Token_setProxyType(pProxy, TOKEN_TYPE_KEYWORD_UNION);
  }
}

typedef struct Position {
  size_t row;
  size_t column;
} Position;
static inline Position Lexer_cursorPosition(Lexer *lexer) {
  return (Position){.row = lexer->row, .column = lexer->column};
}

static inline void Lexer_symbolColon(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_COLON);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  switch (*Lexer_pointsAt(lexer)) {
  case ':': {
    Token_setProxyType(pProxy, TOKEN_TYPE_COLON_COLON);
    Token_setProxyLength(pProxy, 2);
    break;
  }
  }

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolEqual(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_EQUAL);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolComma(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_COMMA);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolDollar(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_DOLLAR);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolCaret(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_CARET);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolSemicolon(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_SEMICOLON);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolRightParenthesis(Lexer *lexer,
                                                Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_RIGHT_PARENTHESIS);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolLeftParenthesis(Lexer *lexer,
                                               Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_LEFT_PARENTHESIS);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolMinus(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_MINUS);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  switch (*Lexer_pointsAt(lexer)) {
  case '>': {
    Token_setProxyType(pProxy, TOKEN_TYPE_MINUS_GREATER_THAN);
    Token_setProxyLength(pProxy, 2);
    break;
  }
  }

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolPercentage(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_PERCENTAGE);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolForwardSlash(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_FORWARD_SLASH);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolAsterisk(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_ASTERISK);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolPlus(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_PLUS);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbolLessThan(Lexer *lexer, Token_Proxy *pProxy) {
  Token_setProxyPtr(pProxy, Lexer_pointsAt(lexer));
  Token_setProxyType(pProxy, TOKEN_TYPE_LESS_THAN);
  Token_setProxyLength(pProxy, 1);

  Lexer_advanceCursor(lexer);
  switch (*Lexer_pointsAt(lexer)) {
  case '-': {
    Token_setProxyType(pProxy, TOKEN_TYPE_LESS_THAN_MINUS);
    Token_setProxyLength(pProxy, 2);
    break;
  }
  }

  Lexer_advanceCursor(lexer);
  Lexer_complete(lexer);
}

static inline void Lexer_symbol(Lexer *lexer, Token_Proxy *pProxy) {
  Position pos = Lexer_cursorPosition(lexer);
  Token_setProxyRow(pProxy, pos.row);
  Token_setProxyColumn(pProxy, pos.column);

  switch (*Lexer_pointsAt(lexer)) {
  case '%': {
    Lexer_symbolPercentage(lexer, pProxy);
    break;
  }
  case '/': {
    Lexer_symbolForwardSlash(lexer, pProxy);
    break;
  }
  case '*': {
    Lexer_symbolAsterisk(lexer, pProxy);
    break;
  }
  case '+': {
    Lexer_symbolPlus(lexer, pProxy);
    break;
  }
  case '-': {
    Lexer_symbolMinus(lexer, pProxy);
    break;
  }
  case '<': {
    Lexer_symbolLessThan(lexer, pProxy);
    break;
  }
  case '=': {
    Lexer_symbolEqual(lexer, pProxy);
    break;
  }
  case ',': {
    Lexer_symbolComma(lexer, pProxy);
    break;
  }
  case '^': {
    Lexer_symbolCaret(lexer, pProxy);
    break;
  }
  case '$': {
    Lexer_symbolDollar(lexer, pProxy);
    break;
  }
  case ';': {
    Lexer_symbolSemicolon(lexer, pProxy);
    break;
  }
  case ':': {
    Lexer_symbolColon(lexer, pProxy);
    break;
  }
  case '(': {
    Lexer_symbolLeftParenthesis(lexer, pProxy);
    break;
  }
  case ')': {
    Lexer_symbolRightParenthesis(lexer, pProxy);
    break;
  }
  default:
    assert(0 && "unimplemented symbol");
  }

  assert(Lexer_currentState(lexer) == LEXER_STATE_COMPLETE);
}

static inline void Lexer_identifier(Lexer *lexer, Token_Proxy *pProxy) {
  char *pStart = Lexer_pointsAt(lexer);
  Position pos = Lexer_cursorPosition(lexer);

  Lexer_advanceNSyncState(lexer);
  for (; Lexer_isState(lexer, LEXER_STATE_IDENTIFIER);) {
    Lexer_advanceNSyncState(lexer);
  }

  assert(Lexer_currentState(lexer) == LEXER_STATE_COMPLETE);

  Token_setProxyPtr(pProxy, pStart);
  Token_setProxyLength(pProxy, (Lexer_pointsAt(lexer) - pStart));
  Token_setProxyRow(pProxy, pos.row);
  Token_setProxyColumn(pProxy, pos.column);
  Token_setProxyType(pProxy, TOKEN_TYPE_IDENTIFIER);

  Lexer_isKeyword(lexer, pProxy);
}

static inline void Lexer_digit(Lexer *lexer, Token_Proxy *pProxy) {
  char *pStart = Lexer_pointsAt(lexer);
  Position pos = Lexer_cursorPosition(lexer);

  Lexer_advanceNSyncState(lexer);
  for (; Lexer_isState(lexer, LEXER_STATE_DIGIT);) {
    Lexer_advanceNSyncState(lexer);
  }

  assert(Lexer_currentState(lexer) == LEXER_STATE_COMPLETE);

  Token_setProxyPtr(pProxy, pStart);
  Token_setProxyLength(pProxy, (Lexer_pointsAt(lexer) - pStart));
  Token_setProxyRow(pProxy, pos.row);
  Token_setProxyColumn(pProxy, pos.column);
  Token_setProxyType(pProxy, TOKEN_TYPE_DIGIT);
}

Result Lexer_tokenize(Lexer *lexer, Token_Proxy *pProxy) {
  Bool isNotComplete = TRUE;

  assert(Lexer_currentState(lexer) == LEXER_STATE_INITIAL);
  for (; isNotComplete;) {
    ///< Lexer always tokenize starting from initial state
    switch (Lexer_currentState(lexer)) {
    case LEXER_STATE_INITIAL: {
      Lexer_nextState(lexer);
      break;
    }
    case LEXER_STATE_COMPLETE: {
      ///< Always returns to `LEXER_STATE_INITIAL`
      Lexer_restart(lexer);
      isNotComplete = FALSE;
      break;
    }
    case LEXER_STATE_IDENTIFIER: {
      Lexer_identifier(lexer, pProxy);
      break;
    }
    case LEXER_STATE_SYMBOL: {
      Lexer_symbol(lexer, pProxy);
      break;
    }
    case LEXER_STATE_DIGIT: {
      Lexer_digit(lexer, pProxy);
      break;
    }
    case LEXER_STATE_END_OF_FILE: {
      isNotComplete = FALSE;
      Token_setProxyNull(pProxy);
      return Result_createSuccess();
    }
    case LEXER_STATE_WHITESPACE: {
      for (; Lexer_isState(lexer, LEXER_STATE_WHITESPACE);) {
        if (*Lexer_pointsAt(lexer) == '\n') {
          Lexer_newline(lexer);
        }
        Lexer_advanceNSyncState(lexer);
      }
      Lexer_restart(lexer);
      break;
    }
    default:
      assert(0 && "Encountered LEXER_STATE_UNDEFINED");
    }
  }
  return Result_createSuccess();
}
