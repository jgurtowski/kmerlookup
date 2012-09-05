#ifndef MISC_FUNC_H
#define MISC_FUNC_H

/**
 * Find max element in array given accessor and comparator
 *@param arr to find max value in
 *@param len length of array
 *@param accessor Accessor function to pull comparable elements from the array
 *@param comparator Comparator function to compare the elements returned by accessor
 *@return index in arr that contains the maximum element
 */
int array_max(const void *arr, int len, const void * (*accessor)(const void *arr, int i), int (*comparator)(const void *a, const void *b));


/**
 *Pre defined accessor for an int array
 */
const void *int_accessor(const void *arr, int i);

/**
 *Pre defined comparator for int elements
 */
int int_comparator(const void *aa, const void *bb);

/**
 *Pre defined comparator for float elements
 */
int float_comparator(const void *aa, const void *bb);

#endif
