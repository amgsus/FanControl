/*
 * STM8S103F3 Single Fan Controller with Gasifs interface.
 *
 * By:              A.G.
 * Created:         2019.12.31
 * Last modified:   2020.01.01
 */

#include "main.h"
#include "gpio.h"
#include "gasifs_vars.h"
#include "led.h"
#include "asm.h"
#include "params.h"
#include "watchdog.h"

extern void SystemInit(void);

#define UIF_flag 0x01
#define TIM4UIF (TIM4_SR & UIF_flag)
#define RESET_TIM4UIF() do { TIM4_SR &= ~UIF_flag; } while (0)

PRIVATE volatile WORD g_LastTachoCapturedValue = 0; // RPM
PRIVATE volatile WORD g_DCINRawValue = 0; // ADC value

PRIVATE
WORD
ReadTachoCaptureSafe(void);

PRIVATE
void
CalculateRPM(void);

PRIVATE
void
CheckGasifsParametersUpdateEvent(void);

PRIVATE
void
CheckGasifsKey(void);

PRIVATE
void
CheckGasifsResetRequest(void);

PRIVATE void
IncrementGasifsCounter(void);

PRIVATE
void
ADCTask(void);

PRIVATE void
CalculateDCINVoltage(void);

// ----------------------------------------------------------------------------

// TODO: Implement working mode (REG_PAR_MODE).

int main()
{
    WORD cnt = 0;

    SystemInit();

    LED_Init();
    LED_SetFallbackState(LED_SLOW_BLINK);
    LED_SetState(LED_SLOW_BLINK, LED_NO_TIMEOUT);

    GasifsInit();
    SetInfoRegisters(RST_SR);
    LoadParameters();

    SET_GASIFSVAR_DIAG(REG_REP_PWM , 0);
    SET_GASIFSVAR_DIAG(REG_REP_RPM , 0);
    SET_GASIFSVAR_DIAG(REG_REP_PER , 0);
    SET_GASIFSVAR_DIAG(REG_REP_DCIN, 0);

    SetupPWM(GASIFSVAR_PARM(REG_PAR_FREQ) * 100U, GASIFSVAR_PARM(REG_PAR_DUTY)); // Startup defaults.

    UnmaskInterrupts();

    for (;;)
    {
        if (GasifsIOTransportTask(0)) {
            if (GasifsProcess()) {
                CheckGasifsResetRequest();
                CheckGasifsParametersUpdateEvent();
                CheckGasifsKey();
                REPORT_USART_COMM_OK();
            }
        }

        if (TIM4UIF) {
            RESET_TIM4UIF();
            LED_Task();
            cnt++;
            if (cnt % 50 == 0) { // 50 ms.
                ADCTask();
            }
            if (cnt == 250) {
                CalculateRPM();
            } else if (cnt == 500) {
                cnt = 0;
                CalculateDCINVoltage();
            }
            IncrementGasifsCounter();
        }

        GPIO_Toggle(p_DEBUG);
    }
}

// ----------------------------------------------------------------------------



PRIVATE void
IncrementGasifsCounter(void)
{
    SET_GASIFSVAR_INFO(REG_NFO_TICK, GASIFSVAR_INFO(REG_NFO_TICK) + 1);
    if (GASIFSVAR_INFO(REG_NFO_TICK) >= 60000) { // Overflows in 1 minute.
        SET_GASIFSVAR_INFO(REG_NFO_TICK, 0);
    }
}

BOOL
SetTask(WORD value)
{
    WORD pwm = SetDutyCycle(value);
    SET_GASIFSVAR_DIAG(REG_REP_PWM, pwm);
    return 1;
}

PRIVATE
void
CheckGasifsParametersUpdateEvent(void)
{
    if (g_ParamsUpdateEvent) {
        g_ParamsUpdateEvent = FALSE;
        if (g_LastParamChanged == REG_PAR_FREQ) {
            DWORD freq;
            DWORD freqSet;
            freq = GASIFSVAR_PARM(REG_PAR_FREQ) * 100U; // SetupPWM() expects f in Hz (here 0.1 kHz).
            freqSet = SetupPWM(freq, GASIFSVAR_DIAG(REG_REP_PWM));
            SET_GASIFSVAR_PARM(REG_PAR_FREQ, freqSet);
        }
    }
}

