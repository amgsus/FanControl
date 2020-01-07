/*
 * By:              A.G.
 * Created:         2020.01.01
 * Last modified:   2020.01.06
 */

#include "eeprom.h"
#include "defines.h"
#include "macros.h"
#include "crc.h"

// -----------------------------------------------------------------------------

SEG_EEPROM EEPROM_UNION g_EEPROMStorage[EEPROM_NUM_OF_RECORDS]; // Do NOT initialize!

// -----------------------------------------------------------------------------

#define FLASH_IAPSR_DUL     BIT(3)
#define FLASH_IAPSR_EOP     BIT(2)
#define FLASH_IAPSR_HVOFF   BIT(6)
#define FLASH_CR2_ERASE     BIT(5)
#define FLASH_NCR2_NERASE   BIT(5)

BOOL
EraseEEPROMPage(WORD startAddress)
{
    BYTE u8;

    if (EEPROM_BASE_ADDRESS <= startAddress && startAddress <= EEPROM_MAX_USER_ADDRESS) {
        startAddress &= ~(EEPROM_PAGE_SIZE - 1); // Force zero address of block.

        if (!(FLASH_IAPSR & FLASH_IAPSR_DUL)) {
            UnlockEEPROM();
        }

        FLASH_CR2 |= FLASH_CR2_ERASE;
        FLASH_NCR2 &= ~FLASH_NCR2_NERASE;

        _MEM_(startAddress)     = 0;
        _MEM_(startAddress + 1) = 0;
        _MEM_(startAddress + 2) = 0;
        _MEM_(startAddress + 3) = 0;

        do {
            u8 = FLASH_IAPSR; // Clears EOP after read.
        } while (!(u8 & FLASH_IAPSR_EOP)); // End of operation.

        return 1;
    }
    return 0;
}

#define FLASH_DUKR_KEY1 0xAE
#define FLASH_DUKR_KEY2 0x56

void
UnlockEEPROM(void)
{
    do {
        FLASH_DUKR = FLASH_DUKR_KEY1;
        FLASH_DUKR = FLASH_DUKR_KEY2;
    } while (!(FLASH_IAPSR & FLASH_IAPSR_DUL));
}
