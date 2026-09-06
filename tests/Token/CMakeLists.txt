add_executable(Token_test test.c)

target_sources(Token_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test.c

        ${CMAKE_SOURCE_DIR}/src/Token.c
        ${CMAKE_SOURCE_DIR}/src/Allocator.c
        ${CMAKE_SOURCE_DIR}/src/Result.c
)

target_include_directories(Token_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../utils/
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(Token_test PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-g -fsanitize=address -DDEBUG_MODE>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Release>>:-O3 -DNDEBUG>
)
target_link_options(Token_test PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>: -fsanitize=address>
)

add_test(NAME Token_Stream_alignment_requirements_met_and_are_pointed_to_correctly
        COMMAND Token_test)
