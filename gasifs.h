// ****************************************************************************
//     Gasifs Stream Parser & State Machine w/ Register Map
// ****************************************************************************
// *** Author:   A.G.
// *** Version:  2.1.0
// *** Modified: 2018/11/19 01:41
// *** License:  MIT
// ****************************************************************************
//     (c) Sensotronica Ltd. 2018
// ****************************************************************************

#ifndef  GASIFS_H
#define  GASIFS_H  0x210

#include "gasifs_port.h"

// -------------------------------------------------------- DATA STRUCTURES ---

/**
 * This data structure contains information about a received command from a
 * Gasifs master device.
 *
 * See a function GasifsGetCommand().
 *
 * A field 'act' may take one of these values: GASIFS_CMND or GASIFS_RREG or
 * GASIFS_WREG. According to this value, a developer need to determine what
 * fields of an union "dat" should be read then.
 */
typedef struct GASIFSRCV {
    BYTE  act;
    union {
        // V1
        GASIFSCMD cmd;
        // V2
        struct {
            GASIFSREG reg;
            GASIFSVAL val;
        } regRW;
    } dat;
} GASIFSRCV;

/**
 * This data structure contains a response that is being sent to a master device.
 * See a function GasifsAcknoledge().
 *
 * A developer should fill fields that matches the type of packet that is
 * received from a master device. For GASIFS_CMND valid field is "diagn" (diagnostic info), for GASIFS_RREG or GASIFS_WREG valid field is "regRW" (register read/write).
 */
typedef union  GASIFSACK {
    // V1
    struct {
        BYTE reg;
        GASIFSOLD val;
    } diagn;
    // V2
    struct {
        GASIFSREG reg;
        GASIFSVAL val;
    } regRW;
} GASIFSACK;

// ---------------------------------------------------------------- GENERAL ---

void GasifsInit(void);

BOOL GasifsIOTransportTask(BYTE reserved); // TRUE - valid packet has been received.

BOOL GasifsProcess(void); // TRUE - data has been processed and ACK.

void GasifsReset(void);

// ----------------------------------------------------------- FLOW CONTROL ---

#define GASIFS_HCMD 0
#define GASIFS_RREG 1
#define GASIFS_WREG 2

/**
 *
 * @param rc
 * @return
 */
BOOL GasifsGetCommand(GASIFSRCV *rc);

/**
 *
 * @return
 */
CHAR GasifsBUID(void);

/**
 *
 * @param ack
 * @return
 */
BYTE GasifsAcknoledge(GASIFSACK ack);

/**
 *
 * @return
 */
BOOL GasifsNACK(void);

// ------------------------------------------------- USER-DEFINED CALLBACKS ---

/**
 *
 * @param cmd
 */
extern
void GasifsHCMD(GASIFSCMD cmd);

/**
 *
 * @param reg
 */
extern
void GasifsRREG(GASIFSREG reg);

/**
 *
 * @param reg
 * @param val
 */
extern
void GasifsWREG(GASIFSREG reg, GASIFSVAL val);

// ----------------------------------------------------------------------------
# endif // ------------------------------------------- End of Include Guard ---
