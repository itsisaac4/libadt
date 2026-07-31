#include "libadt/stack.h"
#include "libadt/internal/primitive_dispatch.h"

#define DEFINE_STACK_PRIMITIVE(Suffix, Type)                 \
    bool st_detail_Push##Suffix(Stack_t *stack, Type element) \
    {                                                        \
        return adt_AcceptsElementSize(stack, sizeof(Type)) && \
               st_detail_PushRef(stack, &element);            \
    }
ADT_FOR_EACH_PRIMITIVE(DEFINE_STACK_PRIMITIVE)
#undef DEFINE_STACK_PRIMITIVE
