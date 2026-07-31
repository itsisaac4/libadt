#include "libadt/abstract_data_type.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    EXTREMUM_MINIMUM,
    EXTREMUM_MAXIMUM
} Extremum_t;

typedef struct
{
    CompareFn_t compare;
    const void *selected;
    Extremum_t kind;
} ExtremumContext_t;

typedef struct
{
    ToNumberFn_t toNumber;
    long double sum;
} MeanContext_t;

typedef struct
{
    ToNumberFn_t toNumber;
    double *values;
} NumericBufferContext_t;

static void FindExtremum(const void *element, size_t index, void *context)
{
    (void)index;

    ExtremumContext_t *extremum = (ExtremumContext_t *)context;

    if (extremum->selected == NULL)
    {
        extremum->selected = element;
        return;
    }

    int comparison = extremum->compare(element, extremum->selected);

    if ((extremum->kind == EXTREMUM_MINIMUM && comparison < 0) ||
        (extremum->kind == EXTREMUM_MAXIMUM && comparison > 0))
    {
        extremum->selected = element;
    }
}

static bool CopyExtremum(const ADT_t *adt, CompareFn_t compare, void *outElement, Extremum_t extremum)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL ||
        super->_private.size == 0 ||
        super->_private.elementType.elementSize == 0 ||
        compare == NULL ||
        outElement == NULL)
    {
        return false;
    }

    ExtremumContext_t context = {
        .compare = compare,
        .selected = NULL,
        .kind = extremum};

    if (!adt_ForEach(adt, FindExtremum, &context) ||
        context.selected == NULL)
    {
        return false;
    }

    memmove(outElement, context.selected, super->_private.elementType.elementSize);
    return true;
}

static void AddToMean(const void *element, size_t index, void *context)
{
    (void)index;

    MeanContext_t *mean = context;
    mean->sum += (long double)mean->toNumber(element);
}

static void CollectNumericValue(const void *element, size_t index, void *context)
{
    NumericBufferContext_t *buffer = context;
    buffer->values[index] = buffer->toNumber(element);
}

static double *CollectNumericValues(const ADT_t *adt, ToNumberFn_t toNumber, size_t *outCount)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL ||
        super->_private.size == 0 ||
        super->_private.size > SIZE_MAX / sizeof(double) ||
        toNumber == NULL ||
        outCount == NULL)
    {
        return NULL;
    }

    double *values = malloc(super->_private.size * sizeof(double));

    if (values == NULL)
    {
        return NULL;
    }

    NumericBufferContext_t context = {
        .values = values,
        .toNumber = toNumber};

    if (!adt_ForEach(adt, CollectNumericValue, &context))
    {
        free(values);
        return NULL;
    }

    *outCount = super->_private.size;
    return values;
}

bool adt_Min(const ADT_t *adt, void *outElement)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL)
    {
        return false;
    }

    return adt_MinBy(adt, super->_private.elementType.compare, outElement);
}

bool adt_MinBy(const ADT_t *adt, CompareFn_t compare, void *outElement)
{
    return CopyExtremum(adt, compare, outElement, EXTREMUM_MINIMUM);
}

bool adt_Max(const ADT_t *adt, void *outElement)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL)
    {
        return false;
    }

    return adt_MaxBy(adt, super->_private.elementType.compare, outElement);
}

bool adt_MaxBy(const ADT_t *adt, CompareFn_t compare, void *outElement)
{
    return CopyExtremum(adt, compare, outElement, EXTREMUM_MAXIMUM);
}

bool adt_Mean(const ADT_t *adt, double *outMean)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL)
    {
        return false;
    }

    return adt_MeanBy(adt, super->_private.elementType.toNumber, outMean);
}

bool adt_MeanBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMean)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL || super->_private.size == 0 || toNumber == NULL || outMean == NULL)
    {
        return false;
    }

    MeanContext_t context = {.toNumber = toNumber, .sum = 0.0L};

    if (!adt_ForEach(adt, AddToMean, &context))
    {
        return false;
    }

    *outMean = (double)(context.sum / (long double)super->_private.size);

    return true;
}

bool adt_Median(const ADT_t *adt, double *outMedian)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL)
    {
        return false;
    }

    return adt_MedianBy(adt, super->_private.elementType.toNumber, outMedian);
}

bool adt_MedianBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMedian)
{
    if (outMedian == NULL)
    {
        return false;
    }

    size_t count = 0;
    double *values = CollectNumericValues(adt, toNumber, &count);

    if (values == NULL)
    {
        return false;
    }

    qsort(values, count, sizeof(double), CompareDouble);

    if (count % 2 == 1)
    {
        *outMedian = values[count / 2];
    }
    else
    {
        const double lower = values[count / 2 - 1];
        const double upper = values[count / 2];

        *outMedian = lower / 2.0 + upper / 2.0;
    }

    free(values);
    return true;
}

bool adt_Mode(const ADT_t *adt, double *outMode)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL)
    {
        return false;
    }

    return adt_ModeBy(adt, super->_private.elementType.toNumber, outMode);
}

bool adt_ModeBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMode)
{
    if (outMode == NULL)
    {
        return false;
    }

    size_t count = 0;
    double *values = CollectNumericValues(adt, toNumber, &count);

    if (values == NULL)
    {
        return false;
    }

    qsort(values, count, sizeof(double), CompareDouble);

    size_t currentCount = 1;
    size_t bestCount = 1;
    double bestValue = values[0];

    for (size_t i = 1; i < count; i++)
    {
        if (CompareDouble(&values[i - 1], &values[i]) == 0)
        {
            currentCount++;
        }
        else
        {
            currentCount = 1;
        }

        if (currentCount > bestCount)
        {
            bestCount = currentCount;
            bestValue = values[i];
        }
    }

    free(values);

    if (bestCount == 1)
    {
        return false;
    }

    *outMode = bestValue;
    return true;
}
