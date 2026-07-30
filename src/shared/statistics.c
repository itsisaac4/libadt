#include "libadt/abstract_data_type.h"

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
        super->size == 0 ||
        super->type.elementSize == 0 ||
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

    memmove(outElement, context.selected, super->type.elementSize);
    return true;
}

bool adt_Min(const ADT_t *adt, void *outElement)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL)
    {
        return false;
    }

    return adt_MinBy(adt, super->type.compare, outElement);
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

    return adt_MaxBy(adt, super->type.compare, outElement);
}

bool adt_MaxBy(const ADT_t *adt, CompareFn_t compare, void *outElement)
{
    return CopyExtremum(adt, compare, outElement, EXTREMUM_MAXIMUM);
}
