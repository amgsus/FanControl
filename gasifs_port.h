/*
 * *** Gasifs Port Header (STM8/C8051) ***
 *
 * Version: 1.1
 * By: A.G.
 * Created: 2018/11/19
 * Last modified: 2019/12/31 11:13
 */

#ifndef GASIFS_PORT_H
#define GASIFS_PORT_H

// HAL Macros //

#include "__TYPES.h"
#include "mcu.h"
#include "hex.h"

// ----------------------------------------------------------------------------

// Choose MCU series here:

#define GASIFS_STM8
// #define GASIFS_C8051

// ------------------------------------------------------------- Data types ---

typedef WORD  GASIFSCMD;
#define GASIFS_CMD_SIZE  2  /* 16-bit */

typedef WORD  GASIFSOLD;
#define GASIFS_OLD_SIZE  2  /* 16-bit */

typedef BYTE  GASIFSREG;
#define GASIFS_REG_SIZE  1  /*  8-bit */

typedef WORD  GASIFSVAL;
#define GASIFS_VAL_SIZE  2  /* 16-bit */

// --------------------------------------------------- SERIAL COMMUNICATION ---

// GetDeviceBUID(): Defined in 'main.c'. For resolving module intersection,
// since we cannot include 'gasifs_vars.h' here.
CHAR
GetDeviceBUID(void);

#define DEVICE_BUID ( GetDeviceBUID() )
#define G_CHECK_DEVICE_BUID(ch) ((ch) == DEVICE_BUID || (ch) == '@')

#ifdef GASIFS_C8051
// Definitions for Silicon Labs MCUs.
  #define G_PRE() \
      BYTE oldSFRs = SFRPAGE; \
      SFRPAGE = 0x00; \
      if (SCON0 & (BIT(7) | BIT(6))) { \
          SCON0 &= ~ (BIT(7) | BIT(6)); \
          GasifsReset(); \
          return FALSE; \
      }

  #define G_POST() \
      do { SFRPAGE = oldSFRs; } while (0)

  #define G_GET()     SBUF0
  #define G_PUT(c)    do { SBUF0 = (c); } while (0)
  #define G_RI()      RI0
  #define G_RI_RST()  do { RI0 = 0; } while (0)
  #define G_RXNE()    0
  #define G_TI()      TI0
  #define G_TI_RST()  do { TI0 = 0; } while (0)
  #define G_TXE()
  #define G_TC()
#elif defined(GASIFS_STM8)
// Definitions for ST Microelectronics MCUs (8-bit).
  #define G_PRE()
  #define G_POST()
  #define G_GET()     (UART1_DR)
  #define G_PUT(c)    do { UART1_DR = (c); } while (0)
  #define G_RI()      0
  #define G_RI_RST()
  #define G_RXNE()    (UART1_SR & BIT(5))
  #define G_TI()
  #define G_TI_RST()
  #define G_TXE()     (UART1_SR & BIT(7))
  #define G_TC()      (UART1_SR & BIT(6))
#endif

// ----------------------------------------------------------------- RS-485 ---

extern void RS485_ToggleTransmitter(BOOL enabled);

#define DIR_RX 0
#define DIR_TX 1

#define G_DATA_DIR(dir) \
    do { \
        if ((dir) == DIR_RX) { \
            RS485_ToggleTransmitter(0); \
        } else if ((dir) == DIR_TX) { \
            RS485_ToggleTransmitter(1); \
        } \
    } while (0)

// -------------------------------------------------------- DATA CONVERSION ---

#define G_D2X(n) d2x(n)
#define G_X2D(n) x2d(n)

// ------------------------------------------------------------- IZVRAT >:] ---

#ifndef PRIVATE
  #define PRIVATE static
#endif

#ifndef BIT
  #define BIT(n) (1U << ((unsigned) n))
#endif

# endif // ------------------------------------------- End of Include Guard ---
