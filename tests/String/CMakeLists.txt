add_executable(String_test test.c)

target_sources(String_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test_String_copys.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test_String_copyi.c

        ${CMAKE_SOURCE_DIR}/src/String.c
        ${CMAKE_SOURCE_DIR}/src/Result.c
)

target_include_directories(String_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../utils/
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(String_test PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-g -fsanitize=address -DDEBUG_MODE>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Release>>:-O3 -DNDEBUG>
)
target_link_options(String_test PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>: -fsanitize=address>
)

add_test(NAME String_copys__copy_within_capacity_succeed
        COMMAND String_test "String_copys_1")

add_test(NAME String_copys__copy_exceed_capacity_fails
        COMMAND String_test "String_copys_2")

add_test(NAME String_copyi__index_within_capacity_succeed
        COMMAND String_test "String_copyi_1")
add_test(NAME String_copyi__index_exceed_capacity_fails
        COMMAND String_test "String_copyi_2")
add_test(NAME String_copyi__newLength_within_capacity_succeed
        COMMAND String_test "String_copyi_3")
add_test(NAME String_copyi__newLength_within_capacity_suceeed_twice_consecutively
        COMMAND String_test "String_copyi_4")
add_test(NAME String_copyi__newLength_exceed_capacity_fails
        COMMAND String_test "String_copyi_5")

