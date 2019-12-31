#include "main.h"
#include "gpio.h"
#include "gasifs_vars.h"
#include "led.h"

extern void SystemInit(void);

#define UIF_flag 0x01
#define TIM4UIF (TIM4_SR & UIF_flag)
#define RESET_TIM4UIF() do { TIM4_SR &= ~UIF_flag; } while (0)

int main()
{
    SystemInit();

    LED_Init();
    LED_SetFallbackState(LED_SLOW_BLINK);
    LED_SetState(LED_SLOW_BLINK, LED_NO_TIMEOUT);

    GasifsInit();

    SET_GASIFSVAR_DIAG(REG_REP_PWM , 0);
    SET_GASIFSVAR_DIAG(REG_REP_RPM , 0);

    SET_GASIFSVAR_PARM(REG_PAR_BUID, 'N');
    SET_GASIFSVAR_PARM(REG_PAR_DFLT, 500); // 50.0%
    SET_GASIFSVAR_PARM(REG_PAR_FREQ, 150); // 15.0 kHz

    SetupPWM(GASIFSVAR_PARM(REG_PAR_FREQ) * 100U, GASIFSVAR_PARM(REG_PAR_DFLT)); // Startup defaults.

    for (;;) {
        if (TIM4UIF) {
            RESET_TIM4UIF();
            LED_Task();
        }

        if (GasifsIOTransportTask(0)) {
            if (GasifsProcess()) {
                REPORT_USART_COMM_OK();
                if (g_ParamsUpdateEvent) {
                    g_ParamsUpdateEvent = FALSE;
                    if (g_LastParamChanged == REG_PAR_FREQ) {
                        DWORD freq;
                        DWORD freqSet;
                        freq = GASIFSVAR_PARM(REG_PAR_FREQ) * 100UL;
                        freqSet = SetupPWM(freq, GASIFSVAR_DIAG(REG_REP_PWM));
                        SET_GASIFSVAR_PARM(REG_PAR_FREQ, freqSet);
                    }
                }
            }
        }
    }
}

BOOL
SetTask(WORD value)
{
    WORD pwm = SetDutyCycle(value);
    SET_GASIFSVAR_DIAG(REG_REP_PWM, pwm);
    return 1;
}

CHAR
GetDeviceBUID(void)
{
    return GASIFSVAR_PARM(REG_PAR_BUID);
}

void
RS485_ToggleTransmitter(BOOL enabled)
{
    GPIO_Write(p_RS485_DE, enabled ? PP_HIGH : PP_LOW);
}




#define TIM2_CR1_CEN BIT(0)

/**
 * Configures TIM2's overflow value to provide PWM at the specified frequency.
 * The set duty cycle is kept and re-calculated proportionally to match the new
 * counter range. Note: The timer is stopped and PWM duty cycle set to 0 while
 * being re-configured.
 *
 * @param freq
 *      Specifies PWM frequency. Value in kHz multiplied by 10 (for example,
 *      200 => 20 [kHz] * 10).
 * @param dutyCycle
 *      The value passed to SetDutyCycle() call.
 * @return
 */
DWORD
SetupPWM(DWORD freq, WORD dutyCycle)
{
    DWORD u32;
    BOOL  enabled;

    if (freq < MIN_PWM_FREQUENCY) {
        freq = MIN_PWM_FREQUENCY;
    } else if (freq > MAX_PWM_FREQUENCY) {
        freq = MAX_PWM_FREQUENCY;
    }

    u32 = F_TIM2 / freq;

    enabled = TIM2_CR1 & TIM2_CR1_CEN;
    TIM2_CR1 &= ~TIM2_CR1_CEN;

    TIM2_ARRH = HIBYTE(u32);
    TIM2_ARRL = LOBYTE(u32);
    TIM2_CNTRH = 0;
    TIM2_CNTRL = 0;

    SetTask(0);

    if (enabled) {
        TIM2_CR1 |=  TIM2_CR1_CEN;
    }

    SetTask(dutyCycle);

    return freq;
}

// ----------------------------------------------------------------------------

WORD
SetDutyCycle(WORD value) // Precision: 0.1%.
{
    WORD  maxValue;
    DWORD u32;
    if (value > 1000) { // 100.0%
        value = 1000;
    }
    maxValue = (WORD)(TIM2_ARRH << 8) | TIM2_ARRL;
    u32 = ((DWORD) (maxValue) * value) / 1000UL;
    TIM2_CCR3H = HIBYTE(u32);
    TIM2_CCR3L = LOBYTE(u32);
    return value;
}
