#include "Result.h"

Bool Result_expectCode(Result result, Code code) { return result.code == code; }

Bool Result_isSuccess(Result result) { return result.ctx == SUCCESS; }

const char *const Result_ctx(Result result) {
  switch (result.ctx) {
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
  switch (_result.ctx) {
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
