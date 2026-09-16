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
  lexer->cursor = lexer->row = lexer->column = 0;
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
static inline void Lexer_restart(Lexer *lexer) {
  lexer->state = LEXER_STATE_INITIAL;
}
static inline void Lexer_advanceCursor(Lexer *lexer) {
  lexer->cursor += 1;
  lexer->column += 1;
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
  if (strncmp(lhs, "enum", 4) == 0) {
    Token_setProxyType(pProxy, TOKEN_TYPE_KEYWORD_END);
  }
}

static void Lexer_identifier(Lexer *lexer, Token_Proxy *pProxy) {
  char *pStart = Lexer_pointsAt(lexer);

  Lexer_advanceNSyncState(lexer);
  for (; Lexer_isState(lexer, LEXER_STATE_IDENTIFIER);) {
    Lexer_advanceNSyncState(lexer);
  }

  assert(Lexer_currentState(lexer) == LEXER_STATE_COMPLETE);

  Token_setProxyPtr(pProxy, pStart);
  Token_setProxyLength(pProxy, (Lexer_pointsAt(lexer) - pStart));
  Token_setProxyType(pProxy, TOKEN_TYPE_IDENTIFIER);

  Lexer_isKeyword(lexer, pProxy);
}

Result Lexer_tokenize(Lexer *lexer, Token_Proxy *pProxy) {
  Bool isNotComplete = TRUE;

  for (; isNotComplete;) {
    if (!Lexer_isState(lexer, LEXER_STATE_END_OF_FILE)) {
      ///< Lexer always tokenize starting from initial state
      assert(Lexer_currentState(lexer) == LEXER_STATE_INITIAL);
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
      }
      case LEXER_STATE_END_OF_FILE: {
        return Result_createSuccess();
      }
      case LEXER_STATE_WHITESPACE: {
        if (*Lexer_pointsAt(lexer) == '\n') {
          Lexer_newline(lexer);
        }
      }
      default:
        assert(0 && "Encountered LEXER_STATE_UNDEFINED");
      }
    }
  }
  return Result_createSuccess();
}
