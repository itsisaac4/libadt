#ifndef LIBADT_ABSTRACT_DATA_TYPE_H
#define LIBADT_ABSTRACT_DATA_TYPE_H

#include <stdbool.h>
#include <stddef.h>

#include "element/comparators.h"
#include "element/number_converters.h"
#include "element/printers.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Releases resources owned by an element.
 * @param[in,out] element Element whose resources are released.
 * @return Nothing.
 */
typedef void (*DestroyFn_t)(void *element);

/**
 * @brief Describes the runtime behavior of an element type.
 *
 * Members:
 * - `elementSize`: Size of one element in bytes.
 * - `compare`: Optional element comparator.
 * - `print`: Optional element printer.
 * - `toNumber`: Optional numeric projection.
 * - `destroy`: Optional owned-resource destructor.
 */
typedef struct
{
    size_t elementSize;
    CompareFn_t compare;
    PrintFn_t print;
    ToNumberFn_t toNumber;
    DestroyFn_t destroy;
} ADT_ElementTypeInfo_t;

#ifndef __cplusplus
/**
 * @brief Creates complete runtime information for an element type.
 * @param Type Element type.
 * @param Compare Comparator, or NULL.
 * @param Print Printer, or NULL.
 * @param ToNumber Numeric projection, or NULL.
 * @param Destroy Owned-resource destructor, or NULL.
 * @return A complete element type descriptor.
 */
#define ADT_ELEMENT_TYPE_INFO(Type, Compare, Print, ToNumber, Destroy) \
    ((ADT_ElementTypeInfo_t){                                          \
        .elementSize = sizeof(Type),                                   \
        .compare = (Compare),                                          \
        .print = (Print),                                              \
        .toNumber = (ToNumber),                                        \
        .destroy = (Destroy)})
#endif

/** @brief Container-specific virtual operations. */
typedef struct ADT_VTable_t ADT_VTable_t;

/**
 * @brief Stores state shared by every libadt container.
 *
 * Members:
 * - `_private`: Internal shared state; do not modify.
 */
typedef struct
{
    struct
    {
        const ADT_VTable_t *vtable;
        size_t size;
        ADT_ElementTypeInfo_t elementType;
    } _private;
} ADT_Super_t;

/** @brief Opaque parameter type accepted by shared ADT functions. */
typedef void ADT_t;

/**
 * @brief Sorting algorithms supported by shared ADT operations.
 *
 * Values:
 * - `ADT_SORT_BUBBLE`: Stable adjacent-swap sort.
 * - `ADT_SORT_SELECTION`: Minimum-selection sort.
 * - `ADT_SORT_INSERTION`: Stable insertion sort.
 * - `ADT_SORT_QUICK`: Partition-based quick sort.
 * - `ADT_SORT_BOGO`: Bounded randomized sort.
 */
typedef enum
{
    ADT_SORT_BUBBLE,
    ADT_SORT_SELECTION,
    ADT_SORT_INSERTION,
    ADT_SORT_QUICK,
    ADT_SORT_BOGO
} ADT_SortAlgorithm_t;

/**
 * @brief Visits one element without allowing modification.
 * @param element Current element.
 * @param index Logical element index.
 * @param context Caller-provided operation state.
 * @return Nothing.
 */
typedef void (*ADT_ConstVisitFn_t)(const void *element, size_t index, void *context);

/**
 * @brief Visits one element and allows its stored value to be modified.
 * @param[in,out] element Current element.
 * @param index Logical element index.
 * @param context Caller-provided operation state.
 * @return Nothing.
 */
typedef void (*ADT_MutableVisitFn_t)(void *element, size_t index, void *context);

/**
 * @brief Defines container-specific traversal operations.
 *
 * Members:
 * - `containerName`: Human-readable container type.
 * - `visit`: Read-only traversal operation.
 * - `visitMutable`: Mutable element traversal operation.
 */
struct ADT_VTable_t
{
    const char *containerName;
    void (*visit)(const ADT_Super_t *adt, ADT_ConstVisitFn_t visitor, void *context);
    void (*visitMutable)(ADT_Super_t *adt, ADT_MutableVisitFn_t visitor, void *context);
};

/* --- adt_ForEach / abstractDataType_ForEach ------------------------------- */

/**
 * @brief Applies a read-only operation to every element.
 * @param adt Initialized container.
 * @param visitor Operation called for each element.
 * @param context Caller-provided operation state.
 * @return true on success; otherwise false.
 */
#define abstractDataType_ForEach adt_ForEach
bool adt_ForEach(const ADT_t *adt, ADT_ConstVisitFn_t visitor, void *context);

