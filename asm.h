/*
 * By:              A.G.
 * Created:         2020.01.01
 * Last modified:   2020.01.01
 */

#ifndef STM8_ASM_H
#define STM8_ASM_H

// ----------------------------------------------------------------------------

#ifndef __CSMC__
#define _asm(x)
# endif

#define Halt()              _asm("hlt")
#define WaitForEvent()      _asm("wfe")
#define WaitForInterrupt()  _asm("wfi")
#define MaskInterrupts()    _asm("sim")
#define UnmaskInterrupts()  _asm("rim")
#define Nop()               _asm("nop")
#define Trap()              _asm("trap")

# endif // ------------------------------------------- End of Include Guard ---
