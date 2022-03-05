// oleddisp_i2c.cpp

#include "oleddisp_i2c.h"

bool TOledDisp_i2c::InitInterface()
{
	// test communication

  if (!pi2c)
  {
    return false;
  }

	uint8_t testcmd[2] = {0x00, 0x00};

	pi2c->StartWriteData(i2caddress, 0, &testcmd[0], 2);
	pi2c->WaitFinish();
	return (pi2c->error == 0);
}

void TOledDisp_i2c::WriteCmd(uint8_t adata)
{
	cmdbuf[0] = 0x00;
	cmdbuf[1] = adata;
	pi2c->WaitFinish();
	pi2c->StartWriteData(i2caddress, 0, &cmdbuf[0], 2);
	pi2c->WaitFinish();
}

void TOledDisp_i2c::Run()
{
	uint32_t n;

	switch (updatestate)
	{
	case 0: // wait for update request
		if (lastupdate != updatecnt)
		{
			updatestate = 1;  Run();  return;
		}
		break;

	case 1: // start update
		if (!pi2c->Finished())
		{
			return;
		}
		n = 0;
		cmdbuf[n++] = 0x80; cmdbuf[n++] = 0x21; // set column start+end address;
		cmdbuf[n++] = 0x80; cmdbuf[n++] = 0;    // start column
		cmdbuf[n++] = 0x80; cmdbuf[n++] = hwwidth - 1;  // end column
		cmdbuf[n++] = 0x80; cmdbuf[n++] = 0x22; // set page start+end
		cmdbuf[n++] = 0x80; cmdbuf[n++] = 0;    // start page
		cmdbuf[n++] = 0x80; cmdbuf[n++] = (hwheight >> 3) - 1;   // end page
		cmdbuf[n-2] = 0x00; // replace 0x80 with 0x00 for command chain termination
		pi2c->StartWriteData(i2caddress, 0, &cmdbuf[0], n);
		updatestate = 2;
		break;

	case 2: // wait preparation finish
		if (!pi2c->Finished())
		{
			return;
		}

		if (pi2c->error)
		{
			++commerrcnt;
			updatestate = 0; // repeat command phase
		}
		else
		{
			updatestate = 5; Run(); return;
		}
		break;

	case 5: // start sending data
		lastupdate = updatecnt;
		pi2c->StartWriteData(i2caddress, 0x40 | I2CEX_1, pdispbuf, hwwidth * (hwheight >> 3));
		updatestate = 6;
		break;

	case 6: // wait until the buffer write finished
		if (!pi2c->Finished())
		{
			return;
		}

		if (pi2c->error)
		{
			++commerrcnt;
		}

		updatestate = 0; // return the idle state
		break;
	}
}
