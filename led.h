#ifndef LED_H
#define LED_H

#include "__TYPES.h"

#define LED_TASK_INTERVAL             1 /* ms */

enum
    LED_STATE
{
    LED_OFF,
    LED_ON,
    LED_BLINK,
    LED_SLOW_BLINK,
    LED_FLASH
};

void
LED_Init(void);
void
LED_Task(void);
void
LED_SetFallbackState(enum LED_STATE state); // Sets the state of LED after timeout for current rotation reached.
void
LED_SetState(enum LED_STATE state, BYTE times); // Times specifies a timeout in milliseconds (for ON state) or the number of times the LED blinks.

#define LED_NO_TIMEOUT ((BYTE) ~0)

#define LED_TIMES_UART_COMM_OK 2
#define REPORT_USART_COMM_OK() \
    do { LED_SetState(LED_BLINK, LED_TIMES_UART_COMM_OK); } while (0)

#define LED_SetStateNoTimeout(state) \
    do { LED_SetState((state), LED_NO_TIMEOUT); } while (0);

# endif // ------------------------------------------- End of Include Guard ---
