/*
 * *** Gasifs (STM8/C8051) ***
 *
 * Version: 1.1
 * By: A.G.
 * Created: 2018/11/19 03:29
 * Last modified: 2019/12/31 15:57
 */

#include "gasifs.h"
#include "gasifs_port.h"

#if   GASIFS_CMD_SIZE == 1
  /* 2 hexadecimal digits ( 8-bit) */
  #define CHARS_CMD 2
#elif GASIFS_CMD_SIZE == 2
  /* 4 hexadecimal digits (16-bit) */
  #define CHARS_CMD 4
#elif GASIFS_CMD_SIZE == 4
  /* 8 hexadecimal digits (32-bit) */
  #define CHARS_CMD 8
#else
  #error "Invalid definition for GASIFS_CMD_SIZE: expected 1 or 2 or 4."
#endif

#if   GASIFS_OLD_SIZE == 1
  #define CHARS_OLD 2
#elif GASIFS_OLD_SIZE == 2
  #define CHARS_OLD 4
#elif GASIFS_OLD_SIZE == 4
  #define CHARS_OLD 8
#else
  #error "Invalid definition for GASIFS_OLD_SIZE: expected 1 or 2 or 4."
#endif

#if   GASIFS_REG_SIZE == 1
  #define CHARS_REG 2
#elif GASIFS_REG_SIZE == 2
  #define CHARS_REG 4
#else
  #error "Invalid definition for GASIFS_REG_SIZE: expected 1 or 2."
#endif

#if   GASIFS_VAL_SIZE == 1
  #define CHARS_VAL 2
#elif GASIFS_VAL_SIZE == 2
  #define CHARS_VAL 4
#elif GASIFS_VAL_SIZE == 4
  #define CHARS_VAL 8
#else
  #error "Invalid definition for GASIFS_VAL_SIZE: expected 1 or 2 or 4."
#endif

PRIVATE enum FSM {
    S_WAITBUID, S_SELPROTO, S_READDATA, S_CMDREADY, S_WRITEACK, S_WRITNACK
} state; // State machine.

PRIVATE enum COMMAND {
    D_HCMD = GASIFS_HCMD, D_RREG = GASIFS_RREG, D_WREG = GASIFS_WREG
} dt; // Data format.

PRIVATE BYTE       ti;      // Index of byte transmitting.
PRIVATE CHAR       ts[16];  // Buffer for response data (BUID + '?'/'=' + 16-bit + 32-bit + '\0' + GUARD_AREA).
PRIVATE BYTE       cc;      // Char counter.
PRIVATE BYTE       hx;      // Checksum accumulator.
PRIVATE CHAR       bid;     // Received bus ID.
PRIVATE GASIFSREG  reg;     // Register index.
PRIVATE GASIFSVAL  val;     // Register value.
PRIVATE GASIFSCMD  cmd;     // Command.
PRIVATE BYTE       exe;     // For calling the handler only once.

void
GasifsInit(void)
{
    GasifsReset();
}

void
GasifsReset(void)
{
    state = S_WAITBUID;
    dt  = D_HCMD;
    cc  = 0;
    hx  = 0;
    ti  = 0;
    bid = 'Z'; // Doesn't matter.
    val = 0;
    reg = 0;
    cmd = 0;
    exe = 0;
    G_DATA_DIR(DIR_RX);
}

BOOL
GasifsIOTransportTask(BYTE reserved)
{
    CHAR  db;
    BOOL  rr;
    BOOL  wr;
    BYTE  dc;

    G_PRE();

    if (G_RI() || G_RXNE()) { // Combine appropriate conditions for STM8 and Silabs.
        G_RI_RST();
        db = G_GET();
        switch (state) {
            case S_WAITBUID:
                // Wait for bus ID of this device or for '@' (optional)...
                if (G_CHECK_DEVICE_BUID(db)) {
                    GasifsReset();
                    bid   = db;
                    state = S_SELPROTO;
                }
                break;
            case S_SELPROTO:
                // Determine the format of the receiving data...
                state = S_READDATA;
                wr    = (db == '=');
                rr    = (db == '?');
                if (wr) {
                    cc = CHARS_REG + CHARS_VAL;
                    dt = D_WREG;
                    break;
                }
                if (rr) {
                    cc = CHARS_REG;
                    dt = D_RREG;
                    break;
                }
                dt    = D_HCMD;
                cc    = CHARS_CMD;
            case S_READDATA:
                if (((db < '0') || (db > 'Z') || (db > '9' && db < 'A')) &&
                    (db != '_')) { // Reset FSM on any unexpected byte.
                    GasifsReset();
                } else {
                    if (cc) {
                        if (db == '_') {
                            GasifsReset();
                            break;
                        }
                        dc = G_X2D(db);
                        if (dt == D_HCMD) { // Parsing an old format value.
                            cmd <<= 4;
                            cmd |= dc;
                        } else {
                            if (dt == D_WREG && cc < (CHARS_VAL +
                                                      1)) { // Parsing register index or value?
                                val <<= 4;
                                val |= dc;
                            } else {
                                reg <<= 4;
                                reg |= dc;
                            }
                        }
                        hx ^= dc;
                        cc--;
                    } else {
                        if (db == '_') {
                            // Ignore checksum check...
                            state = S_CMDREADY;
                        } else {
                            dc = G_X2D(db);
                            if (dc != hx) {
                                GasifsReset(); // Checksum not match.
                                break;
                            } else {
                                state = S_CMDREADY;
                            }
                        }
                    }
                }
                break;
        }
    }

#ifdef GASIFS_C8051
    // !ti allows to bypass TI0 bit check if it was cleared previously.
    if (G_TI() || (!ti && cc)) {
        G_TI_RST();
        if (state == S_WRITEACK) {
            if (cc > 0) {
                G_PUT((BYTE) ts[ti++]);
                cc--;
            } else {
                GasifsReset();
            }
        }
    }
#elif defined(GASIFS_STM8)
    if (state == S_WRITEACK) {
        if (cc > 0) {
            if (G_TXE()) {
                G_PUT((BYTE) ts[ti++]);
                cc--;
            }
        } else if (G_TC()) { // Wait until all bytes are shifted out.
            GasifsReset();
        }
    }
#endif

    G_POST();

    return (state == S_CMDREADY);
}

