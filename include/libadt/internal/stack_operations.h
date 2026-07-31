#ifndef LIBADT_INTERNAL_STACK_OPERATIONS_H
#define LIBADT_INTERNAL_STACK_OPERATIONS_H

#ifndef LIBADT_STACK_H
#error "Include libadt/stack.h instead."
#endif

/** @cond INTERNAL */
bool st_detail_PushRef(Stack_t *stack, const void *element);

#define ADT_PRIMITIVE(Suffix, Type) \
    bool st_detail_Push##Suffix(Stack_t *stack, Type element);
ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE
/** @endcond */

#endif