PRIVATE
void
CheckGasifsKey(void)
{
    switch (g_ParamKey) {
        case KEY_RESTORE_DEFAULT_PARAMETERS:
            LoadDefaultParameters();
            break;
        case KEY_LOAD_STORED_PARAMETERS:
            LoadParameters();
            break;
        case KEY_STORE_PARAMETERS:
            SaveParameters();
            break;
    }
    g_ParamKey = 0;
}

PRIVATE
void
CheckGasifsResetRequest(void)
{
    if (g_ResetRequested) {
        while (1) {
            WWDG_CR &= ~BIT(6); // Clear WWDG's T6 bit to generate software reset.
        }
    }
}

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

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

// -------------------------------------------------------------- Tachometer ---

#define TACHO_AVG_COUNT 8 /* 2^x */

ISR(TIM1_CAPCOM_IRQHandler) {

    WORD deltaTicks;
    WORD tickNow;

    static WORD   accum[TACHO_AVG_COUNT];
    static UINT32 sum    = 0;
    static BYTE   tail   = 0;
    static BYTE   head   = 0;
    static BYTE   count  = 0;
    static WORD   last   = 0;

    TIM1_SR2 &= ~BIT(3); // Clear CC3OF (overcapture).
    tickNow = (WORD)(TIM1_CCR3H << 8) | TIM1_CCR3L; // Clears CC3IE by reading CCR3L.

    deltaTicks = tickNow - last;
    last = tickNow;

    sum += deltaTicks; // Accum avg.

    accum[head] = deltaTicks;
    head = (head + 1) & (TACHO_AVG_COUNT - 1);
    count++;

    if (count >= TACHO_AVG_COUNT) {
        sum -= accum[tail];
        tail = (tail + 1) & (TACHO_AVG_COUNT - 1);
        count--;
    }

    g_LastTachoCapturedValue = sum / count;
}

PRIVATE
WORD
ReadTachoCaptureSafe(void)
{
    volatile WORD n;
    do {
        n = g_LastTachoCapturedValue;
    } while (n != g_LastTachoCapturedValue);
    return n;
}

PRIVATE
void
CalculateRPM(void)
{
    UINT32 n;
    UINT32 accum;
    n = ReadTachoCaptureSafe();
    accum = n * 50UL; // 50 us resolution.
    accum /= 100; // .1 ms
    SET_GASIFSVAR_DIAG(REG_REP_PER, (GASIFSOLD) accum);
    n = 10000U / (WORD) accum;
    n *= 60; // Pulses scaled up to 1 minute.
    if (GASIFSVAR_PARM(REG_PAR_TDIV) > 1) {
        n /= GASIFSVAR_PARM(REG_PAR_TDIV);
    }
    SET_GASIFSVAR_DIAG(REG_REP_RPM, (GASIFSOLD) n);
}

// ----------------------------------------------- DC IN Voltage Measurement ---

#define ADC_FILTER_LENGTH 8 /* 2^x */

PRIVATE
void
ADCTask(void)
{
    static WORD   accum[ADC_FILTER_LENGTH];
    static UINT32 sum    = 0;
    static BYTE   tail   = 0;
    static BYTE   head   = 0;
    static BYTE   count  = 0;

    WORD adcValue;

    if (ADC_CSR & BIT(7)) { // EOC.
        adcValue  = ADC_DRL;
        adcValue |= ADC_DRH << 8;

        sum += adcValue; // Averaging.

        accum[head] = adcValue;
        head = (head + 1) & (ADC_FILTER_LENGTH - 1);
        count++;

        if (count >= ADC_FILTER_LENGTH) {
            sum -= accum[tail];
            tail = (tail + 1) & (ADC_FILTER_LENGTH - 1);
            count--;
        }

        g_DCINRawValue = sum / count;

        ADC_CSR &= ~BIT(7);
        ADC_CR1 |=  BIT(0); // Start next conversion.
    }
}

PRIVATE void
CalculateDCINVoltage(void)
{
    UINT32 n = (UINT32) g_DCINRawValue * GASIFSVAR_PARM(REG_PAR_VSCL);
    n /= 1000UL;
    n += (SINT16) GASIFSVAR_PARM(REG_PAR_VOFS); // Cast to signed.
    SET_GASIFSVAR_DIAG(REG_REP_DCIN, (UINT16) n); // Potential data lost.
}
