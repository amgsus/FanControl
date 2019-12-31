#ifndef HEX_H
#define HEX_H

#include "__TYPES.h"

/**
 * Converts a decimal into a single hexadecimal character.
 *
 * @param xdi
 *      Input (0..15).
 * @return
 *      Returns a hexadecimal character matching the input value ('0'..'9',
 *      'A'..'F'), or NULL character if the value is greater than 15.
 */
CHAR d2x(BYTE xdi);

/**
 * Converts a hexadecimal character into a decimal.
 *
 * @param xch
 *      Input ('0'..'9', 'A'..'Z').
 * @return
 *      Returns a matched decimal value (from 0 to 15), or 255 if the input is
 *      not a valid hexadecimal character.
 */
BYTE x2d(CHAR xch);

# endif // ------------------------------------------- End of Include Guard ---