/* --- adt_ForEachMutable / abstractDataType_ForEachMutable ----------------- */

/**
 * @brief Applies a mutable operation to every stored element.
 * @param[in,out] adt Initialized container.
 * @param visitor Operation called for each element.
 * @param context Caller-provided operation state.
 * @return true on success; otherwise false.
 */
#define abstractDataType_ForEachMutable adt_ForEachMutable
bool adt_ForEachMutable(ADT_t *adt, ADT_MutableVisitFn_t visitor, void *context);

/* --- adt_Print / abstractDataType_Print ----------------------------------- */

/**
 * @brief Prints a container using its configured element printer.
 * @param adt Initialized container.
 * @return true on success; otherwise false.
 */
#define abstractDataType_Print adt_Print
bool adt_Print(const ADT_t *adt);

/* --- adt_PrintDebug / abstractDataType_PrintDebug ------------------------- */

/**
 * @brief Prints shared container state and call-site information.
 * @param adt Container to inspect, or NULL.
 * @param expression Source expression text.
 * @param file Source filename.
 * @param line Source line.
 * @return Nothing.
 */
#define abstractDataType_PrintDebug adt_PrintDebug
void adt_PrintDebug(const ADT_t *adt, const char *expression, const char *file, int line);

/* --- adt_Size / abstractDataType_Size ------------------------------------- */

/**
 * @brief Gets a container's element count.
 * @param adt Container to inspect, or NULL.
 * @return The element count, or zero for NULL.
 */
#define abstractDataType_Size adt_Size
static inline size_t adt_Size(const ADT_t *adt)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;
    return super == NULL ? 0 : super->_private.size;
}

/* --- adt_IsEmpty / abstractDataType_IsEmpty ------------------------------- */

/**
 * @brief Checks whether a container is empty.
 * @param adt Container to inspect, or NULL.
 * @return true for NULL or empty state; otherwise false.
 */
#define abstractDataType_IsEmpty adt_IsEmpty
static inline bool adt_IsEmpty(const ADT_t *adt)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;
    return super == NULL || super->_private.size == 0;
}

/* --- adt_ElementType / abstractDataType_ElementType ----------------------- */

/**
 * @brief Gets a container's element type information.
 * @param adt Container to inspect, or NULL.
 * @return The container-owned element type information, or NULL.
 */
#define abstractDataType_ElementType adt_ElementType
static inline const ADT_ElementTypeInfo_t *adt_ElementType(const ADT_t *adt)
{
    const ADT_Super_t *super = (const ADT_Super_t *)adt;
    return super == NULL ? NULL : &super->_private.elementType;
}

/* --- adt_Min / abstractDataType_Min --------------------------------------- */

/**
 * @brief Copies the smallest element selected by the configured comparator.
 * @param adt Initialized nonempty container.
 * @param[out] outElement Destination for the shallow element copy.
 * @return true on success; otherwise false.
 */
#define abstractDataType_Min adt_Min
bool adt_Min(const ADT_t *adt, void *outElement);

/* --- adt_MinBy / abstractDataType_MinBy ----------------------------------- */

/**
 * @brief Copies the smallest element selected by a comparator.
 * @param adt Initialized nonempty container.
 * @param compare Comparator used for this operation.
 * @param[out] outElement Destination for the shallow element copy.
 * @return true on success; otherwise false.
 */
#define abstractDataType_MinBy adt_MinBy
bool adt_MinBy(const ADT_t *adt, CompareFn_t compare, void *outElement);

/* --- adt_Max / abstractDataType_Max --------------------------------------- */

/**
 * @brief Copies the largest element selected by the configured comparator.
 * @param adt Initialized nonempty container.
 * @param[out] outElement Destination for the shallow element copy.
 * @return true on success; otherwise false.
 */
#define abstractDataType_Max adt_Max
bool adt_Max(const ADT_t *adt, void *outElement);

/* --- adt_MaxBy / abstractDataType_MaxBy ----------------------------------- */

/**
 * @brief Copies the largest element selected by a comparator.
 * @param adt Initialized nonempty container.
 * @param compare Comparator used for this operation.
 * @param[out] outElement Destination for the shallow element copy.
 * @return true on success; otherwise false.
 */
#define abstractDataType_MaxBy adt_MaxBy
bool adt_MaxBy(const ADT_t *adt, CompareFn_t compare, void *outElement);

/* --- adt_Mean / abstractDataType_Mean ------------------------------------- */

/**
 * @brief Calculates the arithmetic mean using the configured projection.
 * @param adt Initialized nonempty container.
 * @param[out] outMean Calculated mean.
 * @return true on success; otherwise false.
 */
