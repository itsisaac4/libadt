#ifndef LIBADT_INTERNAL_CONTIGUOUS_STORAGE_H
#define LIBADT_INTERNAL_CONTIGUOUS_STORAGE_H

#include <stdbool.h>
#include <stddef.h>

#include "../../element/comparators.h"

/**
 * @brief Owns resizable contiguous raw storage.
 *
 * Members:
 * - `data`: Allocated element storage.
 * - `capacity`: Number of element slots.
 */
typedef struct
{
    void *data;
    size_t capacity;
} ContiguousStorage_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Allocates empty contiguous storage.
 * @param[out] storage Storage to initialize.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool contiguousStorage_Init(ContiguousStorage_t *storage, size_t elementSize);

/**
 * @brief Allocates storage containing shallow element copies.
 * @param[out] storage Storage to initialize.
 * @param elements Contiguous elements, or NULL when count is zero.
 * @param count Number of elements to copy.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool contiguousStorage_InitFrom(ContiguousStorage_t *storage, const void *elements, size_t count, size_t elementSize);

/**
 * @brief Ensures storage has at least the requested capacity.
 * @param[in,out] storage Initialized storage.
 * @param requiredCapacity Required element slots.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool contiguousStorage_Reserve(ContiguousStorage_t *storage, size_t requiredCapacity, size_t elementSize);

/**
 * @brief Gets mutable storage at a physical index.
 * @param[in,out] storage Initialized storage.
 * @param index Physical element index.
 * @param elementSize Size of one element in bytes.
 * @return The element address, or NULL for invalid arguments.
 */
void *contiguousStorage_At(ContiguousStorage_t *storage, size_t index, size_t elementSize);

/**
 * @brief Gets read-only storage at a physical index.
 * @param storage Initialized storage.
 * @param index Physical element index.
 * @param elementSize Size of one element in bytes.
 * @return The element address, or NULL for invalid arguments.
 */
const void *contiguousStorage_AtConst(const ContiguousStorage_t *storage, size_t index, size_t elementSize);

/**
 * @brief Inserts a shallow copy and shifts following elements right.
 * @param[in,out] storage Initialized storage.
 * @param count Current logical element count.
 * @param index Insertion index from zero through count.
 * @param element Element to copy.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool contiguousStorage_Insert(ContiguousStorage_t *storage, size_t count, size_t index, const void *element, size_t elementSize);

/**
 * @brief Erases raw element bytes and shifts following elements left.
 * @param[in,out] storage Initialized storage.
 * @param count Current logical element count.
 * @param index Element index.
 * @param elementSize Size of one element in bytes.
 * @return true on success; otherwise false.
 */
bool contiguousStorage_Erase(ContiguousStorage_t *storage, size_t count, size_t index, size_t elementSize);

/**
 * @brief Checks whether an address points into allocated storage.
 * @param storage Initialized storage.
 * @param address Address to inspect.
 * @param elementSize Size of one element in bytes.
 * @return true when the address is inside storage; otherwise false.
 */
bool contiguousStorage_ContainsAddress(const ContiguousStorage_t *storage, const void *address, size_t elementSize);

/**
 * @brief Finds the first matching element in sorted contiguous storage.
 *
 * The first @p count elements must already be ordered by @p compare.
 *
 * @param storage Initialized storage.
 * @param count Current logical element count.
 * @param elementSize Size of one element in bytes.
 * @param compare Comparator defining the storage order.
 * @param target Element value to find.
 * @param[out] outIndex Index of the first matching element when found.
 * @return true when a match is found; otherwise false.
 */
bool contiguousStorage_BinarySearchBy(
    const ContiguousStorage_t *storage,
    size_t count,
    size_t elementSize,
    CompareFn_t compare,
    const void *target,
    size_t *outIndex);

/**
 * @brief Releases raw storage without destroying element resources.
 * @param[in,out] storage Storage to release, or NULL.
 * @return Nothing.
 */
void contiguousStorage_Destroy(ContiguousStorage_t *storage);

#ifdef __cplusplus
}
#endif

#endif
