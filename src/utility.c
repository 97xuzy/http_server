
#include "utility.h"

/*!
Copy a number of characters from source to destination.
A null character will be added at the end of destination string.
(strncpy from stdlib doesnt guarantee the null character under all condition)
which means the destination char array must have space for at least (num + 1)
bytes for the operation to be complete correctly
*/
char* my_strncpy(char* destination, const char* source, size_t num)
{
    for(size_t i = 0; i < num; i++)
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return destination;
}

#ifdef TEST

bool test_my_strncpy()
{
    printf("\n--------------------\n");
    printf("Test my_strncpy()\n");
    printf("--------------------\n\n");

    char str1[64];
    char str2[64];
    strcpy(str1, "1234!");
    my_strncpy(str2, str1, 3);

    if(strcmp(str2, "123") != 0)
    {
        printf("%s, %s\n", str2, "123");
        return false;
    }

    return true;
}

// Main Function for Test
int main()
{
    test_my_strncpy();

    return 0;
}

#endif
