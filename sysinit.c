#include "main.h"
#include "gpio.h"

#define SERIAL_BAUDRATE     ( 9600 )        /* bauds */

#define _UART_DIV     \
    ((WORD) ((float)(F_MASTER) / (float)(SERIAL_BAUDRATE) + 0.5))
#define _PRECALC_BRR2 \
    ((BYTE) ((((_UART_DIV) & 0xF000) >> 8) | ((_UART_DIV) & 0x000F)))
#define _PRECALC_BRR1 \
    ((BYTE) ((((_UART_DIV) & 0x0FF0) >> 4)))

void
SystemInit(void)
{
    WORD  u16;

    // System clock.

    CLK_CCOR    = 0x00; // Clock output disabled.

    CLK_ICKCR   = 0x01; // Enable on-chip HSI.
    CLK_SWR     = 0xE1; // Select HSI as master clock source.
    CLK_CKDIVR  = 0x00; // F_MASTER = 16 MHz. CPU runs at max frequency.
    CLK_SWCR    = 0x02; // Set SWEN bit (execute clock switching). Keep switch interrupt disabled.

    // Peripheral clocking.

    CLK_PCKENR1 = 0xFF;
    CLK_PCKENR2 = 0xFF;

    // GPIO.

    GPIO_Setup(p_PWM,      GPIO_OUT_PP_LOW_FAST);
    GPIO_Setup(p_TACHO,    GPIO_IN_PU_NO_IT);
    GPIO_Setup(p_RS485_DE, GPIO_OUT_PP_LOW_FAST);
    GPIO_Setup(p_TX,       GPIO_OUT_PP_HIGH_FAST);
    GPIO_Setup(p_RX,       GPIO_IN_PU_NO_IT);
    GPIO_Setup(p_LED,      GPIO_OUT_OD_HIZ_SLOW);

    // Timebase.

    TIM4_PSCR   = 0x07; // Divide by 128 (i.e. 8 us clock).
    TIM4_ARR    = 255-125; // Period = 1 ms.
    TIM4_CNTR   = TIM4_ARR;
    TIM4_IER    = 0x00; // Interrupts disabled.
    TIM4_EGR    = 0x00; // No action on UEV.
    TIM4_CR1    = 0x01; // Start. UEV enabled.

    // UART.

    UART1_CR2   = 0x00; // Reset. UART is disabled.
    UART1_BRR1  = _PRECALC_BRR1;
    UART1_BRR2  = _PRECALC_BRR2;
    UART1_CR1   = 0x00; // 1 start bit, 8 data bits.
    UART1_CR3   = 0x00; // LIN mode disable. 1 stop bit. SCLK off.
    UART1_CR4   = 0x00; // LIN mode not used.
    UART1_CR5   = 0x00; // Smartcard and IrDA modes not used.
    UART1_GTR   = 0x00; // Smartcard mode not used.
    UART1_PSCR  = 0x00; // Smartcard and IrDA modes not used.
    UART1_CR2   = 0x0C; // Enable TX/RX. All UART interupts disabled.

    // PWM.

    u16 = F_MASTER / F_PWM_DEFAULT;

    TIM2_CR1    = 0x00;             // Stop.
    TIM2_PSCR   = 0;                // No prescaler.
    TIM2_IER    = 0;                // Disable interrupts.
    TIM2_EGR    = 0x00;             // Disable events.
    TIM2_ARRH   = HIBYTE(u16);      // Set auto-reload value (PWM frequency).
    TIM2_ARRL   = LOBYTE(u16);
    TIM2_CCR1H  = 0;                // Reset OCC1 counter.
    TIM2_CCR1L  = 0;
    TIM2_CCMR1  = 0;                // No output.
    TIM2_CCR2H  = 0;                // Reset OCC2 counter.
    TIM2_CCR2L  = 0;
    TIM2_CCMR2  = 0;                // No output.
    TIM2_CCMR3  = BIN(110) << 4U;   // CC3 is OUTPUT. Edge-aligned PWM (mode 1). CCR1x preload.
    TIM2_CCR3H  = 0;                // Reset OCC3 counter.
    TIM2_CCR3L  = 0;
    TIM2_CCER1  = 0;                // Disable both CC1 & CC2.
    TIM2_CCER2  = 0x01;             // Enable OC3. Set OC3 active high.
    TIM2_CNTRH  = 0;                // Reset counter.
    TIM2_CNTRL  = 0;
    TIM2_CR1   |= 0x01;             // Enable.

    // Input Capture Timer.
}


