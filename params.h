/*
 * By:              A.G.
 * Created:         2020.01.01
 * Last modified:   2020.01.03
 */

#ifndef FANCONTROL_FLASHPARAMS_H
#define FANCONTROL_PARAMS_H

// ----------------------------------------------------------------------------

#include "__TYPES.h"

void
SetInfoRegisters(BYTE resetStatus);

void
LoadDefaultParameters(void);

BOOL
LoadParameters(void);

BOOL
SaveParameters(void);

# endif // ------------------------------------------- End of Include Guard ---
