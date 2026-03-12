
#ifndef HWSPI_LPC_H_
#define HWSPI_LPC_H_

#define HWSPI_PRE_ONLY
#include "hwspi.h"

class THwSpi_lpc : public THwSpi_pre
{
public:
	bool Init(int adevnum);

	bool TrySendData(uint8_t adata);
	bool TryRecvData(uint8_t * dstptr);
	bool SendFinished();

	void DmaAssign(bool istx, THwDmaChannel * admach);

	bool DmaStartSend(THwDmaTransfer * axfer);
	bool DmaStartRecv(THwDmaTransfer * axfer);
	bool DmaSendCompleted();
	bool DmaRecvCompleted();

public:
	unsigned  					basespeed;
	HW_SPI_REGS * 			regs;

  void SetCs(unsigned value);
};


#define HWSPI_IMPL THwSpi_lpc

#endif // def HWSPI_LPC_H_
