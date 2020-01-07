/*
 * By:              A.G.
 * Created:         N/A
 * Last modified:   2020.01.04
 */

#ifndef COSMIC_DEFINITIONS_H
#define COSMIC_DEFINITIONS_H

#if defined(__CSMC__)
#include "csmcreal.h"
#else
#include "csmcmasked.h"
#endif

typedef void FAR_MODIFIER (*InterruptHandlerProcedure_t)(void);

#define SFR(name, addr) volatile unsigned char name AT_MEMADDR(addr)

#define ISR(vect) FAR_MODIFIER INTERRUPT_MODIFIER void vect(void)

#define DECLARE_ISR(vect) extern INTERRUPT_MODIFIER ISR(vect)

# endif // ------------------------------------------- End of Include Guard ---
