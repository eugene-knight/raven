#ifndef RESULT_H
#define RESULT_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof((ARR)[0]))
#define UNUSED(VAR) (void)VAR

#define X_CONTEXT                                                              \
  X(CONTEXT_GENERAL) /* General errors (e.g null allocations)  */              \
  X(CONTEXT_STRING_TYPE)                                                       \
  X(CONTEXT_LEXER_TYPE)

#define GENERAL_CODE(X)                                                        \
  X(GENERAL_NULL_ALLOCATION, "Indicates allocations that returned null")       \
  X(GENERAL_ZERO_SIZE_ALLOCATION,                                              \
    "Indicates allocation that was passed a size of "                          \
    "0")                                                                       \
  X(GENERAL_ERRNO, "Obtain details from `strerror`")

#define STRING_CODE(X)                                                         \
  X(STRING_TYPE_COPY_EXCEED_CAPACITY, "Copy length exceeded string capacity")

#define X_CODE(X)                                                              \
  GENERAL_CODE(X)                                                              \
  STRING_CODE(X)

#define TRUE 1
#define FALSE 0

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
  Code code;
  Context context;
} Result;
typedef struct Result_Data {
  char *pCtx;
  char *pCode;
  char *pDesc;
} Result_Data;

Bool Result_expectCode(Result result, Code code);
Bool Result_expectContext(Result result, Context ctx);
Bool Result_isSuccess(Result result);
Result_Data Result_data(Result result);
void Result_printData(Result result);
Result Result_createSuccess(void);
Result Result_create(Context ctx, Code code);

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
