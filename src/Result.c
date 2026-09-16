#include "Result.h"

Result Result_createSuccess(void) { return (Result){0}; }
Result Result_create(Context ctx, Code code) {
  return (Result){.code = code, .context = ctx};
}
Bool Result_expectContext(Result result, Context ctx) {
  return result.context == ctx;
}
Bool Result_expectCode(Result result, Code code) { return result.code == code; }
Bool Result_isSuccess(Result result) { return result.context == SUCCESS; }

const char *const Result_ctx(Result result) {
  switch (result.context) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    return #ENUM;
    X_CONTEXT
#undef X
  }
  return "CTX_UNDEFINED";
}

const char *const Result_code(Result result) {
  switch (result.code) {
#define X(ENUM, _)                                                             \
  case ENUM:                                                                   \
    return #ENUM;
    X_CODE(X)
#undef X
  }
  return "CODE_UNDEFINED";
}

const char *const Result_desc(Result result) {
  switch (result.code) {
#define X(ENUM, DESC)                                                          \
  case ENUM:                                                                   \
    return #DESC;
    X_CODE(X)
#undef X
  }
  return "DESC_UNDEFINED";
}

Result_Data Result_data(Result _result) {
  Result_Data result = {0};
  switch (_result.context) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    result.pCtx = #ENUM;                                                       \
    break;
    X_CONTEXT
#undef X
  default:
    result.pCtx = "CTX_UNDEFINED";
    break;
  }
  switch (_result.code) {
#define X(ENUM, _)                                                             \
  case ENUM:                                                                   \
    result.pCode = #ENUM;                                                      \
    break;
    X_CODE(X)
#undef X
  default:
    result.pCode = "CODE_UNDEFINED";
    break;
  }
  switch (_result.code) {
#define X(ENUM, DESC)                                                          \
  case ENUM:                                                                   \
    result.pDesc = #DESC;                                                      \
    break;
    X_CODE(X)
#undef X
  default:
    result.pDesc = "DESC_UNDEFINED";
  }
  return result;
}

#include <stdio.h>
void Result_printData(Result result) {
  Result_Data data = Result_data(result);
  fprintf(stderr,
          "CONTEXT: %s\n"
          "CODE:    %s\n"
          "DETAILS: %s\n",
          data.pCtx, data.pCode, data.pDesc);
}
