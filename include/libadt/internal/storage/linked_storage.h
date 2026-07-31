#ifndef LIBADT_INTERNAL_LINKED_STORAGE_H
#define LIBADT_INTERNAL_LINKED_STORAGE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct LinkedStorageNode LinkedStorageNode_t;

/**
 * @brief Stores one element and its neighboring nodes.
 *
 * Members:
 * - `data`: Owned raw element copy.
 * - `previous`: Previous node, or NULL at the head.
 * - `next`: Next node, or NULL at the tail.
 */
struct LinkedStorageNode
{
    void *data;
    LinkedStorageNode_t *previous;
    LinkedStorageNode_t *next;
};

/**
 * @brief Owns a doubly linked sequence of raw element copies.
 *
 * Members:
 * - `head`: First storage node.
 * - `tail`: Last storage node.
 */
typedef struct
{
    LinkedStorageNode_t *head;
    LinkedStorageNode_t *tail;
} LinkedStorage_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initializes empty linked storage.
 * @param[out] storage Storage to initialize.
 * @return true on success; otherwise false.
 */
bool linkedStorage_Init(LinkedStorage_t *storage);

/**
 * @brief Initializes linked storage with shallow element copies.
 * @param[out] storage Storage to initialize.
 * @param elements Contiguous elements, or NULL when count is zero.
 * @param count Number of elements to copy.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool linkedStorage_InitFrom(LinkedStorage_t *storage, const void *elements, size_t count, size_t elementSize);

/**
 * @brief Gets mutable storage at a logical index.
 * @param[in,out] storage Initialized storage.
 * @param count Current logical element count.
 * @param index Element index.
 * @return The element address, or NULL for invalid arguments.
 */
void *linkedStorage_At(LinkedStorage_t *storage, size_t count, size_t index);

/**
 * @brief Gets read-only storage at a logical index.
 * @param storage Initialized storage.
 * @param count Current logical element count.
 * @param index Element index.
 * @return The element address, or NULL for invalid arguments.
 */
const void *linkedStorage_AtConst(const LinkedStorage_t *storage, size_t count, size_t index);

/**
 * @brief Inserts a shallow element copy at an index.
 * @param[in,out] storage Initialized storage.
 * @param count Current logical element count.
 * @param index Insertion index from zero through count.
 * @param element Element to copy.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool linkedStorage_Insert(LinkedStorage_t *storage, size_t count, size_t index, const void *element, size_t elementSize);

/**
 * @brief Prepends a shallow element copy.
 * @param[in,out] storage Initialized storage.
 * @param element Element to copy.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool linkedStorage_Prepend(LinkedStorage_t *storage, const void *element, size_t elementSize);

/**
 * @brief Appends a shallow element copy.
 * @param[in,out] storage Initialized storage.
 * @param element Element to copy.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool linkedStorage_Append(LinkedStorage_t *storage, const void *element, size_t elementSize);

/**
 * @brief Erases an element's raw storage and node.
 * @param[in,out] storage Initialized storage.
 * @param count Current logical element count.
 * @param index Element index.
 * @return true on success; otherwise false.
 */
bool linkedStorage_Erase(LinkedStorage_t *storage, size_t count, size_t index);

/**
 * @brief Checks whether an address points into an element allocation.
 * @param storage Initialized storage.
 * @param address Address to inspect.
 * @param elementSize Size of one element in bytes.
 * @return true when the address is inside storage; otherwise false.
 */
bool linkedStorage_ContainsAddress(const LinkedStorage_t *storage, const void *address, size_t elementSize);

/**
 * @brief Releases raw nodes and storage without destroying element resources.
 * @param[in,out] storage Storage to release, or NULL.
 * @return Nothing.
 */
void linkedStorage_Destroy(LinkedStorage_t *storage);

#ifdef __cplusplus
}
#endif

#endif
