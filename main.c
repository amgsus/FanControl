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

    SET_GASIFSVAR_DIAG(REG_REP_PWM, 0);

    for (;;) {
        if (TIM4UIF) {
            RESET_TIM4UIF();
            LED_Task();
        }

        if (GasifsIOTransportTask(0)) {
            if (GasifsProcess()) {
                REPORT_USART_COMM_OK();
            }
        }
    }
}

BOOL SetTask(WORD value) { return 1; }

CHAR
GetDeviceBUID(void)
{
    return 'N';
}

void
RS485_ToggleTransmitter(BOOL enabled)
{
    GPIO_Write(p_RS485_DE, enabled ? PP_HIGH : PP_LOW);
}
