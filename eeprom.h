/*
 * By:              A.G.
 * Created:         2020.01.01
 * Last modified:   2020.01.06
 */

#ifndef STM8S103F3_EEPROM_H
#define STM8S103F3_EEPROM_H

#include "__TYPES.h"
#include "mcu.h"

// ------------------------------------------------------- EEPROM Definition ---

#define EEPROM_BASE_ADDRESS             0x4000U
#define EEPROM_MAX_USER_ADDRESS         0x427FU
#define EEPROM_PAGE_SIZE                0x40U   /* 64 bytes for STM8S103F3 */
// #define EEPROM_NUM_OF_PAGES_AVAILABLE   ((EEPROM_MAX_USER_ADDRESS - EEPROM_BASE_ADDRESS + 1) / EEPROM_PAGE_SIZE) /* STM8S103F3 = ((EEPROM_MAX_USER_ADDRESS - EEPROM_BASE_ADDRESS + 1) : EEPROM_BLOCK_SIZE) */
#define EEPROM_NUM_OF_PAGES_AVAILABLE   2 /* STM8S103F3 = ((EEPROM_MAX_USER_ADDRESS - EEPROM_BASE_ADDRESS + 1) : EEPROM_BLOCK_SIZE) */

// ------------------------------------------------------------------- Utils ---

BOOL
EraseEEPROMPage(WORD startAddress);

#define LockEEPROM() \
    do { FLASH_IAPSR &= ~FLASH_IAPSR_DUL; } while (0)

void
UnlockEEPROM(void);

// ------------------------------------------------ Data Structure in EEPROM ---

#define MAX_STORED_PARAMETERS   8
#define SIZE_OF_STORED_PARAMETERS_ARRAY (MAX_STORED_PARAMETERS * sizeof(WORD))

#define RECORD_MARK_VALID       0xA5
#define RECORD_MARK_DELETED     0xAA

typedef struct {
    // Keep the order of fields.
    // Header:
    BYTE valid; // Only valid pattern.
    BYTE deleted; // Any value different from the one of an erased cell -> error. Valid pattern -> deleted.
    BYTE checksum;
    BYTE reserved[1]; // The value of this cell should be always erased.
    // Data:
    WORD param[MAX_STORED_PARAMETERS];
} EEPROM_DATA_RECORD, *EEPROM_DATA_RECORD_PTR;

typedef const EEPROM_DATA_RECORD* EEPROM_CONST_DATA_RECORD_PTR;

#define RECORD_SIZE (EEPROM_PAGE_SIZE / 2)

// Using the union for aligning data in the array.
typedef union {
    EEPROM_DATA_RECORD record;
    BYTE               raw[RECORD_SIZE];
} EEPROM_UNION, *EEPROM_UNION_PTR;

#define EEPROM_NUM_OF_RECORDS (EEPROM_NUM_OF_PAGES_AVAILABLE * EEPROM_PAGE_SIZE / RECORD_SIZE)

/**
 * The whole EEPROM segment mapped to a typed array.
 *
 * !!! UNPROTECT EEPROM (DATA) SEGMENT BEFORE WRITE !!!
 */
extern SEG_EEPROM EEPROM_UNION g_EEPROMStorage[EEPROM_NUM_OF_RECORDS];

#define NO_RECORD_FOUND 0xFF

#define ERASED_CELL_VALUE 0x00

#endif // --------------------------------------------- End of Include Guard ---
