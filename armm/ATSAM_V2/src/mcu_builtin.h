/*
 *  file:     mcu_builtin.h (ATSAM_V2)
 *  brief:    Built-in ATSAM_V2 MCU definitions
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#ifndef __MCU_BUILTIN_H
#define __MCU_BUILTIN_H

#ifdef LITTLE_ENDIAN
  #undef LITTLE_ENDIAN
#endif

#if 0

//----------------------------------------------------------------------
// Atmel v2
//----------------------------------------------------------------------

#elif defined(MCU_ATSAMD51J18)

  #define MCUF_ATSAM_V2
  #define MCUSF_E5X

  #define __SAMD51J18A__
  #include "samd51.h"

#elif defined(MCU_ATSAMD51J19)

  #define MCUF_ATSAM_V2
  #define MCUSF_E5X

  #define __SAMD51J19A__
  #include "samd51.h"

#elif defined(MCU_ATSAMD51P19)

  #define MCUF_ATSAM_V2
  #define MCUSF_E5X

  #define __SAMD51P19A__
  #include "samd51.h"

#elif defined(MCU_ATSAMD51J20)

  #define MCUF_ATSAM_V2
  #define MCUSF_E5X

  #define __SAMD51J20A__
  #include "samd51.h"

#elif defined(MCU_ATSAME51J20)

  #define MCUF_ATSAM_V2
  #define MCUSF_E5X

  #define __SAME51J20A__
  #include "same51.h"

#elif defined(MCU_ATSAME51J18)

  #define MCUF_ATSAM_V2
  #define MCUSF_E5X

  #define __SAME51J18A__
  #include "same51.h"

#elif defined(MCU_ATSAME51J19)

  #define MCUF_ATSAM_V2
  #define MCUSF_E5X

  #define __SAME51J19A__
  #include "same51.h"

#elif defined(MCU_ATSAMC21J18)

  #define MCUF_ATSAM_V2
  #define MCUSF_C2X

  #define __SAMC21J18A__
  #include "samc21.h"

#elif defined(MCU_ATSAMD10J18)

  #define MCUF_ATSAM_V2
  #define MCUSF_DXX

  #define __SAMD10J18A__
  #include "samd10.h"

#else

  #error "Unknown MCU"

#endif

#endif
