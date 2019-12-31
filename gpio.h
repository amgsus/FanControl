// STM8 GPIO Control
// File: gpio.h
// Created: 2019.03.30 15:45
// By: A.G.
// Updated: 2019.05.12 23:00

#ifndef RX100_EMU_GPIO_H
#define RX100_EMU_GPIO_H

#include "__TYPES.h"
#include "macros.h"

// ----------------------------------------------------------------------------

#define __I  volatile const /*!< defines 'read only' permissions     */
#define __O  volatile       /*!< defines 'write only' permissions    */
#define __IO volatile       /*!< defines 'read / write' permissions  */

struct
    GPIO
{
    __IO BYTE ODR;
    __IO BYTE IDR;
    __IO BYTE DDR;
    __IO BYTE CR1;
    __IO BYTE CR2;
};

typedef struct GPIO GPIO_TypeDef;

#define GPIOA_BaseAddress      0x5000
#define GPIOB_BaseAddress      0x5005
#define GPIOC_BaseAddress      0x500A
#define GPIOD_BaseAddress      0x500F
#define GPIOE_BaseAddress      0x5014
#define GPIOF_BaseAddress      0x5019

#define GPIOA ((struct GPIO *) GPIOA_BaseAddress)
#define GPIOB ((struct GPIO *) GPIOB_BaseAddress)
#define GPIOC ((struct GPIO *) GPIOC_BaseAddress)
#define GPIOD ((struct GPIO *) GPIOD_BaseAddress)
#define GPIOE ((struct GPIO *) GPIOE_BaseAddress)
#define GPIOF ((struct GPIO *) GPIOF_BaseAddress)

// ----------------------------------------------------------------------------

#define GPIO_PORT_A 1
#define GPIO_PORT_B 2
#define GPIO_PORT_C 3
#define GPIO_PORT_D 4
#define GPIO_PORT_E 5
#define GPIO_PORT_F 6

#define GPIO_PINENC(port, pino) ((WORD)((port) << 8) | (1 << (pino)))
#define PIN_NUMBER(pin) ((BYTE)((pin)))
#define PIN_PORT_NUMBER(pin) ((BYTE)((pin) >> 8))

