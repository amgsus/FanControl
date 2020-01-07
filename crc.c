/*
 * By:              A.G.
 * Created:         2017.11.27
 * Last modified:   2020.01.04
 */

#include "crc.h"

BYTE
ComputeCRC8(BYTE b, BYTE accum)
{
    BYTE i;
    BYTE data;
    data = accum ^ b;
    for ( i = 0; i < 8; i++ ) {
        if (data & 0x80) {
            data <<= 1;
            data  ^= 0x07;
        } else {
            data <<= 1;
        }
    }
    return data;
}

BYTE
ComputeBlockCRC8(LPCVOID buffer, WORD n, BYTE accum)
{
    LPBYTE p;
    p = (LPBYTE) buffer;
    while (n > 0) {
        accum = ComputeCRC8(accum, *p++);
        n--;
    }
    return accum;
}
