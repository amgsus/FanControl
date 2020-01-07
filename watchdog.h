/*
 * By:              A.G.
 * Created:         2020.01.01
 * Last modified:   2020.01.03
 */

#ifndef FANCONTROL_WATCHDOG_H
#define FANCONTROL_WATCHDOG_H

// ----------------------------------------------------------------------------

#include "__TYPES.h"
#include "mcu.h"

#define IWDG_KEY_REFRESH 0xAA

void
ReloadWatchdog(void);

# endif // ------------------------------------------- End of Include Guard ---


