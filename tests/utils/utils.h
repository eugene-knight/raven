#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

/// @brief Obtain result data on condition
#define TEST_RESULT_DATA_IF_NOT(CONDITION)                                         \
  do {                                                                         \
    if (!(CONDITION)) {                                                         \
      Result_Data resultData = Result_data(result);                            \
      printf("%-15s: %s\n", "Context:", resultData.pCtx);                      \
      printf("%-15s: %s\n", "Code:", resultData.pCode);                        \
      printf("%-15s: %s\n", "Description", resultData.pDesc);                  \
      fflush(stdout);                                                          \
      return -1;                                                               \
    }                                                                          \
  } while (0)

#endif
