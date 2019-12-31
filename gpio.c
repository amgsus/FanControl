// RX100_EMU
// File: gpio.c
// Created: 2019.03.30 15:45
// Author: A.G.

#include "gpio.h"
#include "defines.h"

// -----------------------------------------------------------------------------

PRIVATE struct GPIO * __GetPort(BYTE portNumber);
PRIVATE struct GPIO * __GetPortFromPin(enum GPIO_Pin pin);

// -----------------------------------------------------------------------------

void
    GPIO_Setup(enum GPIO_Pin pin, enum GPIO_Mode mode)
{
    GPIO_TypeDef *port;
    BYTE mask;

    port = __GetPortFromPin(pin);
    mask = PIN_NUMBER(pin);


    /* Reset corresponding bit to pinMask in CR2 register */
    port->CR2 &= (BYTE)(~(mask));

    /*-----------------------------*/
    /* Input/Output mode selection */
    /*-----------------------------*/

    if ((((BYTE)(mode)) & (BYTE)0x80) != (BYTE)0x00) /* Output mode */
    {
        if ((((BYTE)(mode)) & (BYTE)0x10) != (BYTE)0x00) /* High level */
        {
            port->ODR |= (BYTE)mask;
        }
        else /* Low level */
        {
            port->ODR &= (BYTE)(~(mask));
        }
        /* Set Output mode */
        port->DDR |= (BYTE)mask;
    }
    else /* Input mode */
    {
        /* Set Input mode */
        port->DDR &= (BYTE)(~(mask));
    }

    /*------------------------------------------------------------------------*/
    /* Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection */
    /*------------------------------------------------------------------------*/

    if ((((BYTE)(mode)) & (BYTE)0x40) != (BYTE)0x00) /* Pull-Up or Push-Pull */
    {
        port->CR1 |= (BYTE)mask;
    }
    else /* Float or Open-Drain */
    {
        port->CR1 &= (BYTE)(~(mask));
    }

    /*-----------------------------------------------------*/
    /* Interrupt (Input) or Slope (Output) modes selection */
    /*-----------------------------------------------------*/

    if ((((BYTE)(mode)) & (BYTE)0x20) != (BYTE)0x00) /* Interrupt or Slow slope */
    {
        port->CR2 |= (BYTE)mask;
    }
    else /* No external interrupt or No slope control */
    {
        port->CR2 &= (BYTE)(~(mask));
    }
}

// -----------------------------------------------------------------------------

enum GPIO_Value
    GPIO_Read(enum GPIO_Pin pin)
{
    GPIO_TypeDef *gpio = __GetPortFromPin(pin);
    return (gpio->IDR & PIN_NUMBER(pin)) ? PP_HIGH : PP_LOW;
}

void
GPIO_Write(enum GPIO_Pin pin, enum GPIO_Value value)
{
    GPIO_TypeDef *gpio = __GetPortFromPin(pin);
    if (value == PP_HIGH) {
        gpio->ODR |= PIN_NUMBER(pin);
    } else {
        gpio->ODR &= ~PIN_NUMBER(pin);
    }
}

enum GPIO_Value
    GPIO_Toggle(enum GPIO_Pin pin)
{
    enum GPIO_Value  valueNow;
    valueNow = GPIO_Read(pin);
    GPIO_Write(pin, (valueNow == PP_HIGH ? PP_LOW : PP_HIGH));
    return valueNow;
}

void
    GPIO_SetAllHighZ(BYTE portNumber)
{
    struct GPIO* port = __GetPort(portNumber);
    port->CR2 = 0x00; // Disable pull-up.
    port->DDR = 0x00; // Input.
    port->CR1 = 0x00; // Floating.
}

// -----------------------------------------------------------------------------

PRIVATE struct GPIO *
    __GetPortFromPin(enum GPIO_Pin pin)
{
    return __GetPort(PIN_PORT_NUMBER(pin));
}

PRIVATE struct GPIO *
    __GetPort(BYTE portNumber)
{
    switch   (portNumber) {
        case GPIO_PORT_A:
            return GPIOA;
        case GPIO_PORT_B:
            return GPIOB;
        case GPIO_PORT_C:
            return GPIOC;
        case GPIO_PORT_D:
            return GPIOD;
        case GPIO_PORT_E:
            return GPIOE;
        case GPIO_PORT_F:
            return GPIOF;
    }

    while (1); // PANIC(); // Stuck here, 'cause we cannot return NULL.
}
