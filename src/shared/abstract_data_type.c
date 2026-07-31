#include "libadt/abstract_data_type.h"

#include <stdio.h>

typedef struct
{
    PrintFn_t print;
    bool firstElement;
} PrintContext_t;

static void PrintElement(const void *element, size_t index, void *context)
{
    (void)index;

    PrintContext_t *printContext = (PrintContext_t *)context;

    if (!printContext->firstElement)
    {
        printf(", ");
    }

    printContext->print(element);
    printContext->firstElement = false;
}

bool adt_ForEach(const ADT_t *adt, ADT_ConstVisitFn_t visitor, void *context)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL || super->_private.vtable == NULL || super->_private.vtable->visit == NULL || visitor == NULL)
    {
        return false;
    }

    super->_private.vtable->visit(super, visitor, context);
    return true;
}

bool adt_ForEachMutable(ADT_t *adt, ADT_MutableVisitFn_t visitor, void *context)
{
    ADT_Super_t *super = (ADT_Super_t *)adt;

    if (super == NULL || super->_private.vtable == NULL || super->_private.vtable->visitMutable == NULL || visitor == NULL)
    {
        return false;
    }

    super->_private.vtable->visitMutable(super, visitor, context);
    return true;
}

bool adt_Print(const ADT_t *adt)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;

    if (super == NULL ||
        super->_private.vtable == NULL ||
        super->_private.vtable->containerName == NULL ||
        super->_private.vtable->visit == NULL ||
        super->_private.elementType.print == NULL)
    {
        return false;
    }

    PrintContext_t context = {
        .print = super->_private.elementType.print,
        .firstElement = true};

    printf("%s (size: %zu): [", super->_private.vtable->containerName, super->_private.size);
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
        super->_private.vtable == NULL || super->_private.vtable->containerName == NULL
            ? "<uninitialized>"
            : super->_private.vtable->containerName);
    printf("  size: %zu\n", super->_private.size);
    printf("  element size: %zu\n", super->_private.elementType.elementSize);
    printf("  comparator: %s\n", super->_private.elementType.compare == NULL ? "NULL" : "set");
    printf("  printer: %s\n", super->_private.elementType.print == NULL ? "NULL" : "set");
    printf("  numeric projection: %s\n", super->_private.elementType.toNumber == NULL ? "NULL" : "set");
    printf("  destructor: %s\n", super->_private.elementType.destroy == NULL ? "NULL" : "set");
    printf("  elements: ");

    if (super->_private.vtable == NULL || super->_private.vtable->visit == NULL)
    {
        printf("<unavailable>\n");
        return;
    }

    if (super->_private.elementType.print == NULL)
    {
        printf("<no print function>\n");
        return;
    }

    PrintContext_t context = {
        .print = super->_private.elementType.print,
        .firstElement = true};

    printf("[");
    super->_private.vtable->visit(super, PrintElement, &context);
    printf("]\n");
}
