/*
 * By:              A.G.
 * Created:         2020.01.06
 * Last modified:   2020.01.06
 */

#ifndef UTILS_H
#define UTILS_H

// ----------------------------------------------------------------------------

#include "__TYPES.h"

/**
* Checks if the specified memory area is free and ready for writing (i.e. all
* memory cells are erased).
*
* @param addr
*      A pointer to the start of a memory block.
* @param length
*      The number of bytes to test.
* @return
*      Returns 1 only if all memory cells in the specified area are erased.
*/
BOOL
BlankCheck(LPVOID addr, WORD length);

#define IS_ODD(num) ((num) & 1)
#define IS_EVEN(num) (!IS_ODD(num))

# endif // ------------------------------------------- End of Include Guard ---
