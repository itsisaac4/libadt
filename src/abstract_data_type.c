#include "libadt/abstract_data_type.h"

#include <stdio.h>

typedef struct
{
    PrintFn_t print;
    bool first;
} PrintContext_t;

static void PrintElement(const void *element, size_t index, void *context)
{
    (void)index;

    PrintContext_t *printContext = (PrintContext_t *)context;

    if (!printContext->first)
    {
        printf(", ");
    }

    printContext->print(element);
    printContext->first = false;
}

bool adt_ForEach(const ADT_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL || super->vtable == NULL || super->vtable->visit == NULL || visitor == NULL)
    {
        return false;
    }

    super->vtable->visit(super, visitor, context);
    return true;
}

bool adt_ForEachMutable(ADT_t *adt, ADT_MutableVisitFn_t visitor, void *context)
{
    ADT_Super_t *super = (ADT_Super_t *)adt;

    if (super == NULL || super->vtable == NULL || super->vtable->visitMutable == NULL || visitor == NULL)
    {
        return false;
    }

    super->vtable->visitMutable(super, visitor, context);
    return true;
}

bool adt_Print(const ADT_t *adt)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL ||
        super->vtable == NULL ||
        super->vtable->containerName == NULL ||
        super->vtable->visit == NULL ||
        super->type.print == NULL)
    {
        return false;
    }

    PrintContext_t context = {
        .print = super->type.print,
        .first = true};

    printf("%s (size: %zu): [", super->vtable->containerName, super->size);
    adt_ForEach(adt, PrintElement, &context);
    printf("]\n");

    return true;
}

void adt_PrintDebug(const ADT_t *adt, const char *expression, const char *file, int line)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    printf("ADT Debug\n");
    printf("  expression: %s\n", expression == NULL ? "<unknown>" : expression);
    printf("  location: %s:%d\n", file == NULL ? "<unknown>" : file, line);
    printf("  address: %p\n", (const void *)adt);

    if (super == NULL)
    {
        printf("  state: NULL\n");
        return;
    }

    printf(
        "  container: %s\n",
        super->vtable == NULL || super->vtable->containerName == NULL
            ? "<uninitialized>"
            : super->vtable->containerName);
    printf("  size: %zu\n", super->size);
    printf("  element size: %zu\n", super->type.elementSize);
    printf("  comparator: %s\n", super->type.compare == NULL ? "NULL" : "set");
    printf("  printer: %s\n", super->type.print == NULL ? "NULL" : "set");
    printf("  destructor: %s\n", super->type.destroy == NULL ? "NULL" : "set");
    printf("  elements: ");

    if (super->vtable == NULL || super->vtable->visit == NULL)
    {
        printf("<unavailable>\n");
        return;
    }

    if (super->type.print == NULL)
    {
        printf("<no print function>\n");
        return;
    }

    PrintContext_t context = {
        .print = super->type.print,
        .first = true};

    printf("[");
    super->vtable->visit(super, PrintElement, &context);
    printf("]\n");
}
