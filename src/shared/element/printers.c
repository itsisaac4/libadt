#include "libadt/element/printers.h"

#include <stdio.h>

void PrintChar(const void *element)
{
    const char value = *(const char *)element;
    printf("%c", value);
}

void PrintInt(const void *element)
{
    printf("%d", *(const int *)element);
}

void PrintUnsignedInt(const void *element)
{
    printf("%u", *(const unsigned int *)element);
}

void PrintLong(const void *element)
{
    printf("%ld", *(const long *)element);
}

void PrintFloat(const void *element)
{
    printf("%f", *(const float *)element);
}

void PrintDouble(const void *element)
{
    printf("%f", *(const double *)element);
}

void PrintPointer(const void *element)
{
    void *value = *(void *const *)element;
    printf("%p", value);
}
