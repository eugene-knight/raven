option(TEST_STRING "" ON)
option(TEST_TOKEN "" ON)

if (TEST_STRING)
add_subdirectory(String)
endif()

if (TEST_TOKEN) 
add_subdirectory(Token)
endif()
