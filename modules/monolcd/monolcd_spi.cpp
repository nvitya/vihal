// monolcd_spi.cpp

#include "monolcd_spi.h"
#include "clockcnt.h"

bool TMonoLcd_spi::InitInterface()
{
	if (!pspi
			|| !pin_cs.Assigned()
			|| !pin_cd.Assigned())
	{
		return false;
	}

	if (pin_reset.Assigned())
	{
		pin_reset.Set0();
		delay_us(100000);
		pin_reset.Set1();
	}

  // Reset the controller state...
	pin_cd.Set0();
	pin_cs.Set0();
	delay_us(100000);
	pin_cs.Set1();

	return true;
}

void TMonoLcd_spi::WriteCmd(uint8_t adata) // blocking, but normally used only for the initialization
{
	pin_cd.Set0();
	pin_cs.Set0();
	pspi->StartTransfer(adata,  0,  SPITR_CMD1, 0, nullptr, nullptr);
	pspi->WaitFinish();
	pspi->WaitSendFinish();
	pin_cs.Set1();
}

void TMonoLcd_spi::WriteData(uint8_t adata)  // blocking, but normally used only for the initialization
{
	pin_cd.Set1();
	pin_cs.Set0();
  pspi->StartTransfer(adata,  0,  SPITR_CMD1, 0, nullptr, nullptr);
  pspi->WaitFinish();
  pspi->WaitSendFinish();
	pin_cs.Set1();
}

void TMonoLcd_spi::Run()
{
  if (!pspi)
  {
    return;
  }

  pspi->Run();

	switch (updatestate)
	{
	case 0: // wait for update request
		if (lastupdate == updatecnt)
		{
			return;
		}

		lastupdate = updatecnt;
		current_page = 0;
		dataptr = pdispbuf;
		updatestate = 2;

		// no break !

	case 2: // send commands

		if (MLCD_CTRL_ST75256 == ctrltype) // quite different commanding
		{
			// now the not optimized version
			WriteCmd(0x75); // set page (y) address
			WriteData(0x00); // starting address
			WriteData((hwheight - 1) >> 3);

			WriteCmd(0x15); // set page (y) address
			WriteData(0x00); // starting address
			WriteData(hwwidth - 1);

			WriteCmd(0x5C); // start writing data

			updatestate = 4; // continue with sending data
			break;
		}

		pin_cd.Set0(); // set to command
		if (MLCD_CTRL_PCD8544 == ctrltype)
		{
			cmdbuf[0] = 0x40 | current_page; // set y address
			cmdbuf[1] = 0x80 | 0; // set column start address / MSB
			cmdcnt = 2;
		}
		else
		{
			cmdbuf[0] = 0x00 + (rotation == 2 ? 4 : 0); // set column start address / LSB
			cmdbuf[1] = 0x10; // set column start address / MSB
			cmdbuf[2] = 0xB0 + current_page; // set page address
			cmdcnt = 3;
		}

		cmdidx = 0;

		pin_cs.Set0();
		updatestate = 3;

		pspi->StartTransfer(0, 0, 0, cmdcnt, &cmdbuf[0], nullptr);

		// no break !

	case 3: // sending command bytes

	  if (!pspi->finished)
	  {
	    return;
	  }

		if (!pspi->SendFinished())
		{
			return;
		}

		pin_cs.Set1();
		updatestate = 4;
		// no break !

	case 4: // start send row data

		row_remaining = hwwidth;
		pin_cd.Set1();
		pin_cs.Set0();
		updatestate = 5;

    pspi->StartTransfer(0, 0, 0, row_remaining, dataptr, nullptr);

		// no break;

	case 5: // sending row data

    if (!pspi->finished)
    {
      return;
    }

    if (!pspi->SendFinished())
    {
      return;
    }

    dataptr += hwwidth;

		pin_cs.Set1();

		// go to the next row
		++current_page;
		if (current_page < (hwheight >> 3))
		{
			if (MLCD_CTRL_ST75256 == ctrltype) // quite different commanding
			{
				updatestate = 4; // do not set the address
			}
			else
			{
				updatestate = 2;
			}

			Run(); return;
		}
		else
		{
			// the end.
			updatestate = 0;
		}
		break;
	}
}
