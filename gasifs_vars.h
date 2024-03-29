/*
 * By:              A.G.
 * Created:         2019.12.31
 * Last modified:   2020.01.01
 */

#ifndef GASIFS_VARS_H
#define GASIFS_VARS_H

// ----------------------------------------------------------------------------

#include "gasifs.h"
#include "macros.h"

// ---------------------------------- Diagnostic Report Registers 0x00-0x0F ---

/* Num of diagnostic registers (1 <= N <= 16) */
#define NUM_DIAG  4

#define DIAG_BASE_OFFSET 0x00
#define DIAG_MAX         0x0F

#define REG_REP_PWM   ( DIAG_BASE_OFFSET + 0x00 ) /* PWM */
#define REG_REP_RPM   ( DIAG_BASE_OFFSET + 0x01 ) /* RPM */
#define REG_REP_PER   ( DIAG_BASE_OFFSET + 0x02 ) /* Period */
#define REG_REP_DCIN  ( DIAG_BASE_OFFSET + 0x03 ) /* DC IN voltage (mV) */

// ---------------------------------------------- Param Registers 0x10-0xEF ---

#define PARM_MAX         0xE0
#define PARM_BASE_OFFSET 0x10

/**
 * Number of non-volatile parameters (1 <= N <= 224)
 */
#define NUM_PARM  8     /* See also MAX_STORED_PARAMETERS */

#define REG_PAR_BUID  ( PARM_BASE_OFFSET + 0x00 ) /* Device BUID */
#define REG_PAR_DUTY  ( PARM_BASE_OFFSET + 0x01 ) /* Sets duty cycle on device startup (.1%) */
#define REG_PAR_FREQ  ( PARM_BASE_OFFSET + 0x02 ) /* PWM frequency (.1kHz) */
#define REG_PAR_MODE  ( PARM_BASE_OFFSET + 0x03 ) /* Working mode */
#define REG_PAR_TDIV  ( PARM_BASE_OFFSET + 0x04 ) /* Tachometer pulse divider */
#define REG_PAR_VSCL  ( PARM_BASE_OFFSET + 0x05 ) /* DC IN scale (.01) */
#define REG_PAR_VOFS  ( PARM_BASE_OFFSET + 0x06 ) /* DC IN offset */

// Reserved for future use:

#define REG_PAR_XXXX  ( PARM_BASE_OFFSET + 0x07 ) /* Reserved */

// ------------------------------ Information / Control Registers 0xF0-0xFF ---

/* Num of info registers (1 <= N <= 16) */
#define NUM_INFO  5

#define INFO_BASE_OFFSET 0xF0
#define INFO_MAX         0x0F

#define REG_NFO_HWID  ( INFO_BASE_OFFSET + 0x00 ) /* Hardware ID */
#define REG_NFO_FWVR  ( INFO_BASE_OFFSET + 0x01 ) /* Firmware version */
#define REG_NFO_FLAG  ( INFO_BASE_OFFSET + 0x02 ) /* Flags */
#define REG_NFO_TICK  ( INFO_BASE_OFFSET + 0x03 ) /* Time keeper (milliseconds) */
#define REG_NFO_REGS  ( INFO_BASE_OFFSET + 0x04 ) /* Register count: MSB [lonibble=REP, hinibble=NFO]; LSB [PAR]. */

// ----------------------------------------------------------------------------

#define REGISTER_COUNT (NUM_DIAG + NUM_PARM + NUM_INFO)
#define REGISTER_COUNT_ENCODED ( ((NUM_INFO & 0x0F) << 12) | ((NUM_INFO & 0x0F) << 8) | (NUM_INFO & 0x0F) | NUM_PARM )

extern GASIFSVAL g_GasifsVars[]; // Defined in 'gasifs_hdls.c'
extern BOOLEAN   g_ParamsUpdateEvent;
extern WORD      g_ParamKey;
extern BOOLEAN   g_ResetRequested;
extern WORD      g_LastParamChanged;

#define GASIFSVAR(index) (g_GasifsVars[(index)])
#define SET_GASIFSVAR(index, value) do { g_GasifsVars[(index)] = (GASIFSVAL)(value); } while (0)

// Macros for accessing Gasifs registers (reg - is an absolute value, i.e. REG_PAR_BUID, REG_NFO_HWID, etc.).

#define GASIFSVAR_DIAG(reg) \
    GASIFSVAR(reg)

#define SET_GASIFSVAR_DIAG(reg, value) \
    SET_GASIFSVAR((reg), (value))

#define GASIFSVAR_PARM(reg) \
    GASIFSVAR(NUM_DIAG + ((reg) - PARM_BASE_OFFSET))

#define SET_GASIFSVAR_PARM(reg, value) \
    SET_GASIFSVAR((NUM_DIAG + ((reg) - PARM_BASE_OFFSET)), (value))

#define GASIFSVAR_INFO(reg) \
    GASIFSVAR(NUM_DIAG + NUM_PARM + ((reg) - INFO_BASE_OFFSET))

#define SET_GASIFSVAR_INFO(reg, value) \
    SET_GASIFSVAR((NUM_DIAG + NUM_PARM + ((reg) - INFO_BASE_OFFSET)), (value))

// --------------------------------------------- Parameters Load/Store Keys ---

// Write to REG_NFO_REGS:

#define PARAM_CTRL_KEY(chhi, chlo) ((((WORD)(chhi)) << 8) | ((WORD)(chlo)))

/* [O][K]      = 0x4F4B */
#define KEY_STORE_PARAMETERS            PARAM_CTRL_KEY('O','K')
/* [L]oa[D]    = 0x4C44 */
#define KEY_LOAD_STORED_PARAMETERS      PARAM_CTRL_KEY('L','D')
/* [R][E]store = 0x5245 */
#define KEY_RESTORE_DEFAULT_PARAMETERS  PARAM_CTRL_KEY('R','E')

// --------------------------------------------------- Bits of REG_NFO_FLAG ---

#define FLAG_FLASH_PARAM_LOADED         BIT(0)
#define FLAG_FLASH_WRITE_OK             BIT(1)

# endif // ------------------------------------------- End of Include Guard ---
