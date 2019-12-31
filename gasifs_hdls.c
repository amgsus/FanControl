#include "gasifs.h"
#include "gasifs_port.h"

#include "main.h"
#include "hex.h"
#include "gasifs_vars.h"

// ------------------------------------------------------- Global variables ---

GASIFSVAL g_GasifsVars[NUM_DIAG + NUM_INFO + NUM_PARM];
BOOLEAN   g_ParamsUpdateEvent = FALSE;
WORD      g_ParamKey          = 0;
BOOLEAN   g_ResetRequested    = FALSE;
WORD      g_LastParamChanged  = 0; // 0 - undetermined.

// ----------------------------------------------------------------------------

void
GasifsHCMD(GASIFSCMD cmd)
{
    static BYTE di  = 0; // Rotation index of report.
    GASIFSACK   ackValue;
    BOOL        ack = TRUE;

    switch (cmd) {
        case 0x8000: // TODO: Device off. Note: not all fans can be stopped fully.
            break;
        case 0x8001: // TODO: Device on.
            break;
        case 0xFFF0: // Poll diagnostic registers.
            break;
        default:
            ack = (cmd < 0x8000) ? SetTask(cmd) : FALSE; // SetTask in 'main.c'
            break;
    }

    if (ack) {
        ackValue.diagn.reg = di;
        ackValue.diagn.val = GASIFSVAR(DIAG_BASE_OFFSET + di);
        GasifsAcknoledge(ackValue);
        di++;
        if (di == NUM_DIAG) {
            di = 0;
        }
    } else {
        GasifsNACK();
    }
}

// ----------------------------------------------------------------------------

void
GasifsRREG(GASIFSREG reg)
{
    GASIFSACK ack;

    if (reg <= 0x0F) {
        if (reg >= NUM_DIAG) {
            goto noSuchRegister;
        }
        ack.regRW.val = GASIFSVAR_DIAG(reg);
    } else if (reg >= 0xF0) {
        if (reg - 0xF0 >= NUM_INFO) {
            goto noSuchRegister;
        }
        ack.regRW.val = GASIFSVAR_INFO(reg);
    } else {
        if (reg - 0x10 >= NUM_PARM) {
            goto noSuchRegister;
        }
        ack.regRW.val = GASIFSVAR_PARM(reg);
    }

    ack.regRW.reg = reg;
    GasifsAcknoledge(ack);
    return;

noSuchRegister:
    GasifsNACK();
}

// ----------------------------------------------------------------------------

void
GasifsWREG(GASIFSREG reg, GASIFSVAL val)
{
    GASIFSACK ack;

    g_LastParamChanged = 0;

    if (reg <= 0x0F) {
        goto writeForbidden;
    } else if (reg >= 0xF0) {
        if (reg == REG_NFO_HWID && val == RESET_MAGIC_NUMBER) { // Request for reboot?
            g_ResetRequested = TRUE;
            ack.regRW.val = val;
        } else if (reg == REG_NFO_REGS && (val == KEY_STORE_PARAMETERS || val == KEY_LOAD_STORED_PARAMETERS || val == KEY_RESTORE_DEFAULT_PARAMETERS)) { // Operation under PARM registers?
            g_ParamKey = (WORD) val;
            ack.regRW.val = val;
        } else {
            goto writeForbidden;
        }
    } else {
        if (reg - 0x10 >= NUM_PARM) {
            goto noSuchRegister;
        }
        if (ValidateParameterValue(reg, val)) {
            SET_GASIFSVAR_PARM(reg, val);
            g_LastParamChanged = reg;
            g_ParamsUpdateEvent = TRUE;
        }
        ack.regRW.val = GASIFSVAR_PARM(reg);
    }

    ack.regRW.reg = reg;
    GasifsAcknoledge(ack);
    return;

writeForbidden:
noSuchRegister:
    GasifsNACK();
}

// ----------------------------------------------------------------------------

BOOLEAN
ValidateParameterValue(GASIFSREG reg, GASIFSVAL val)
{
    switch (reg) {
        case REG_PAR_BUID:
            return (val >= 'G' && val <= 'Z');
        case REG_PAR_FREQ:
            return (val >= (MIN_PWM_FREQUENCY / 100U) && val <= (MAX_PWM_FREQUENCY / 100U));
        default:
            return (FALSE);
    }
}
