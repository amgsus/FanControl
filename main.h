/*
 * By:              A.G.
 * Created:         2019.12.31
 * Last modified:   2020.01.04
 */

#ifndef MAIN_H
#define MAIN_H

// ----------------------------------------------------------------------------

#include "__TYPES.h"
#include "mcu.h"
#include "gasifs.h"
#include "gpio.h"

// ----------------------------------------------------------------- Pinout ---

#define p_DEBUG     PA2
#define p_PWM       PA3
#define p_LED       PB5
#define p_TACHO     PC3
#define p_VIN       PD2
#define p_RS485_DE  PD4
#define p_TX        PD5
#define p_RX        PD6

// ----------------------------------------------------------------------------

#define F_MASTER        16000000UL  /* Hz */
#define F_PWM_DEFAULT   15000U      /* Hz */
#define F_TIM2          F_MASTER

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

WORD
SetDutyCycle(WORD value);

#define MIN_PWM_FREQUENCY 1000U     /*   1 kHz */
#define MAX_PWM_FREQUENCY 100000U   /* 100 kHz */
#define MIN_DUTY_CYCLE    0U        /*     0 % */
#define MAX_DUTY_CYCLE    1000U     /* 100.0 % */

DWORD
SetupPWM(DWORD freq, WORD dutyCycle);

// ----------------------------------------------------------------- Gasifs ---

/**
 * When this value is written to REG_NFO_HWID (0xF0) register the device will
 * be reset immediately.
 */
#define RESET_MAGIC_NUMBER  0xDEAD

// ----------------------------------------------------------------------------
# endif // ------------------------------------------- End of Include Guard ---