enum
    GPIO_Pin
{
    PA0 = GPIO_PINENC(GPIO_PORT_A, 0),
    PA1 = GPIO_PINENC(GPIO_PORT_A, 1),
    PA2 = GPIO_PINENC(GPIO_PORT_A, 2),
    PA3 = GPIO_PINENC(GPIO_PORT_A, 3),
    PA4 = GPIO_PINENC(GPIO_PORT_A, 4),
    PA5 = GPIO_PINENC(GPIO_PORT_A, 5),
    PA6 = GPIO_PINENC(GPIO_PORT_A, 6),
    PA7 = GPIO_PINENC(GPIO_PORT_A, 7),

    PB0 = GPIO_PINENC(GPIO_PORT_B, 0),
    PB1 = GPIO_PINENC(GPIO_PORT_B, 1),
    PB2 = GPIO_PINENC(GPIO_PORT_B, 2),
    PB3 = GPIO_PINENC(GPIO_PORT_B, 3),
    PB4 = GPIO_PINENC(GPIO_PORT_B, 4),
    PB5 = GPIO_PINENC(GPIO_PORT_B, 5),
    PB6 = GPIO_PINENC(GPIO_PORT_B, 6),
    PB7 = GPIO_PINENC(GPIO_PORT_B, 7),

    PC0 = GPIO_PINENC(GPIO_PORT_C, 0),
    PC1 = GPIO_PINENC(GPIO_PORT_C, 1),
    PC2 = GPIO_PINENC(GPIO_PORT_C, 2),
    PC3 = GPIO_PINENC(GPIO_PORT_C, 3),
    PC4 = GPIO_PINENC(GPIO_PORT_C, 4),
    PC5 = GPIO_PINENC(GPIO_PORT_C, 5),
    PC6 = GPIO_PINENC(GPIO_PORT_C, 6),
    PC7 = GPIO_PINENC(GPIO_PORT_C, 7),

    PD0 = GPIO_PINENC(GPIO_PORT_D, 0),
    PD1 = GPIO_PINENC(GPIO_PORT_D, 1),
    PD2 = GPIO_PINENC(GPIO_PORT_D, 2),
    PD3 = GPIO_PINENC(GPIO_PORT_D, 3),
    PD4 = GPIO_PINENC(GPIO_PORT_D, 4),
    PD5 = GPIO_PINENC(GPIO_PORT_D, 5),
    PD6 = GPIO_PINENC(GPIO_PORT_D, 6),
    PD7 = GPIO_PINENC(GPIO_PORT_D, 7),

    PE0 = GPIO_PINENC(GPIO_PORT_E, 0),
    PE1 = GPIO_PINENC(GPIO_PORT_E, 1),
    PE2 = GPIO_PINENC(GPIO_PORT_E, 2),
    PE3 = GPIO_PINENC(GPIO_PORT_E, 3),
    PE4 = GPIO_PINENC(GPIO_PORT_E, 4),
    PE5 = GPIO_PINENC(GPIO_PORT_E, 5),
    PE6 = GPIO_PINENC(GPIO_PORT_E, 6),
    PE7 = GPIO_PINENC(GPIO_PORT_E, 7),

    PF0 = GPIO_PINENC(GPIO_PORT_F, 0),
    PF1 = GPIO_PINENC(GPIO_PORT_F, 1),
    PF2 = GPIO_PINENC(GPIO_PORT_F, 2),
    PF3 = GPIO_PINENC(GPIO_PORT_F, 3),
    PF4 = GPIO_PINENC(GPIO_PORT_F, 4),
    PF5 = GPIO_PINENC(GPIO_PORT_F, 5),
    PF6 = GPIO_PINENC(GPIO_PORT_F, 6),
    PF7 = GPIO_PINENC(GPIO_PORT_F, 7)
};

enum
    GPIO_Mode // Bit 7 - DDR, Bit 6 - CR1, Bit 5 - CR2, Bit 4 - ODR.
{
    GPIO_IN_FL_NO_IT      = (BYTE) 0x00,
    GPIO_IN_PU_NO_IT      = (BYTE) 0x40,
    GPIO_IN_FL_IT         = (BYTE) 0x20,
    GPIO_IN_PU_IT         = (BYTE) 0x60,
    GPIO_OUT_OD_LOW_FAST  = (BYTE) 0xA0,
    GPIO_OUT_PP_LOW_FAST  = (BYTE) 0xE0,
    GPIO_OUT_OD_LOW_SLOW  = (BYTE) 0x80,
    GPIO_OUT_PP_LOW_SLOW  = (BYTE) 0xC0,
    GPIO_OUT_OD_HIZ_FAST  = (BYTE) 0xB0,
    GPIO_OUT_PP_HIGH_FAST = (BYTE) 0xF0,
    GPIO_OUT_OD_HIZ_SLOW  = (BYTE) 0x90,
    GPIO_OUT_PP_HIGH_SLOW = (BYTE) 0xD0
};

enum
    GPIO_Value
{
    PP_LOW = 0,
    PP_HIGH = 1,
    OD_LOW = PP_LOW,
    OD_HIZ = PP_HIGH
};

// ----------------------------------------------------------------------------

void GPIO_Setup(enum GPIO_Pin pin, enum GPIO_Mode config);
enum GPIO_Value GPIO_Read(enum GPIO_Pin pin);
void GPIO_Write(enum GPIO_Pin pin, enum GPIO_Value value);
enum GPIO_Value GPIO_Toggle(enum GPIO_Pin pin);
void GPIO_SetAllHighZ(BYTE portNumber);

# endif // ------------------------------------------- End of Include Guard ---
