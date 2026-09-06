#include "String.h"
#include "utils.h"

int test_String_copys(int id) {

  char srcString[] = "Hello world!";
  size_t srcStringSize = ARRAY_SIZE(srcString) - 1; // Null terminator does not count

  switch (id) {
  case 1: {
    char copysBuffer[ARRAY_SIZE(srcString)] = "Garbage";
    size_t copysBufferSize = ARRAY_SIZE(srcString);

    String copys = {
        .ptr = &copysBuffer[0],
        .length = copysBufferSize,
        .capacity = copysBufferSize,
    };

    // length <= copys.capacity, thus should succeed
    Result result = String_copys(&copys, srcString, srcStringSize);
    TEST_RESULT_DATA_IF_NOT(Result_isSuccess(result));
    return 0;
  }

  case 2: {
    char copysBufferHalved[(ARRAY_SIZE(srcString) / 2)] = "Garba";
    size_t copysBufferHalvedSize = ARRAY_SIZE(copysBufferHalved);

    String copys = {
        .ptr = &copysBufferHalved[0],
        .length = copysBufferHalvedSize,
        .capacity = copysBufferHalvedSize,
    };

    // srcString > copys.capacity, thus should fail
    Result result = String_copys(&copys, srcString, srcStringSize);
    TEST_RESULT_DATA_IF_NOT(result.code == STRING_TYPE_COPY_OUT_OF_BOUNDS_S);
    return 0;
  }
  }

  return -1;
}

