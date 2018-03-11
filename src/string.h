#include <stdlib.h>

typedef struct String
{
    char *ptr;
    size_t length;
    size_t capacity;
} String;

char at(const String *str, size_t index)
{
    if(index > length-1) return -1;
    return str->ptr[index];
}

int append(String *str, const String *str2)
{
    int new_cap = str->capacity > str2->capacity ? str->capacity : str2->capacity;
    new_cap *= 2;
    str->ptr = realloc(new_cap, new_cap*2);
    str->capacity = new_cap;
}

int append(String *str, const char *str2)
{
    ;
}

