#ifndef FANCONTROL_MAIN_H
#define FANCONTROL_MAIN_H

// ----------------------------------------------------------------------------

#include "__TYPES.h"
#include "cosmic.h"
#include "iostm8s103.h"
#include "gasifs.h"
#include "gpio.h"

// ----------------------------------------------------------------- Pinout ---

#define p_PWM       PA3
#define p_LED       PB5
#define p_TACHO     PC3
#define p_RS485_DE  PD4
#define p_TX        PD5
#define p_RX        PD6

// ----------------------------------------------------------------------------

BOOL SetTask(WORD value);

BOOLEAN
ValidateParameterValue(GASIFSREG reg, GASIFSVAL val);

#define GPIO_LED_OFF 1
#define GPIO_LED_ON  0

#define GPIO_SetLED(state) \
    do { \
        if ((state) == GPIO_LED_OFF) { \
            GPIO_Write(p_LED, OD_HIZ); \
        } else if ((state) == GPIO_LED_ON) { \
            GPIO_Write(p_LED, OD_LOW); \
        } \
    } while (0)

// ----------------------------------------------------------------- Gasifs ---

/* [D]p[M]b = 0x4450 */

#define DEFAULT_BUID        'M'
#define HARDWARE_TAG        ( ('D' << 8) + 'P' )

/**
 * When this value is written to REG_NFO_HWID (0xF0) register the device will
 * be reset immediately.
 */
#define RESET_MAGIC_NUMBER  0xDEAD

// ----------------------------------------------------------------------------
# endif // ------------------------------------------- End of Include Guard ---
