/*
 * By:              A.G.
 * Created:         2017.11.27
 * Last modified:   2020.01.04
 */

#ifndef CRC8_NO_TABLE_H
#define CRC8_NO_TABLE_H

#include "__TYPES.h"

#define CRC8_INITIAL_ACCUM 0xFF

BYTE
ComputeCRC8(BYTE b, BYTE accum);

BYTE
ComputeBlockCRC8(LPCVOID buffer, WORD n, BYTE accum);

# endif // ------------------------------------------- End of Include Guard ---
