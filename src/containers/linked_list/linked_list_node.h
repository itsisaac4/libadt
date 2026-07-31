#ifndef LIBADT_INTERNAL_LINKED_LIST_NODE_H
#define LIBADT_INTERNAL_LINKED_LIST_NODE_H

#include "libadt/linked_list.h"

struct LinkedListNode
{
    struct
    {
        void *data;
        LinkedListNode_t *previous;
        LinkedListNode_t *next;
    } _private;
};

#endif