#define abstractDataType_Mean adt_Mean
bool adt_Mean(const ADT_t *adt, double *outMean);

/* --- adt_MeanBy / abstractDataType_MeanBy --------------------------------- */

/**
 * @brief Calculates the arithmetic mean using a projection override.
 * @param adt Initialized nonempty container.
 * @param toNumber Numeric projection for this operation.
 * @param[out] outMean Calculated mean.
 * @return true on success; otherwise false.
 */
#define abstractDataType_MeanBy adt_MeanBy
bool adt_MeanBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMean);

/* --- adt_Median / abstractDataType_Median --------------------------------- */

/**
 * @brief Calculates the median using the configured numeric projection.
 * @param adt Initialized nonempty container.
 * @param[out] outMedian Calculated median.
 * @return true on success; otherwise false.
 */
#define abstractDataType_Median adt_Median
bool adt_Median(const ADT_t *adt, double *outMedian);

/* --- adt_MedianBy / abstractDataType_MedianBy ----------------------------- */

/**
 * @brief Calculates the median using a numeric projection override.
 * @param adt Initialized nonempty container.
 * @param toNumber Numeric projection for this operation.
 * @param[out] outMedian Calculated median.
 * @return true on success; otherwise false.
 */
#define abstractDataType_MedianBy adt_MedianBy
bool adt_MedianBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMedian);

/* --- adt_Mode / abstractDataType_Mode ------------------------------------- */

/**
 * @brief Finds the repeated mode using the configured numeric projection.
 * @param adt Initialized nonempty container.
 * @param[out] outMode Smallest mode when multiple values tie.
 * @return true when a repeated mode exists; otherwise false.
 */
#define abstractDataType_Mode adt_Mode
bool adt_Mode(const ADT_t *adt, double *outMode);

/* --- adt_ModeBy / abstractDataType_ModeBy --------------------------------- */

/**
 * @brief Finds the repeated mode using a numeric projection override.
 * @param adt Initialized nonempty container.
 * @param toNumber Numeric projection for this operation.
 * @param[out] outMode Smallest mode when multiple values tie.
 * @return true when a repeated mode exists; otherwise false.
 */
#define abstractDataType_ModeBy adt_ModeBy
bool adt_ModeBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMode);

/* --- adt_Sort / abstractDataType_Sort ------------------------------------- */

/**
 * @brief Sorts a container using its configured comparator.
 * @param[in,out] adt Initialized container.
 * @param algorithm Sorting algorithm to use.
 * @return true on success; otherwise false.
 */
#define abstractDataType_Sort adt_Sort
bool adt_Sort(ADT_t *adt, ADT_SortAlgorithm_t algorithm);

/* --- adt_SortBy / abstractDataType_SortBy --------------------------------- */

/**
 * @brief Sorts a container using a comparator override.
 * @param[in,out] adt Initialized container.
 * @param algorithm Sorting algorithm to use.
 * @param compare Comparator used for this operation.
 * @return true on success; otherwise false.
 */
#define abstractDataType_SortBy adt_SortBy
bool adt_SortBy(ADT_t *adt, ADT_SortAlgorithm_t algorithm, CompareFn_t compare);

/* --- adt_isSorted / abstractDataType_isSorted ---------------------------- */

/**
 * @brief Checks whether a container is ordered by its configured comparator.
 * @param adt Initialized container.
 * @return true when the container is sorted; otherwise false.
 */
#define abstractDataType_isSorted adt_isSorted
bool adt_isSorted(const ADT_t *adt);

/* --- adt_isSortedBy / abstractDataType_isSortedBy ------------------------ */

/**
 * @brief Checks whether a container is ordered by a comparator override.
 * @param adt Initialized container.
 * @param compare Comparator defining the expected order.
 * @return true when the container is sorted; otherwise false.
 */
#define abstractDataType_isSortedBy adt_isSortedBy
bool adt_isSortedBy(const ADT_t *adt, CompareFn_t compare);

#ifdef __cplusplus
}
#endif

/**
 * @brief Counts elements in a fixed-size C array.
 * @param array Array object, not a pointer.
 * @return Its element count.
 */
#define ARRAY_COUNT(array) \
    (sizeof(array) / sizeof((array)[0]))

/**
 * @brief Prints shared debug information for a container variable.
 * @param adt Container variable, not a pointer.
 * @return Nothing.
 */
#define ADT_DEBUG(adt) \
    adt_PrintDebug(&(adt), #adt, __FILE__, __LINE__)

#endif
