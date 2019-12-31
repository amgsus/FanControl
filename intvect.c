#include "__TYPES.h"
#include "cosmic.h"

typedef struct {
    BYTE instruction;
    InterruptHandlerProcedure_t serviceRoutinePtr;
} InterruptTableEntry_t;

#if defined(__CSMC__)
extern void _stext(); /* RESET vector routine */
#else
void _stext() {}
#endif

// ISRs //

ISR(NonHandledInterrupt) {
    while (1); // ToDo: In order to detected unexpected interrupts put a breakpoint here.
}

// Interrupt Vector Table //

#define OPCODE 0x82 /* See PM0044 sheet */

InterruptTableEntry_t const _vectab[] = {
    {OPCODE, (InterruptHandlerProcedure_t) _stext}, /* Reset */
    {OPCODE, NonHandledInterrupt}, /* Software Trap */
    {OPCODE, NonHandledInterrupt}, /* IRQ0  : TLI */
    {OPCODE, NonHandledInterrupt}, /* IRQ1  : AWU */
    {OPCODE, NonHandledInterrupt}, /* IRQ2  : CLK */
    {OPCODE, NonHandledInterrupt}, /* IRQ3  : EXTI0 (PAx) */
    {OPCODE, NonHandledInterrupt}, /* IRQ4  : EXTI1 (PBx) */
    {OPCODE, NonHandledInterrupt}, /* IRQ5  : EXTI2 (PCx) */
    {OPCODE, NonHandledInterrupt}, /* IRQ6  : EXTI3 (PDx) */
    {OPCODE, NonHandledInterrupt}, /* IRQ7  : EXTI4 (PEx) */
    {OPCODE, NonHandledInterrupt}, /* IRQ8  */
    {OPCODE, NonHandledInterrupt}, /* IRQ9  */
    {OPCODE, NonHandledInterrupt}, /* IRQ10 : SPI */
    {OPCODE, NonHandledInterrupt}, /* IRQ11 : TIM1 Update/OVF/UVF/Break */
    {OPCODE, NonHandledInterrupt}, /* IRQ12 : TIM1 CAPCOM */
    {OPCODE, NonHandledInterrupt}, /* IRQ13 : TIM2 Update/OVF */
    {OPCODE, NonHandledInterrupt}, /* IRQ14 : TIM2 CAPCOM */
    {OPCODE, NonHandledInterrupt}, /* IRQ15 */
    {OPCODE, NonHandledInterrupt}, /* IRQ16 */
    {OPCODE, NonHandledInterrupt}, /* IRQ17 : UART1 TXE */
    {OPCODE, NonHandledInterrupt}, /* IRQ18 : UART1 RXNE */
    {OPCODE, NonHandledInterrupt}, /* IRQ19 : I2C */
    {OPCODE, NonHandledInterrupt}, /* IRQ20 */
    {OPCODE, NonHandledInterrupt}, /* IRQ21 */
    {OPCODE, NonHandledInterrupt}, /* IRQ22 : ADC1 EOC */
    {OPCODE, NonHandledInterrupt}, /* IRQ23 : TIM4 Update/OVF */
    {OPCODE, NonHandledInterrupt}, /* IRQ24 : Flash EOP/WR_PG_DIS */
    {OPCODE, NonHandledInterrupt}, /* IRQ25 */
    {OPCODE, NonHandledInterrupt}, /* IRQ26 */
    {OPCODE, NonHandledInterrupt}, /* IRQ27 */
    {OPCODE, NonHandledInterrupt}, /* IRQ28 */
    {OPCODE, NonHandledInterrupt}, /* IRQ29 */
};
