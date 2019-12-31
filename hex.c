#include "__TYPES.h"

CHAR
d2x(BYTE xdi)
{
    if (xdi < 10)
        return '0' + (xdi);
    else if (xdi < 16)
        return 'A' + (xdi - 10); // Brackets are mandatory!
    return '\0';
}

BYTE
x2d(CHAR xch)
{
    if (xch >= '0' && xch <= '9')
        return xch - '0';
    if (xch >= 'A' && xch <= 'F')
        return xch - 'A' + 10;
    if (xch >= 'a' && xch <= 'f')
        return xch - 'a' + 10;
    return 0xFF;
}
