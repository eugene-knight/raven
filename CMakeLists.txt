cmake_minimum_required(VERSION 4.0.0)
project(gr LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_testing()
add_subdirectory(tests)

add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>: -Wall -Werror -g -fsanitize=address>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Release>>:-O3>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:DEBUG_MODE>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Release>>:NDEBUG>
        CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)
target_link_options(${PROJECT_NAME} PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>: -fsanitize=address>
)

target_sources(${PROJECT_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/src/main.c
        ${CMAKE_SOURCE_DIR}/src/Result.c
        ${CMAKE_SOURCE_DIR}/src/Allocator.c
        ${CMAKE_SOURCE_DIR}/src/String.c
        ${CMAKE_SOURCE_DIR}/src/Lexer.c
        ${CMAKE_SOURCE_DIR}/src/Token.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/include
)
