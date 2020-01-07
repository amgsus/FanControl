/*
 * By:              A.G.
 * Created:         2020.01.06
 * Last modified:   2020.01.06
 */

#include "utils.h"
#include "eeprom.h"

BOOL
BlankCheck(LPVOID addr, WORD length)
{
    LPBYTE p = (LPBYTE) addr;
    while (length > 0) {
        if (*p != ERASED_CELL_VALUE) {
            return 0;
        }
        length--;
        p++;
    }
    return 1;
}
