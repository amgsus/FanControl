#include "led.h"
#include "main.h" // LED pin definition.
#include "macros.h"

typedef enum LED_STATE State;

PRIVATE State   __state;
PRIVATE State   __fallbackState;
PRIVATE BOOLEAN __stateChanged;
PRIVATE BYTE    __times;
PRIVATE WORD    __counter;

void
LED_Init(void)
{
    __state           = LED_OFF;
    __fallbackState   = LED_OFF;
    __stateChanged    = TRUE;
    __times           = LED_NO_TIMEOUT;
    __counter         = 0;
}

void
LED_SetFallbackState(enum LED_STATE state)
{
    __fallbackState = state;
}

void
LED_SetState(enum LED_STATE state, BYTE times)
{
    __stateChanged = (state != __state);
    __state = state;
    __times = times; // FIXME: Zero is not allowed. Reserved for internal use.
}

void
LED_Task(void)
{
    BOOL cycleFlag = FALSE;

    if (__stateChanged) {
        __counter = 0;
    } else {
        __counter += LED_TASK_INTERVAL;
    }

    switch (__state) {
        case LED_OFF:
            if (__stateChanged) {
                GPIO_SetLED(GPIO_LED_OFF);
            }
            break;

        case LED_ON:
            if (__stateChanged) {
                GPIO_SetLED(GPIO_LED_ON);
            }
            break;

        case LED_BLINK:
            if (__counter == 0) {
                GPIO_SetLED(GPIO_LED_ON);
            } else if (__counter == 100) {
                GPIO_SetLED(GPIO_LED_OFF);
            } else if (__counter == 200) {
                __counter = ~0;
                cycleFlag = TRUE;
            }
            break;

        case LED_SLOW_BLINK:
            if (__counter == 0) {
                GPIO_SetLED(GPIO_LED_ON);
            } else if (__counter == 2000) {
                GPIO_SetLED(GPIO_LED_OFF);
            } else if (__counter == 4000) {
                __counter = ~0;
                cycleFlag = TRUE;
            }
            break;

        case LED_FLASH:
            if (__counter == 0) {
                GPIO_SetLED(GPIO_LED_ON);
            } else if (__counter == 50) {
                GPIO_SetLED(GPIO_LED_OFF);
            } else if (__counter == 1000) {
                __counter = ~0;
                cycleFlag = TRUE;
            }
            break;
    }

    __stateChanged = FALSE;

    if (cycleFlag) {
        if (__times != LED_NO_TIMEOUT) {
            __times -= 1;
            if (__times == 0) {
                LED_SetState(__fallbackState, LED_NO_TIMEOUT);
                __stateChanged = TRUE; // Force.
            }
        }
    }
}