BOOL
GasifsProcess(void)
{
    if (state == S_CMDREADY) {
        if (!(exe)) { // Multiple calls of this function will not call handlers more than one time until ACK or NACK (reset).
            exe = 1;
            switch (dt) {
                case D_HCMD:
                    GasifsHCMD(cmd);
                    break;
                case D_RREG:
                    GasifsRREG(reg);
                    break;
                case D_WREG:
                    GasifsWREG(reg, val);
                    break;
            }
        }
    }
    return (state == S_CMDREADY || state == S_WRITEACK);
}

#define CHARS_BUID_XXX_CRC 3

BYTE
GasifsAcknoledge(GASIFSACK ack)
{
#if GASIFS_VAL_SIZE == 4 || GASIFS_OLD_SIZE == 4
    DWORD  vl;
#else
    WORD   vl;
#endif
    BYTE   xx;
    BYTE   dc;
    BYTE   ix;
    CHAR  *si;
    BYTE   of;

    if (state != S_CMDREADY) {
        return 0;
    }

    if (bid < 'A' || bid > 'Z') { // If device was selected by issuing '@', then respond with the default BUID.
        bid = DEVICE_BUID;
    }

    ts[0] = bid + 32; // Convert to lower case.

    if (dt == D_HCMD) {
        if ( ack.diagn.reg > 15 )
             return 0; // Otherwise it cannot be converted to a single heximal digit.
        ts[1] = G_D2X(ack.diagn.reg);
        xx = ack.diagn.reg;
        si = &ts[2];
        vl = ack.diagn.val;
        cc = CHARS_OLD + CHARS_BUID_XXX_CRC; // Length of diagnostic packet.
    } else { // RREG, WREG
        ts[1] = '=';
#if   GASIFS_REG_SIZE == 1
        dc = ack.regRW.reg >> 4; // reg
        ts[2] = G_D2X(dc);
        xx = dc;
        dc = ack.regRW.reg & 0x0F; // reg
        ts[3] = G_D2X(dc);
        xx = xx ^ dc;
        si = &ts[4];
#elif GASIFS_REG_SIZE == 2
        of = (BYTE)(ack.regRW.reg >> 8);
        dc = of >> 4; // reg
        ts[2] = G_D2X(dc);
        xx = dc;
        dc = of & 0x0F; // reg
        ts[3] = G_D2X(dc);
        xx = xx ^ dc;
        of = (BYTE)(ack.regRW.reg);
        dc = of >> 4; // reg
        ts[4] = G_D2X(dc);
        xx = xx ^ dc;
        dc = of & 0x0F; // reg
        ts[5] = G_D2X(dc);
        xx = xx ^ dc;
        si = &ts[6];
#endif
        vl = ack.regRW.val;
        cc = CHARS_REG + CHARS_VAL + CHARS_BUID_XXX_CRC; // Length of regRW packet.
    }

    of  = (dt == D_HCMD ? CHARS_OLD : CHARS_VAL);
    si += (of - 1);
    ix  = (of);

    while (ix) { // Translate a payload.
        ix--;
        dc = (BYTE)(vl & 0x0F);
        *si-- = G_D2X(dc);
        xx ^= dc;
        vl >>= 4;
    }

    si += of;
    si += 1;
    *si = G_D2X(xx); // Checksum.

    ti = 0;
    state = S_WRITEACK;
    G_DATA_DIR(DIR_TX); // Switch bus transmitter now to allow some settle time.

    return cc; // Returns the number of bytes of the response (0 - error).
}

BOOL
GasifsNACK(void)
{
    if (state != S_CMDREADY) {
        return FALSE;
    }
    GasifsReset();
    return TRUE;
}

#if 0 /* Exclude from compilation */
CHAR
GasifsBUID(void)
{
    return (state == S_CMDREADY ? bid : (CHAR)('\0'));
}

BOOL
GasifsGetCommand(GASIFSRCV *rc)
{
    if (state != S_CMDREADY) {
        return FALSE;
    }

    switch (dt) {
        case D_HCMD:
            rc->dat.cmd = cmd;
            break;
        case D_RREG:
            rc->dat.regRW.reg = reg;
            rc->dat.regRW.val = 0;
            break;
        case D_WREG:
            rc->dat.regRW.reg = reg;
            rc->dat.regRW.val = val;
            break;
    }

    rc->act = dt;

    return  TRUE;
}
#endif
