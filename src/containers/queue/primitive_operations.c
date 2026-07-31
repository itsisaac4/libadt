#include "libadt/internal/primitive_dispatch.h"
#include "libadt/queue.h"

#define DEFINE_QUEUE_PRIMITIVE(Suffix, Type)                    \
    bool qu_detail_Enqueue##Suffix(Queue_t *queue, Type element) \
    {                                                           \
        return adt_AcceptsElementSize(queue, sizeof(Type)) &&    \
               qu_detail_EnqueueRef(queue, &element);            \
    }
ADT_FOR_EACH_PRIMITIVE(DEFINE_QUEUE_PRIMITIVE)
#undef DEFINE_QUEUE_PRIMITIVE
