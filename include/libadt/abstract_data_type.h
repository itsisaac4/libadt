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
     * - `ADT_SORT_BOGO`: Bounded randomized demonstration sort.
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

    /**
     * @brief Applies a read-only operation to every element.
     * @param adt Initialized container.
     * @param visitor Operation called for each element.
     * @param context Caller-provided operation state.
     * @return true on success; otherwise false.
     */
    bool adt_ForEach(const ADT_t *adt, ADT_ConstVisitFn_t visitor, void *context);

    /**
     * @brief Applies a mutable operation to every stored element.
     * @param[in,out] adt Initialized container.
     * @param visitor Operation called for each element.
     * @param context Caller-provided operation state.
     * @return true on success; otherwise false.
     */
    bool adt_ForEachMutable(ADT_t *adt, ADT_MutableVisitFn_t visitor, void *context);

    /**
     * @brief Prints a container using its configured element printer.
     * @param adt Initialized container.
     * @return true on success; otherwise false.
     */
    bool adt_Print(const ADT_t *adt);

    /**
     * @brief Prints shared container state and call-site information.
     * @param adt Container to inspect, or NULL.
     * @param expression Source expression text.
     * @param file Source filename.
     * @param line Source line.
     * @return Nothing.
     */
    void adt_PrintDebug(const ADT_t *adt, const char *expression, const char *file, int line);

    /**
     * @brief Gets a container's element count.
     * @param adt Container to inspect, or NULL.
     * @return The element count, or zero for NULL.
     */
    static inline size_t adt_Size(const ADT_t *adt)
    {
        const ADT_Super_t *super = (const ADT_Super_t *)adt;
        return super == NULL ? 0 : super->_private.size;
    }

    /**
     * @brief Checks whether a container is empty.
     * @param adt Container to inspect, or NULL.
     * @return true for NULL or empty state; otherwise false.
     */
    static inline bool adt_IsEmpty(const ADT_t *adt)
    {
        const ADT_Super_t *super = (const ADT_Super_t *)adt;
        return super == NULL || super->_private.size == 0;
    }

    /**
     * @brief Gets a container's element type information.
     * @param adt Container to inspect, or NULL.
     * @return The container-owned element type information, or NULL.
     */
    static inline const ADT_ElementTypeInfo_t *adt_ElementType(const ADT_t *adt)
    {
        const ADT_Super_t *super = (const ADT_Super_t *)adt;
        return super == NULL ? NULL : &super->_private.elementType;
    }

    /**
     * @brief Copies the smallest element selected by the configured comparator.
     * @param adt Initialized nonempty container.
     * @param[out] outElement Destination for the shallow element copy.
     * @return true on success; otherwise false.
     */
    bool adt_Min(const ADT_t *adt, void *outElement);

    /**
     * @brief Copies the smallest element selected by a comparator.
     * @param adt Initialized nonempty container.
     * @param compare Comparator used for this operation.
     * @param[out] outElement Destination for the shallow element copy.
     * @return true on success; otherwise false.
     */
    bool adt_MinBy(const ADT_t *adt, CompareFn_t compare, void *outElement);

    /**
     * @brief Copies the largest element selected by the configured comparator.
     * @param adt Initialized nonempty container.
     * @param[out] outElement Destination for the shallow element copy.
     * @return true on success; otherwise false.
     */
    bool adt_Max(const ADT_t *adt, void *outElement);

    /**
     * @brief Copies the largest element selected by a comparator.
     * @param adt Initialized nonempty container.
     * @param compare Comparator used for this operation.
     * @param[out] outElement Destination for the shallow element copy.
     * @return true on success; otherwise false.
     */
    bool adt_MaxBy(const ADT_t *adt, CompareFn_t compare, void *outElement);

    /**
     * @brief Calculates the arithmetic mean using the configured projection.
     * @param adt Initialized nonempty container.
     * @param[out] outMean Calculated mean.
     * @return true on success; otherwise false.
     */
    bool adt_Mean(const ADT_t *adt, double *outMean);

    /**
     * @brief Calculates the arithmetic mean using a projection override.
     * @param adt Initialized nonempty container.
     * @param toNumber Numeric projection for this operation.
     * @param[out] outMean Calculated mean.
     * @return true on success; otherwise false.
     */
    bool adt_MeanBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMean);

    /**
     * @brief Calculates the median using the configured numeric projection.
     * @param adt Initialized nonempty container.
     * @param[out] outMedian Calculated median.
     * @return true on success; otherwise false.
     */
    bool adt_Median(const ADT_t *adt, double *outMedian);

    /**
     * @brief Calculates the median using a numeric projection override.
     * @param adt Initialized nonempty container.
     * @param toNumber Numeric projection for this operation.
     * @param[out] outMedian Calculated median.
     * @return true on success; otherwise false.
     */
    bool adt_MedianBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMedian);

    /**
     * @brief Finds the repeated mode using the configured numeric projection.
     * @param adt Initialized nonempty container.
     * @param[out] outMode Smallest mode when multiple values tie.
     * @return true when a repeated mode exists; otherwise false.
     */
    bool adt_Mode(const ADT_t *adt, double *outMode);

    /**
     * @brief Finds the repeated mode using a numeric projection override.
     * @param adt Initialized nonempty container.
     * @param toNumber Numeric projection for this operation.
     * @param[out] outMode Smallest mode when multiple values tie.
     * @return true when a repeated mode exists; otherwise false.
     */
    bool adt_ModeBy(const ADT_t *adt, ToNumberFn_t toNumber, double *outMode);

    /**
     * @brief Sorts a container using its configured comparator.
     * @param[in,out] adt Initialized container.
     * @param algorithm Sorting algorithm to use.
     * @return true on success; otherwise false.
     */
    bool adt_Sort(ADT_t *adt, ADT_SortAlgorithm_t algorithm);

    /**
     * @brief Sorts a container using a comparator override.
     * @param[in,out] adt Initialized container.
     * @param algorithm Sorting algorithm to use.
     * @param compare Comparator used for this operation.
     * @return true on success; otherwise false.
     */
    bool adt_SortBy(ADT_t *adt, ADT_SortAlgorithm_t algorithm, CompareFn_t compare);

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
