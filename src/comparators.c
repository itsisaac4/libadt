#include "libadt/comparators.h"

int CompareChar(const void *first, const void *second)
{
    const char a = *(const char *)first;
    const char b = *(const char *)second;

    return (a > b) - (a < b);
}

int CompareInt(const void *first, const void *second)
{
    const int a = *(const int *)first;
    const int b = *(const int *)second;

    return (a > b) - (a < b);
}

int CompareUnsignedInt(const void *first, const void *second)
{
    const unsigned int a = *(const unsigned int *)first;
    const unsigned int b = *(const unsigned int *)second;

    return (a > b) - (a < b);
}

int CompareLong(const void *first, const void *second)
{
    const long a = *(const long *)first;
    const long b = *(const long *)second;

    return (a > b) - (a < b);
}

int CompareFloat(const void *first, const void *second)
{
    const float a = *(const float *)first;
    const float b = *(const float *)second;

    return (a > b) - (a < b);
}

int CompareDouble(const void *first, const void *second)
{
    const double a = *(const double *)first;
    const double b = *(const double *)second;

    return (a > b) - (a < b);
}

int ComparePointer(const void *first, const void *second)
{
    const void *a = *(const void **)first;
    const void *b = *(const void **)second;

    return (a > b) - (a < b);
}