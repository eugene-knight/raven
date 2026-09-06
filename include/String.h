#ifndef STRING_H
#define STRING_H

#include "Allocator.h"
#include "Result.h"

typedef struct String {
    char *ptr;
    size_t length;
    size_t capacity;
} String;
typedef struct String_View {
    char *ptr;
    size_t length;
} String_View;

Result String_init(String *out, size_t capacity, const Allocator *const pAllocator);
String String_ownCstr(char *src, size_t length, size_t capacity);
void String_release(String *in, const Allocator *const pAllocator);
Result String_copys(String *in, char *src, size_t length);
Result String_copyi(String *in, char *src, size_t length, size_t index);
Result String_copya(String *in, char *src, size_t length);
void String_move(String *dst, String *src);
const char * const String_data(String *in);
size_t String_length(String *in);
size_t String_capacity(String *in);

String_View String_View_fromString(String *string);
String_View String_View_fromChar(char *src, size_t length);


#endif
