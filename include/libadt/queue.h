#ifndef LIBADT_QUEUE_H
#define LIBADT_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

#include "abstract_data_type.h"
#include "internal/primitive_dispatch.h"
#include "internal/storage/linked_storage.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Stores elements in first-in, first-out order.
 *
 * Members:
 * - `super`: Shared ADT state; must remain first.
 * - `_private`: Internal queue state; do not modify.
 */
typedef struct
{
    ADT_Super_t super;

    struct
    {
        LinkedStorage_t storage;
    } _private;
} Queue_t;

#ifdef __cplusplus
static_assert(offsetof(Queue_t, super) == 0, "Queue_t.super must be first");
#else
_Static_assert(offsetof(Queue_t, super) == 0, "Queue_t.super must be first");
#endif

#ifndef __cplusplus
/* --- QU_INIT / QUEUE_INIT ------------------------------------------------ */

/**
 * @brief Initializes an empty queue of primitive elements and automatically constructs its ADT_ElementTypeInfo_t.
 * @param queue Queue to initialize.
 * @param type Element type.
 * @return true on success; otherwise false.
 */
#define QUEUE_INIT QU_INIT
#define QU_INIT(queue, type)   \
    qu_Init(                   \
        (queue),               \
        ADT_ELEMENT_TYPE_INFO( \
            type,              \
            COMPARATOR(type),  \
            PRINTER(type),     \
            TO_NUMBER(type),   \
            NULL))

/* --- QU_INIT_FROM / QUEUE_INIT_FROM -------------------------------------- */

/**
 * @brief Initializes a queue from primitive elements and automatically constructs its ADT_ElementTypeInfo_t.
 * @param queue Queue to initialize.
 * @param values Values ordered from front to back.
 * @return true on success; otherwise false.
 */
#define QUEUE_INIT_FROM QU_INIT_FROM
#define QU_INIT_FROM(queue, values)                 \
    qu_InitFrom(                                    \
        (queue),                                    \
        (values),                                   \
        ARRAY_COUNT(values),                        \
        ADT_ELEMENT_TYPE_INFO(                      \
            typeof_unqual((values)[0]),             \
            COMPARATOR(typeof_unqual((values)[0])), \
            PRINTER(typeof_unqual((values)[0])),    \
            TO_NUMBER(typeof_unqual((values)[0])),  \
            NULL))
#endif

/* --- qu_Init / queue_Init ------------------------------------------------- */

/**
 * @brief Initializes an empty queue.
 * @param[out] queue Queue to initialize.
 * @param elementType Runtime element information; elementSize must be nonzero.
 * @return true on success; otherwise false.
 */
#define queue_Init qu_Init
bool qu_Init(Queue_t *queue, ADT_ElementTypeInfo_t elementType);

/* --- qu_InitFrom / queue_InitFrom ----------------------------------------- */

/**
 * @brief Initializes a queue with shallow copies ordered front to back.
 * @param[out] queue Queue to initialize.
 * @param elements Contiguous elements, or NULL when initialCount is zero.
 * @param initialCount Number of elements.
 * @param elementType Runtime element information; elementSize must be nonzero.
 * @return true on success; otherwise false.
 */
#define queue_InitFrom qu_InitFrom
bool qu_InitFrom(Queue_t *queue, const void *elements, size_t initialCount, ADT_ElementTypeInfo_t elementType);

/* --- qu_Front / queue_Front ----------------------------------------------- */

/**
 * @brief Copies the front element without removing it.
 * @param queue Source queue.
 * @param[out] outElement Destination storage.
 * @return true on success; otherwise false.
 */
#define queue_Front qu_Front
bool qu_Front(const Queue_t *queue, void *outElement);

/* --- qu_Back / queue_Back ------------------------------------------------- */

/**
 * @brief Copies the back element without removing it.
 * @param queue Source queue.
 * @param[out] outElement Destination storage.
 * @return true on success; otherwise false.
 */
#define queue_Back qu_Back
bool qu_Back(const Queue_t *queue, void *outElement);

#include "internal/queue_operations.h"

#ifndef __cplusplus
/* --- qu_Enqueue / queue_Enqueue ------------------------------------------- */

/**
 * @brief Enqueues a shallow element copy at the back.
 *
 * Supports any initialized element type. Pass supported primitives by value
 * and all other element types by address.
 *
 * @param queue Target queue.
 * @param element Value or address matching the initialized element type.
 * @return true on success; otherwise false.
 */
#define queue_Enqueue qu_Enqueue
#define qu_Enqueue(queue, element)                             \
    ADT_DETAIL_DISPATCH_ELEMENT(qu_detail_Enqueue, (element))( \
        (queue),                                               \
        (element))
#endif

/* --- qu_Dequeue / queue_Dequeue ------------------------------------------- */

/**
 * @brief Dequeues the front element and transfers its resource ownership.
 * @param[in,out] queue Target queue.
 * @param[out] outElement Removed element.
 * @return true on success; otherwise false.
 */
#define queue_Dequeue qu_Dequeue
bool qu_Dequeue(Queue_t *queue, void *outElement);

/* --- qu_Discard / queue_Discard ------------------------------------------- */

/**
 * @brief Removes the front element and destroys its owned resources.
 * @param[in,out] queue Target queue.
 * @return true on success; otherwise false.
 */
#define queue_Discard qu_Discard
bool qu_Discard(Queue_t *queue);

/* --- qu_Clear / queue_Clear ----------------------------------------------- */

/**
 * @brief Removes all elements while retaining type information.
 * @param[in,out] queue Queue to clear, or NULL.
 * @return Nothing.
 */
#define queue_Clear qu_Clear
void qu_Clear(Queue_t *queue);

/* --- qu_Destroy / queue_Destroy ------------------------------------------- */

/**
 * @brief Releases all elements and resets the queue.
 * @param[in,out] queue Queue to destroy, or NULL.
 * @return Nothing.
 */
#define queue_Destroy qu_Destroy
void qu_Destroy(Queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif
