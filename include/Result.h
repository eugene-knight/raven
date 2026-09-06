#ifndef RESULT_H
#define RESULT_H

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof((ARR)[0]))
#define UNUSED(VAR) (void)VAR

#define X_CONTEXT                                                              \
  X(CTX_GENERAL)     /* General errors (e.g null allocations) */               \
  X(CTX_STRING_TYPE) /* struct String; (e.g. index >= capacity) */             \
  X(CTX_LEXER_TYPE)  /* struct Lexer; (e.g. pFile == NULL) */                  \
  X(CTX_ERRNO)       /* errno errors. (e.g perror, strerror) */

#define GENERAL_CODE(X)                                                        \
  X(GENERAL_NULL_ALLOC, "Indicates allocations that returned null")            \
  X(GENERAL_ZERO_SIZE_ALLOC, "Indicates allocation that was passed a size of " \
                             "0")

#define STRING_TYPE_CODE(X)                                                    \
  X(STRING_TYPE_COPY_OUT_OF_BOUNDS_I,                                          \
    "Copy start index exceeded string allocated capacity")                     \
  X(STRING_TYPE_COPY_OUT_OF_BOUNDS_S,                                          \
    "Copy size exceeded string allocate capacity")

#define LEXER_TYPE_CODE(X)                                                     \
  X(LEXER_TYPE_FAILED_TO_OPEN_FILE, "Failed to open file")

#define X_CODE(X)                                                              \
  GENERAL_CODE(X)                                                              \
  STRING_TYPE_CODE(X)                                                          \
  LEXER_TYPE_CODE(X)

#define BOOL_TRUE 1
#define BOOL_FALSE 0

#ifndef BOOL_TYPE
#define BOOL_TYPE
typedef uint8_t Bool;
#endif


typedef uint32_t Context;
enum {
  SUCCESS,
#define X(ENUM) ENUM,
  X_CONTEXT
#undef X
};

typedef uint32_t Code;
enum {
#define X(ENUM, _) ENUM,
  X_CODE(X)
#undef X
};

typedef struct Result {
  uint32_t code;
  uint32_t ctx;
} Result;
typedef struct Result_Data {
  char *pCtx;
  char *pCode;
  char *pDesc;
} Result_Data;

/// @brief Quick allocation NULL check
#define CHKALLOCR_(RESULT, MEM)                                                \
  do {                                                                         \
    if (MEM == NULL) {                                                         \
      RESULT.ctx = CTX_GENERAL;                                                \
      RESULT.code = GENERAL_NULL_ALLOC;                                        \
      return RESULT;                                                           \
    }                                                                          \
  } while (0)

#define CHKALLOCR(MEM) CHKALLOCR_(result, MEM)

/// @brief Quick allocation NULL check
#define CHKALLOCG_(RESULT, MEM)                                                \
  do {                                                                         \
    if (MEM == NULL) {                                                         \
      RESULT.ctx = CTX_GENERAL;                                                \
      RESULT.code = GENERAL_NULL_ALLOC;                                        \
      goto _result;                                                            \
    }                                                                          \
  } while (0)
#define CHKALLOCG(MEM) CHKALLOCG_(result, MEM)

#define CHKALLOCGN_(RESULT, MEM, GOTO)                                         \
  do {                                                                         \
    if (MEM == NULL) {                                                         \
      RESULT.ctx = CTX_GENERAL;                                                \
      RESULT.code = GENERAL_NULL_ALLOC;                                        \
      goto GOTO;                                                               \
    }                                                                          \
  } while (0)
#define CHKALLOCGN(MEM, GOTO) CHKALLOCGN_(result, MEM, GOTO)

#define CHKRESULT_(RESULT, BODY)                                               \
  do {                                                                         \
    if (RESULT.ctx != SUCCESS)                                                 \
      BODY                                                                     \
  } while (0)
#define CHKRESULT(BODY) CHKRESULT_(result, BODY)

#define CHKRESULTDATA_ONERRORG(RESULT)                                          \
  CHKRESULT_(RESULT, {                                                         \
    Result_Data data = Result_data(RESULT);                                    \
    fprintf(stderr,                                                            \
            "%-15s: %s\n"                                                      \
            "%-15s: %s\n"                                                      \
            "%-15s: %s\n",                                                     \
            "Context: ", data.pCtx, "Code: ", data.pCode,                      \
            "Description: ", data.pDesc);                                      \
    goto _result;                                                              \
  })

Bool Result_expectCode(Result result, Code code);
Bool Result_isSuccess(Result result);
Result_Data Result_data(Result result);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    // If using a modern C23 compiler
    #define NODISCARD [[nodiscard]]
#elif defined(__GNUC__) || defined(__clang__)
    // For GCC/Clang in C99 mode
    #define NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
    // For MSVC
    #define NODISCARD _Check_return_
#else
    // Fallback for other standard C99 compilers
    #define NODISCARD
#endif

#endif
