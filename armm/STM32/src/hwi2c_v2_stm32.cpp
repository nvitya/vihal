/* -----------------------------------------------------------------------------
 * This file is a part of the NVCM project: https://github.com/nvitya/nvcm
 * Copyright (c) 2018 Viktor Nagy, nvitya
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
 *  file:     hwi2c_stm32.cpp
 *  brief:    STM32 I2C / TWI
 *  version:  1.00
 *  date:     2018-02-10
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwi2c.h"

#include "stm32_utils.h"

#include "traces.h"

#ifdef RCC_APB1ENR1_I2C1EN  // G4
  #define RCC_APB1ENR_I2C1EN     RCC_APB1ENR1_I2C1EN
  #define RCC_APB1ENR_I2C2EN     RCC_APB1ENR1_I2C2EN
  #define RCC_APB1ENR_I2C3EN     RCC_APB1ENR1_I2C3EN
#elif defined(RCC_APBENR1_I2C1EN)  // G0
  #define RCC_APB1ENR_I2C1EN     RCC_APBENR1_I2C1EN
  #define RCC_APB1ENR_I2C2EN     RCC_APBENR1_I2C2EN
  #define RCC_APB1ENR_I2C3EN     RCC_APBENR1_I2C3EN
#elif defined(RCC_APB1LENR_I2C1EN) // H7
  #define RCC_APB1ENR_I2C1EN     RCC_APB1LENR_I2C1EN
  #define RCC_APB1ENR_I2C2EN     RCC_APB1LENR_I2C2EN
  #define RCC_APB1ENR_I2C3EN     RCC_APB1LENR_I2C3EN
#elif defined(RCC_APB1ENR1_I2C2EN) // H7RS
  #define RCC_APB1ENR_I2C1EN     RCC_APB1ENR1_I2C1_I3C1EN
  #define RCC_APB1ENR_I2C2EN     RCC_APB1ENR1_I2C2EN
  #define RCC_APB1ENR_I2C3EN     RCC_APB1ENR1_I2C3EN
#endif

#if I2C_HW_VER == 2

bool THwI2c_stm32::Init(int adevnum)
{
	uint8_t busid = STM32_BUSID_APB1;

	initialized = false;

	devnum = adevnum;

	regs = nullptr;

	if (false)
	{
	}
#ifdef I2C1
	else if (1 == devnum)
	{
		regs = I2C1;
		APB1ENR_REGISTER |= RCC_APB1ENR_I2C1EN;

		#ifdef RCC_CFGR3_I2C1SW
			RCC->CFGR3 |= RCC_CFGR3_I2C1SW; // select system clock for the source instead of the HSI
		#endif
	}
#endif
#ifdef I2C2
	else if (2 == devnum)
	{
		regs = I2C2;
		APB1ENR_REGISTER |= RCC_APB1ENR_I2C2EN;
	}
#endif
#ifdef I2C3
	else if (3 == devnum)
	{
		regs = I2C3;
		APB1ENR_REGISTER |= RCC_APB1ENR_I2C3EN;
	}
#endif
#ifdef I2C4
	else if (4 == devnum)
	{
		regs = I2C4;
    #ifdef RCC_APB1ENR2_I2C4EN
		  RCC->APB1ENR2 |= RCC_APB1ENR2_I2C4EN;
		#elif defined RCC_APB4ENR_I2C4EN
      RCC->APB4ENR |= RCC_APB4ENR_I2C4EN;
    #else
  		RCC->APB1ENR |= RCC_APB1ENR_I2C4EN;
    #endif
	}
#endif
	if (!regs)
	{
		return false;
	}

	// disable for setup
	regs->CR1 &= ~I2C_CR1_PE;  // resets the I2C internal logic too
	while (regs->CR1 & I2C_CR1_PE)
	{
		// wait until flag is cleared
	}

	unsigned cr1 = 0
		| (0 << 20)      // SMBBUS BITS(4)
		| (0 << 19)      // GCEN: general call enable
		| (0 << 17)      // NOSTRECH
		| (0 << 16)      // SBC: slave byte control
		| (0 << 15)      // RXDMAEN
		| (0 << 14)      // TXDMAEN
		| (0 << 12)      // ANFOFF: 0 = enabled
		| (0 <<  8)      // DNF(3): digital nois filter
		| (0 <<  1)      // IE(7): interrupt enable bits
		| (0 <<  0)      // PE: 1 = enable
	;
	regs->CR1 = cr1;

	unsigned periphclock = stm32_bus_speed(busid);

	// TIMING

	regs->TIMEOUTR = 0; // disable timeout detection

	unsigned halfclockdiv = (periphclock / (speed * 2));
	unsigned presc = 0;
	while (halfclockdiv > 255)
	{
		++presc;
		halfclockdiv = (periphclock / ((1 + presc) * (speed * 2)));
	}

	regs->TIMINGR = 0
		| (presc << 28)  // PRESC: timing prescaler
		| (1 << 20)  // SCLDEL(4): Data setup time
		| (1 << 16)  // SDADEL(4): Data hold time
		| ((halfclockdiv) << 8) // SCLL(8): low period clocks
		| ((halfclockdiv) << 0) // SCLH(8): high period clocks
	;

	cr1 |= 1;  // enable I2C
	regs->CR1 |= cr1;

	initialized = true;

	return true;
}

void THwI2c_stm32::DmaAssign(bool istx, THwDmaChannel * admach)
{
  if (istx)
  {
    txdma = admach;
    if (admach)
    {
      admach->Prepare(istx, (void *)&regs->TXDR, 0);
    }
  }
  else
  {
    rxdma = admach;
    if (admach)
    {
      admach->Prepare(istx, (void *)&regs->RXDR, 0);
    }
  }
}

void THwI2c_stm32::RunTransaction()
{
  unsigned cr2;

  if (0 == trastate)  // transaction init
  {
    istx = (0 != curtra->iswrite);
    devaddr = curtra->address;
    dataptr = (uint8_t *)curtra->dataptr;
    datalen = curtra->datalen;
    remainingbytes = datalen;
    waitreload = false;

    extracnt = ((curtra->extra >> 24) & 3);
    if (extracnt)
    {
      // reverse byte order
      //uint32_t edr = __REV(aextra);
      if (1 == extracnt)
      {
        extradata[0] = (curtra->extra & 0xFF);
      }
      else if (2 == extracnt)
      {
        extradata[0] = ((curtra->extra >> 8) & 0xFF);
        extradata[1] = ((curtra->extra >> 0) & 0xFF);
      }
      else if (3 == extracnt)
      {
        extradata[0] = ((curtra->extra >> 16) & 0xFF);
        extradata[1] = ((curtra->extra >>  8) & 0xFF);
        extradata[2] = ((curtra->extra >>  0) & 0xFF);
      }
    }
    extraremaining = extracnt;

    // clear all interrupt flags
    regs->ICR = 0x3F38;

    if (istx)
    {
      dmaused = ((txdma != nullptr) && (datalen > 1));
      if (dmaused)
      {
        regs->CR1 &= ~I2C_CR1_RXDMAEN;
        regs->CR1 |=  I2C_CR1_TXDMAEN;
      }
      else
      {
        regs->CR1 &= ~(I2C_CR1_TXDMAEN | I2C_CR1_RXDMAEN);
      }
    }
    else
    {
      dmaused = ((rxdma != nullptr) && (datalen > 1));
      if (dmaused)
      {
        regs->CR1 &= ~I2C_CR1_TXDMAEN;
        regs->CR1 |=  I2C_CR1_RXDMAEN;
      }
      else
      {
        regs->CR1 &= ~(I2C_CR1_TXDMAEN | I2C_CR1_RXDMAEN);
      }
    }

    cr2 = 0
      | (0 << 26)     // PECBYTE
      | (0 << 25)     // AUTOEND
      | (0 << 24)     // RELOAD
      | (0 << 16)     // NBYTES(7)
      | (0 << 15)     // NACK
      | (0 << 14)     // STOP
      | (0 << 13)     // START
      | (0 << 12)     // HEAD10R
      | (0 << 11)     // ADD10
      | (0 << 10)     // RD_WRN
      | (devaddr << 1)   // SADD(10), bits 7..1 are used for 7-bit addressing!
    ;
    regs->CR2 = cr2;

    regs->ICR = 0xFFFF; // clear all sticky errors

    trastate = 1;  // prepare to the start
  }

	//uint8_t  firstbyte;
	unsigned isr = regs->ISR;
	unsigned nbytes;

	// check error flags
	if (!curtra->error)
	{
		if (isr & I2C_ISR_NACKF)
		{
		  curtra->error = ERR_I2C_ACK;
		}
		else if (isr & I2C_ISR_ARLO)
		{
		  curtra->error = ERR_I2C_ARBLOST;
		}
		else if (isr & I2C_ISR_BERR)
		{
		  curtra->error = ERR_I2C_BUS;
		}
		else if (isr & I2C_ISR_OVR)
		{
		  curtra->error = ERR_I2C_OVERRUN;
		}

		if (curtra->error)
		{
			// jump to error handling
			trastate = 90;
		}
	}

	switch (trastate)
	{
	case 1:  // wait until busy to start
		// wait until busy
		if (isr & I2C_ISR_BUSY)
		{
			return;
		}

		cr2 = regs->CR2 & 0x3FF; // keep the slave address

		if (extraremaining > 0)
		{
			// start with sending the extra data, no autoend, no reload
			nbytes = extraremaining;
			if (istx)
			{
				cr2 |= I2C_CR2_RELOAD;
				cr2 |= I2C_CR2_AUTOEND;
			}
			waitreload = true;
			trastate = 5;
		}
		else if (istx)
		{
			nbytes = remainingbytes;
			if (nbytes > 255)
			{
				nbytes = 255;
				cr2 |= I2C_CR2_RELOAD;
			}
			cr2 |= I2C_CR2_AUTOEND;
			trastate = 10;
		}
		else // rx
		{
			cr2 |= I2C_CR2_RD_WRN;
			nbytes = remainingbytes;
			if (nbytes > 255)
			{
				nbytes = 255;
				cr2 |= I2C_CR2_RELOAD;
			}
			cr2 |= I2C_CR2_AUTOEND;
			trastate = 20;
		}

		cr2 |= (nbytes << 16) | I2C_CR2_START;  // start the transmission
		regs->CR2 = cr2;
		break;

	case 5: // send extra bytes
		if (isr & I2C_ISR_TXIS)
		{
			regs->TXDR = extradata[extracnt - extraremaining];
			--extraremaining;
			if (extraremaining == 0)
			{
				if (istx)
				{
					trastate = 10;  RunTransaction();  return;
				}
				else
				{
					trastate = 6;  RunTransaction();  return;
				}
			}
		}
		break;

	case 6:  // send re-start
		if (isr & I2C_ISR_TC)
		{
			cr2 = regs->CR2 & 0x3FF; // keep the slave address

			nbytes = remainingbytes;
			if (nbytes > 255)
			{
				nbytes = 255;
				cr2 |= I2C_CR2_RELOAD;
			}
			cr2 |= I2C_CR2_AUTOEND;

			cr2 |= (nbytes << 16) | I2C_CR2_START | I2C_CR2_RD_WRN;
			regs->CR2 = cr2;
			waitreload = false;
			trastate = 20;
		}
		break;

	case 10: // sending bytes

		if (isr & I2C_ISR_TCR) // reload required?
		{
			cr2 = regs->CR2 & 0x3FF; // keep the slave address
			nbytes = remainingbytes;
			if (nbytes > 255)
			{
				nbytes = 255;
				cr2 |= I2C_CR2_RELOAD;
			}
			cr2 |= I2C_CR2_AUTOEND;
			cr2 |= (nbytes << 16);
			regs->CR2 = cr2;
			waitreload = false;
		}

		if (dmaused)
		{
			if (txdma->Active())
			{
				return; // wait until DMA finishes
			}

			if (remainingbytes > 0)
			{
				if (waitreload)
				{
					return;
				}

				if ((isr & I2C_ISR_TXIS) == 0)  // there were stop problems when this was not here ... if someone nows better, please tell
				{
					return;
				}

				// (re-)start DMA

				xfer.srcaddr = dataptr;
				xfer.bytewidth = 1;
				cr2 = regs->CR2;
				xfer.count = ((cr2 >> 16) & 0xFF); // todo: check count zero
				xfer.flags = 0;
				dataptr += xfer.count;
				remainingbytes -= xfer.count;

				txdma->StartTransfer(&xfer);

				waitreload = (remainingbytes > 0);
			}
			else
			{
			  trastate = 29; // finish
			}
			return;
		}

		// else manual mode

		if (remainingbytes > 0)
		{
			// manual sending

			if ((isr & I2C_ISR_TXIS) == 0)  // TX Ready?
			{
				return;
			}

			regs->TXDR = *dataptr++;
			--remainingbytes;

			if (remainingbytes > 0)
			{
				return;
			}
		}
		trastate = 29; // finish
		break;

	case 20: //	receiving bytes
		if (isr & I2C_ISR_TCR) // reload required?
		{
			cr2 = regs->CR2 & 0x3FF; // keep the slave address
			nbytes = remainingbytes;
			if (nbytes > 255)
			{
				nbytes = 255;
				cr2 |= I2C_CR2_RELOAD;
			}
			cr2 |= I2C_CR2_AUTOEND;
			cr2 |= (nbytes << 16); // I2C_CR2_START;
			regs->CR2 = cr2;
			waitreload = false;
		}

		if (dmaused)
		{
			if (rxdma->Active())
			{
				return; // wait until DMA finishes
			}

			if (remainingbytes > 0)
			{
				if (waitreload)
				{
					return;
				}

				// (re-)start DMA

				xfer.dstaddr = dataptr;
				xfer.bytewidth = 1;
				cr2 = regs->CR2;
				xfer.count = ((cr2 >> 16) & 0xFF); // todo: check count zero
				xfer.flags = 0;
				dataptr += xfer.count;
				remainingbytes -= xfer.count;

				rxdma->StartTransfer(&xfer);

				waitreload = (remainingbytes > 0);
			}
			else
			{
			  trastate = 29; // finish
			}
			return;
		}

		// working without DMA

		if ((isr & I2C_ISR_RXNE) == 0)  // RX Ready?
		{
			return;
		}

		*dataptr++ = regs->RXDR;
		--remainingbytes;

		if (remainingbytes > 0)
		{
			return;
		}
		trastate = 29; // terminate
		break;

	case 29: // wait last transfer to finish and send stop
		if (isr & I2C_ISR_STOPF)
		{
		  trastate = 30;
			return;
		}

		if ((isr & I2C_ISR_TC) == 0)
		{
			return;
		}

		if ((regs->CR2 & I2C_CR2_AUTOEND) == 0)
		{
			regs->CR2 |= I2C_CR2_STOP;  // send stop
		}
		trastate = 30; // closing
		break;

	case 30: // closing
		if ((isr & I2C_ISR_STOPF) == 0)
		{
			return;
		}

		regs->ICR = I2C_ICR_STOPCF;

		curtra->completed = true; // transaction finished.
		trastate = 50;
	  break;

	case 50: // finished
		break;

	case 90: // handling errors
		if (dmaused)
		{
		  if (rxdma)  rxdma->Disable();
		  if (txdma)  txdma->Disable();
		}

		if (isr & I2C_ISR_BUSY)
		{
		  regs->CR2 |= I2C_CR2_STOP;  // send stop condition
		}
		trastate = 91;
		break;

	case 91:
		// todo: reset on timeout
		if (isr & I2C_ISR_BUSY)   // wait until end of the transfer
		{
			return;
		}
		regs->ICR = (I2C_ICR_STOPCF | I2C_ICR_NACKCF); // clear the stop flag
    curtra->completed = true; // transaction finished.
		trastate = 50;
		break;

	} // case
}

#endif
