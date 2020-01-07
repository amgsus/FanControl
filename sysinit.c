#include "main.h"
#include "gpio.h"

#define  BAUDRATE 38400

#define _UART_DIV     \
    ((WORD) ((float)(F_MASTER) / (float)(BAUDRATE) + 0.5))
#define _PRECALC_BRR2 \
    ((BYTE) ((((_UART_DIV) & 0xF000) >> 8) | ((_UART_DIV) & 0x000F)))
#define _PRECALC_BRR1 \
    ((BYTE) ((((_UART_DIV) & 0x0FF0) >> 4)))

#define TIM1_PRESCALER ((WORD)(801)) /* 16MHz / 20000 = 800 (50 us) */
#define TIM2_ARR_VALUE ((WORD)(F_MASTER / F_PWM_DEFAULT))

void
SystemInit(void)
{
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
    GPIO_Setup(p_DEBUG,    GPIO_OUT_PP_LOW_FAST);
    GPIO_Setup(p_VIN,      GPIO_IN_FL_NO_IT);

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

    TIM2_CR1    = 0;                // Stop.
    TIM2_SR1    = 0;                // Clear all flags.
    TIM1_SR2    = 0;
    TIM2_PSCR   = 0;                // No prescaler.
    TIM2_CNTRH  = 0;                // Reset counter.
    TIM2_CNTRL  = 0;
    TIM2_IER    = 0;                // Disable interrupts.
    TIM2_EGR    = 0x00;             // Disable events.
    TIM2_ARRH   = HIBYTE(TIM2_ARR_VALUE);      // Set auto-reload value (PWM frequency).
    TIM2_ARRL   = LOBYTE(TIM2_ARR_VALUE);
    TIM2_CCR1H  = 0;                // Reset CC1 counter.
    TIM2_CCR1L  = 0;
    TIM2_CCMR1  = 0;                // Not using CC1.
    TIM2_CCR2H  = 0;                // Reset CC2 counter.
    TIM2_CCR2L  = 0;
    TIM2_CCMR2  = 0;                // Not using CC2.
    TIM2_CCMR3  = BIN(110) << 4U;   // CC3 is OUTPUT. Edge-aligned PWM (mode 1). CCR1x preload.
    TIM2_CCR3H  = 0;                // Reset CC3 counter.
    TIM2_CCR3L  = 0;
    TIM2_CCER1  = 0;                // Disable CC1 & CC2.
    TIM2_CCER2  = 0x01;             // Enable CC3. Set CC3 output active high.
    TIM2_CR1   |= 0x01;             // Enable.

    // Input Capture Timer.

    TIM1_CR1    = 0;                // Stop.
    TIM1_SR1    = 0;                // Clear all flags.
    TIM1_SR2    = 0;
    TIM1_PSCRH  = HIBYTE(TIM1_PRESCALER);
    TIM1_PSCRL  = LOBYTE(TIM1_PRESCALER);
    TIM1_CNTRH  = 0;                // Reset counter.
    TIM1_CNTRL  = 0;
    TIM1_EGR    = BIT(3);           // Enable CC3 event generation.
    TIM1_IER    = BIT(3);           // Unmask CC3 interrupt.
    TIM1_ARRH   = 0xFF;             // Set auto-reload value to MAX.
    TIM1_ARRL   = 0xFF;
    TIM1_ETR    = 0;                // Not using external trigger.
    TIM1_SMCR   = 0;                // Not using slave mode control.
    TIM1_CCR1H  = 0;                // Reset CC1 counter.
    TIM1_CCR1L  = 0;
    TIM1_CCMR1  = 0;                // Not using CC1.
    TIM1_CCR2H  = 0;                // Reset CC2 counter.
    TIM1_CCR2L  = 0;
    TIM1_CCMR2  = 0;                // Not using CC2.
    TIM1_CCR3H  = 0;                // Reset CC3 counter.
    TIM1_CCR3L  = 0;
    TIM1_CCMR3  = (BIN(1111) << 4) | BIN(01); // CC3 is INPUT (IC3->TI3FP). No prescaler. Filter: F_SAMPLING=F_MASTER/32, N=8 (62500/2 kHz).
    TIM1_CCR4H  = 0;                // Reset CC4 counter.
    TIM1_CCR4L  = 0;
    TIM1_CCMR4  = 0;                // Not using CC4.
    TIM1_CCER1  = 0;                // Disable CC1 & CC2.
    TIM1_CCER2  = BIT(0);           // Enable CC3. Capture on rising edge. // TODO: Which of edges is better to capture?
    TIM1_CR1   |= 0x01;             // Enable.

    // TODO: Configure ADC.
}
