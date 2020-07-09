/*
    Pterodon - Macro definitions
    Copyright (C) <2019> ATGDroid <bythedroid@gmail.com>

    This file is part of Pterodon Recovery

    Pterodon is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pterodon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pterodon.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _PTERO_CONSTANTS_H
#define _PTERO_CONSTANTS_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h> // TEMP_FAILURE_RETRY
#include <stdio.h>
#include "common.h"

/* enable for debugging */
#define PTERODON_DEBUG_MODE


/* enable for debugging of the AOSP property api */
//#define PTERODON_PROPERTYSERVICE_DEBUG

// #include <boost/date_time/posix_time/posix_time.hpp>



/* DEFINE "VARIABLE FUNCTIONS" */


#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp)            \
  ({                                       \
    decltype(exp) _rc;                     \
    do {                                   \
      _rc = (exp);                         \
    } while (_rc == -1 && errno == EINTR); \
    _rc;                                   \
  })
#endif

/* Pterodon logging system */
#define LOGE(...) \
  fprintf(stdout,"[E]: " __VA_ARGS__); \
  putc('\n', stdout);
#define LOGI(...) \
  fprintf(stdout,"[I]: " __VA_ARGS__); \
  putc('\n', stdout);
#define LOGW(...) \
  fprintf(stdout,"[W]: " __VA_ARGS__); \
  putc('\n', stdout);
#ifdef PTERODON_DEBUG_MODE
#define LOGD(...) \
  fprintf(stdout,"[D]: " __VA_ARGS__); \
  putc('\n', stdout);
#else
//#define LOGD(...) do {} while (0)
#define LOGD(...)
#endif

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

#define PTERO_SQUARE(a) ((a)*(a))
#define PTERO_MAX(a, b) (a>b ? a : b)
#define PTERO_MIN(a, b) (a>b ? b : a)

#define PTERO_TO_PB(a) ((a) / (1024) / (1024) / (1024) / (1024) / (1024))
#define PTERO_TO_TB(a) ((a) / (1024) / (1024) / (1024) / (1024))
#define PTERO_TO_GB(a) ((a) / (1024) / (1024) / (1024))
#define PTERO_TO_MB(a) ((a) / (1024) / (1024))
#define PTERO_TO_KB(a) ((a) / (1024))

#define ptero_bit(x) (1 << x)
#define ptero_isSpace(c)	((c) == ' ' || (c) == '\t')
#define ptero_isAlpha(c)	(((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define ptero_isDigit(c)	((uint8_t)(c) - '0' <= 9)
#define ptero_isAlphaDigit(c)	((c) >= 'a' && (c) <= 'f')
#define ptero_isHexDigit(c)	(isDigit(c) || isAlphaDigit(c))
#define ptero_isAlphaNumeric(c) (isAlpha(c) || isDigit(c) || ((c) == '-')) 


/* DEFINE environment paths to specific objects */

#define PTERO_COMMAND_FILE     "/cache/recovery/command"
#define PTERO_CMDLINE_PATH       "/proc/cmdline"
#define PTERO_TMP_LOG                  "/tmp/recovery.log"
#define PTERO_TMP_SIDELOAD       "/tmp/sideload"
#define PTERO_LAST_LOGFILE         "/cache/recovery/last_log"
#define PTERO_UPDATED_LOGFILE         "/cache/recovery/log"
#define PTERO_MOUNT_ENTRY        "/proc/mounts"

/* Handle ifdefs */

#ifndef PTERO_VIBRATION_DRIVER_PATH
#define PTERO_VIBRATION_DRIVER_PATH "/sys/class/timed_output/vibrator/enable"
#endif

#ifndef PTERO_USB_DRIVER_STATE
#define PTERO_USB_DRIVER_STATE "/sys/class/android_usb/android0/state"
#endif

#ifndef PTERO_MOUNT_ENTRY_FILE
#define PTERO_MOUNT_ENTRY_FILE "/proc/mounts"
#endif

#ifndef PTERO_LEDS_PATH
#define PTERO_LEDS_PATH               "/sys/class/leds/green"
#endif

#ifndef PTERO_BATTERY_DRIVER_PATH
#define PTERO_BATTERY_DRIVER_PATH               "/sys/class/power_supply/battery"
#endif


/* DEFINE recovery specific variables */

#endif  // _PTERO_CONSTANTS_H
