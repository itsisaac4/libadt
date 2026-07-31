#ifndef LIBADT_INTERNAL_QUEUE_OPERATIONS_H
#define LIBADT_INTERNAL_QUEUE_OPERATIONS_H

#ifndef LIBADT_QUEUE_H
#error "Include libadt/queue.h instead."
#endif

/** @cond INTERNAL */
bool qu_detail_EnqueueRef(Queue_t *queue, const void *element);

#define ADT_PRIMITIVE(Suffix, Type) \
    bool qu_detail_Enqueue##Suffix(Queue_t *queue, Type element);
ADT_FOR_EACH_PRIMITIVE(ADT_PRIMITIVE)
#undef ADT_PRIMITIVE
/** @endcond */

#endif
