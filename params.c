/*
 * By:              A.G.
 * Created:         2020.01.01
 * Last modified:   2020.01.07
 */

#include "params.h"
#include "gasifs_vars.h"
#include "version.h"
#include "eeprom.h"
#include "crc.h"
#include "utils.h"

// -----------------------------------------------------------------------------

#define RPM_PRSC_1 1
#define RPM_PRSC_2 2
#define RPM_PRSC_3 3
#define RPM_PRSC_4 4

#define PAR_MODE_OFF 0
#define PAR_MODE_ALWAYS_ON 1

PRIVATE BYTE
LocateStoredParametersSet(void);

PRIVATE BYTE
GetNextIndex(BYTE indexNow);

PRIVATE BYTE p_LastStoredRecordIndex = NO_RECORD_FOUND;

// -----------------------------------------------------------------------------

void
SetInfoRegisters(BYTE resetStatus)
{
    SET_GASIFSVAR_INFO(REG_NFO_HWID, HW_TAG);
    SET_GASIFSVAR_INFO(REG_NFO_FWVR, FW_VERSION);
    SET_GASIFSVAR_INFO(REG_NFO_FLAG, 0);
    SET_GASIFSVAR_INFO(REG_NFO_TICK, 0);
    SET_GASIFSVAR_INFO(REG_NFO_REGS, REGISTER_COUNT_ENCODED);
}

void
LoadDefaultParameters(void)
{
    SET_GASIFSVAR_PARM(REG_PAR_BUID, BUS_ID);
    SET_GASIFSVAR_PARM(REG_PAR_DUTY, 500);          // 50.0%
    SET_GASIFSVAR_PARM(REG_PAR_FREQ, 150);          // 15.0 kHz
    SET_GASIFSVAR_PARM(REG_PAR_TDIV, RPM_PRSC_2);   // 2
    SET_GASIFSVAR_PARM(REG_PAR_MODE, PAR_MODE_OFF); // OFF
    SET_GASIFSVAR_PARM(REG_PAR_VSCL, 303);          // 0.303 (39K:7K5) => (X * 0.1) V
    SET_GASIFSVAR_PARM(REG_PAR_VOFS, 0);            // 0
    SET_GASIFSVAR_PARM(REG_PAR_XXXX, 0);            // Keep 0 for reserved.
}

BOOL
LoadParameters(void)
{
    BYTE                         rec;
    EEPROM_CONST_DATA_RECORD_PTR p;
    BYTE                         i;

    if ((rec = LocateStoredParametersSet()) > EEPROM_NUM_OF_RECORDS) {
        // EEPROM is empty, or all data entries are invalid...
        LoadDefaultParameters();
        return 0;
    }

    p = &g_EEPROMStorage[rec].record;
    for ( i = 0; i < MAX_STORED_PARAMETERS; i++ ) {
        SET_GASIFSVAR_PARM(PARM_BASE_OFFSET + i, p->param[i]);
    }

    p_LastStoredRecordIndex = rec;
    return 1;
}

// Requires LoadParameters() to be called previously to update
// p_LastStoredRecordIndex module-private variable.
BOOL
SaveParameters(void)
{
    BYTE  i;
    BYTE  newIndex;
    BYTE  checksum;
    BYTE  diffs;

    // Force all reserved parameters to zero value.
    SET_GASIFSVAR_PARM(REG_PAR_XXXX, 0);

    // Look for any changes comparing to the current data stored.
    if (p_LastStoredRecordIndex != NO_RECORD_FOUND) {
        diffs = 0;
        for ( i = 0; i < MAX_STORED_PARAMETERS; i++ ) {
            if (g_EEPROMStorage[p_LastStoredRecordIndex].record.param[i] != GASIFSVAR_PARM(PARM_BASE_OFFSET + i)) {
                diffs = 1;
            }
        }
        if (!(diffs)) {
            return 0; // No changes.
        }
        newIndex = GetNextIndex(p_LastStoredRecordIndex);
    } else {
        newIndex = 0;
    }

    UnlockEEPROM();

    // Only for 32-byte records.
    if (IS_EVEN(newIndex)) {
        // Erase only non-blank pages (saves mem cell resource).
        if (!(BlankCheck(g_EEPROMStorage[newIndex].raw, RECORD_SIZE))) {
            EraseEEPROMPage((WORD) g_EEPROMStorage[newIndex].raw);
        }
    }

    // Copy all data.
    for ( i = 0; i < MAX_STORED_PARAMETERS; i++ ) {
        // Do not use short pointer to g_EEPROMStorage[newIndex].record, because compiler needs to see @eeprom attribute.
        g_EEPROMStorage[newIndex].record.param[i] = GASIFSVAR_PARM(PARM_BASE_OFFSET + i);
    }

    checksum = ComputeBlockCRC8(&(g_EEPROMStorage[newIndex].record.param[0]),
        SIZE_OF_STORED_PARAMETERS_ARRAY, 0xFF);

    // Commit record.
    g_EEPROMStorage[newIndex].record.checksum = checksum;
    g_EEPROMStorage[newIndex].record.valid = RECORD_MARK_VALID;

    // Mark the previous record as deleted.
    if (p_LastStoredRecordIndex != NO_RECORD_FOUND) {
        g_EEPROMStorage[p_LastStoredRecordIndex].record.deleted = RECORD_MARK_DELETED;
    }
    p_LastStoredRecordIndex = newIndex;

    return 1;
}

// -----------------------------------------------------------------------------

PRIVATE BYTE
LocateStoredParametersSet(void)
{
    BYTE  c = 0;
    BYTE  i;
    BYTE  indices[2];
    BYTE  x;

    for ( i = 0; i < EEPROM_NUM_OF_RECORDS; i++ ) {
        EEPROM_CONST_DATA_RECORD_PTR p = &(g_EEPROMStorage[i].record);
        if (p->valid == RECORD_MARK_VALID && p->deleted == ERASED_CELL_VALUE) {
            if (ComputeBlockCRC8(&(p->param[0]), SIZE_OF_STORED_PARAMETERS_ARRAY, 0xFF) == p->checksum) {
                indices[c++] = i;
                if (c == 2) {
                    if (indices[0] == 0 && indices[1] == (EEPROM_NUM_OF_RECORDS - 1)) { // Special write failure case (the 1st and the last positions are valid).
                        // The 1st (absolute) record is considered to be newer.
                        x = indices[0];
                        indices[0] = indices[1];
                        indices[1] = x;
                    }
                    // Cosmic inserts appropriate code to unlock EEPROM before any write operation,
                    // if @eeprom attribute is added to the variable declaration.
                    g_EEPROMStorage[indices[0]].record.deleted = RECORD_MARK_DELETED;
                    indices[0] = indices[1]; // Shift left.
                    c--;
                }
            }
        }
    }

    return (c == 1) ? indices[0] : NO_RECORD_FOUND;
}

PRIVATE BYTE
GetNextIndex(BYTE indexNow)
{
    if (indexNow >= EEPROM_NUM_OF_RECORDS - 1) {
        return 0;
    }
    return indexNow + 1;
}
