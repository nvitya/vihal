/* -----------------------------------------------------------------------------
 * This file is a part of the VIHAL project: https://github.com/nvitya/vihal
 * Copyright (c) 2023 Viktor Nagy, nvitya
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */
/*
 *  file:     hwpins_msp.cpp
 *  brief:    MSP Pin/Pad and GPIO configuration
 *  date:     2023-06-27
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"

#ifdef MCUSF_M0C
  #define MAX_PORT_NUMBER   1
#else
  #define MAX_PORT_NUMBER   2
#endif

#if defined(MCUSF_M0G)

// The Pin-to IOMUX table made from the datasheet (first column of the "Pin Attributes" table)
//   WARNING: they are +1 than the IOMUX->SECCFG.PINCM[] indexes !!!

const uint8_t pincm_table[64] =
{
   1, // A0
   2, // A1
   7, // A2
   8, // A3
   9, // A4
  10, // A5
  11, // A6
  14, // A7
  19, // A8
  20, // A9
  21, // A10
  22, // A11
  34, // A12
  35, // A13
  36, // A14
  37, // A15
  38, // A16
  39, // A17
  40, // A18
  41, // A19
  42, // A20
  46, // A21
  47, // A22
  53, // A23
  54, // A24
  55, // A25
  59, // A26
  60, // A27
   3, // A28
   4, // A29
   5, // A30
   6, // A31

  12, // B0
  13, // B1
  15, // B2
  16, // B3
  17, // B4
  18, // B5
  23, // B6
  24, // B7
  25, // B8
  26, // B9
  27, // B10
  28, // B11
  29, // B12
  30, // B13
  31, // B14
  32, // B15
  33, // B16
  43, // B17
  44, // B18
  45, // B19
  48, // B20
  49, // B21
  50, // B22
  51, // B23
  52, // B24
  56, // B25
  57, // B26
  58, // B27

   0,  // B28 (non-existing)
   0,  // B29 (non-existing)
   0,  // B30 (non-existing)
   0   // B31 (non-existing)
};

#elif defined(MCUSF_M0C)

// The Pin-to IOMUX table made from the datasheet (first column of the "Pin Attributes" table)
//   WARNING: they are +1 than the IOMUX->SECCFG.PINCM[] indexes !!!

const uint8_t pincm_table[32] =
{
   1, // A0
   2, // A1
   3, // A2
   0, // A3  (non-existing)
   5, // A4
   0, // A5  (non-existing)
   7, // A6
   0, // A7  (non-existing)
   0, // A8  (non-existing)
   0, // A9  (non-existing)
   0, // A10 (non-existing)
  12, // A11
   0, // A12 (non-existing)
   0, // A13 (non-existing)
   0, // A14 (non-existing)
   0, // A15 (non-existing)
  17, // A16
  18, // A17
  19, // A18
  20, // A19
  21, // A20
   0, // A21 (non-existing)
  23, // A22
  24, // A23
  25, // A24
  26, // A25
  27, // A26
  28, // A27
  29, // A28
   0, // A29 (non-existing)
   0, // A30 (non-existing)
   0, // A31 (non-existing)
};

#else

  #error "IOMUX index table is missing for this family."

#endif

uint8_t get_pincm_index(unsigned aportnum, unsigned apinnum)
{
	unsigned idx = ((aportnum & 1) << 5) + (apinnum & 0x1F);
	return pincm_table[idx];
}

GPIO_Regs * THwPinCtrl_msp::GetGpioRegs(int aportnum)
{
	if ((aportnum < 0) || (aportnum >= MAX_PORT_NUMBER))
	{
		return nullptr;
	}
	else
	{
		if (aportnum == PORTNUM_B)
		{
      #if !defined(MCUSF_M0C)
			  return GPIOB;
			#else
			  return nullptr;
			#endif
		}
		else
		{
			return GPIOA;
		}
	}
}


bool THwPinCtrl_msp::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	GPIO_Regs * regs = GetGpioRegs(aportnum);
	if (!regs)
	{
		return false;
	}

	if ((apinnum < 0) || (apinnum > 31))
	{
		return false;
	}

	GpioPortEnable(aportnum);

	unsigned pincmidx = get_pincm_index(aportnum, apinnum);
	if (pincmidx < 1)
	{
		return false;
	}

  unsigned cmreg = IOMUX_PINCM_PC_CONNECTED;
  unsigned pinmask  = (1 << apinnum);

	if (flags & PINCFG_DRIVE_STRONG)
	{
		cmreg |= IOMUX_PINCM_DRV_MASK;
	}

#if 0
	if (0 == (flags & PINCFG_OUTPUT))
	{
		cmreg |= IOMUX_PINCM_INENA_MASK;
	}
#endif
	cmreg |= IOMUX_PINCM_INENA_MASK;

	if (flags & PINCFG_AF_MASK)
	{
		cmreg |= ((flags >> PINCFG_AF_SHIFT) & 0x1F);
	}
	else // GPIO or analgoue input
	{
		cmreg |= (1 | IOMUX_PINCM_INENA_MASK);  // AF_1 = GPIO, + input enable
		if (flags & PINCFG_OUTPUT)
		{
		  if (flags & PINCFG_GPIO_INIT_1)
		  {
		  	regs->DOUTSET31_0 = pinmask;
		  }
		  else
		  {
		  	regs->DOUTCLR31_0 = pinmask;
		  }
			regs->DOESET31_0 = pinmask;
		}
		else
		{
			regs->DOECLR31_0 = pinmask;

			if (flags & PINCFG_ANALOGUE)  // no hysteresis for analogue inputs
			{
				//cmreg &= ~(IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_MASK);
				cmreg = 0;
			}
			else
			{
				cmreg |= IOMUX_PINCM_HYSTEN_MASK;
			}
		}
	}

  if (flags & PINCFG_OPENDRAIN)
  {
  	cmreg |= IOMUX_PINCM_HIZ1_MASK;
  }

  if (flags & PINCFG_PULLUP)
  {
  	cmreg |= IOMUX_PINCM_PIPU_MASK;
  }

  if (flags & PINCFG_PULLDOWN)
  {
  	cmreg |= IOMUX_PINCM_PIPD_MASK;
  }

  IOMUX->SECCFG.PINCM[pincmidx-1] = cmreg;

  return true;
}

bool THwPinCtrl_msp::GpioPortEnable(int aportnum)
{
	GPIO_Regs * regs = GetGpioRegs(aportnum);
	if (!regs)
	{
		return false;
	}

  regs->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
  return true;
}

void THwPinCtrl_msp::GpioSet(int aportnum, int apinnum, int value)
{
	GPIO_Regs * regs = GetGpioRegs(aportnum);
	if (!regs)
	{
		return;
	}

  if (1 == value)
  {
  	regs->DOUTSET31_0 = (1 << apinnum);
  }
  else if (value & 2) // toggle
  {
  	regs->DOUTTGL31_0 = (1 << apinnum);
  }
  else
  {
  	regs->DOUTCLR31_0 = (1 << apinnum);
  }
}

void THwPinCtrl_msp::GpioIrqSetup(int aportnum, int apinnum, int amode)
{

}

// GPIO Port

void TGpioPort_msp::Assign(int aportnum)
{
	regs = hwpinctrl.GetGpioRegs(aportnum);
}

void TGpioPort_msp::Set(unsigned value)
{
	regs->DOUT31_0 = value;
}

// GPIO Pin

void TGpioPin_msp::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	regs = hwpinctrl.GetGpioRegs(aportnum);
	if (!regs)
	{
		return;
	}

	setbitvalue = (1 << pinnum);
	clrbitvalue = (1 << pinnum);

  getbitptr    = (unsigned *)&(regs->DIN31_0);
  getoutbitptr = (unsigned *)&(regs->DOUT31_0);
  getbitshift = pinnum;
  togglebitptr = (unsigned *)&(regs->DOUTTGL31_0);

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->DOUTCLR31_0);
    clrbitptr = (unsigned *)&(regs->DOUTSET31_0);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->DOUTSET31_0);
    clrbitptr = (unsigned *)&(regs->DOUTCLR31_0);
  }
}

void TGpioPin_msp::SwitchDirection(int adirection)
{
	if (adirection)
	{
		regs->DOESET31_0 = setbitvalue; // output enable
	}
	else
	{
		regs->DOECLR31_0 = setbitvalue; // output disable
	}
}

TGpioOut::TGpioOut(int aportnum, int apinnum, bool ainvert)
{
	Assign(aportnum, apinnum, ainvert);
}

bool TGpioOut::Setup(unsigned flags)
{
	if (inverted)
	{
	  flags |= PINCFG_GPIO_INVERT;
	}

	return hwpinctrl.GpioSetup(this->portnum, this->pinnum, flags);
}

void TGpioOut::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

  GPIO_Regs * regs = hwpinctrl.GetGpioRegs(aportnum);
	if (!regs)
	{
		return;
	}

	setbitvalue = (1 << pinnum);

  if (inverted)
  {
    setbitptr = (uint32_t *)&(regs->DOUTCLR31_0);
    clrbitptr = (uint32_t *)&(regs->DOUTSET31_0);
  }
  else
  {
    setbitptr = (uint32_t *)&(regs->DOUTSET31_0);
    clrbitptr = (uint32_t *)&(regs->DOUTCLR31_0);
  }
}

