#include "String.h"
#include "utils.h"

int test_String_copyi(int id) {
  char srcString[] = "Hello!"; ///< 7 (including null terminator)
  size_t srcStringSize =
      ARRAY_SIZE(srcString) - 1; ///< 6 (exclude null terminator)

  /// Null terminator excluded
  size_t GarbageSize = 6;
  size_t GaSize = 2;

  char copyiBufferHalved[(ARRAY_SIZE(srcString) / 2)] = "He";
  size_t copyiBufferHalvedSize = ARRAY_SIZE(copyiBufferHalved) - 1;

  char copyiBuffer[ARRAY_SIZE(srcString)] = "Hello!";
  size_t copyiBufferSize = ARRAY_SIZE(copyiBuffer) - 1;

  char copyiBufferDoubled[(ARRAY_SIZE(srcString) * 2)] = "Hello!";
  size_t copyiBufferDoubledSize = ARRAY_SIZE(copyiBufferDoubled) - 1;

  char copyiBufferTripled[(ARRAY_SIZE(srcString) * 3)] = "Hello!";
  size_t copyiBufferTripledSize = ARRAY_SIZE(copyiBufferTripled) - 1;

  switch (id) {
  case 1: {
    String string = {
        .ptr = &copyiBufferDoubled[0],
        .length = GarbageSize,
        .capacity = copyiBufferDoubledSize,
    };
    printf("[CASE 1]: %-10s %s\n", "Before:", string.ptr);
    Result result =
        String_copyi(&string, srcString, srcStringSize, GarbageSize);
    printf("[CASE 1]: %-10s %s\n", "After:", string.ptr);
    TEST_RESULT_DATA_IF_NOT(Result_isSuccess(result));
    return 0;
  }
  case 2: {
    String string = {.ptr = &copyiBufferHalved[0],
                     .length = GaSize,
                     .capacity = copyiBufferHalvedSize};
    printf("[CASE 2]: %-10s %s\n", "Before:", string.ptr);
    Result result = String_copyi(&string, srcString, srcStringSize, GaSize);
    printf("[CASE 2]: %-10s %s\n", "After:", string.ptr);
    TEST_RESULT_DATA_IF_NOT(
        Result_expectCode(result, STRING_TYPE_COPY_OUT_OF_BOUNDS_I));
    return 0;
  }
  case 3: {
    String string = {.ptr = &copyiBufferDoubled[0],
                     .length = GarbageSize,
                     .capacity = copyiBufferDoubledSize};
    printf("[CASE 3]: %-10s %s\n", "1:", string.ptr);
    Result result =
        String_copyi(&string, srcString, srcStringSize, GarbageSize);
    printf("[CASE 3]: %-10s %s\n", "2:", string.ptr);
    TEST_RESULT_DATA_IF_NOT(Result_isSuccess(result));
    return 0;
  }
  case 4: {
    String string = {.ptr = &copyiBufferTripled[0],
                     .length = GarbageSize,
                     .capacity = copyiBufferTripledSize};
    printf("[CASE 3]: %-10s %s\n", "1:", string.ptr);
    Result result =
        String_copyi(&string, srcString, srcStringSize, GarbageSize);
    printf("[CASE 3]: %-10s %s\n", "2:", string.ptr);
    TEST_RESULT_DATA_IF_NOT(Result_isSuccess(result));
    result = String_copyi(&string, srcString, srcStringSize, GarbageSize * 2);
    printf("[CASE 3]: %-10s %s\n", "3:", string.ptr);
    TEST_RESULT_DATA_IF_NOT(Result_isSuccess(result));
    return 0;
  }
  case 5: {
    String string = {.ptr = &copyiBuffer[0],
                     .length = GarbageSize,
                     .capacity = copyiBufferSize};
    printf("[CASE 3]: %-10s %s\n", "1:", string.ptr);
    Result result = ///< GarbageSize - 1 so that we avoid triggering index out of bounds error
        String_copyi(&string, srcString, srcStringSize, GarbageSize - 1);
    printf("[CASE 3]: %-10s %s\n", "2:", string.ptr);
    TEST_RESULT_DATA_IF_NOT(
        Result_expectCode(result, STRING_TYPE_COPY_OUT_OF_BOUNDS_S));
            return 0;
  }
  }
  return -1;
}
