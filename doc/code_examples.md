# VIHAL Code Examples

GPIO Setup on STM32F103 Minimum Development Board:
```C++
TGpioPin  led1pin(PORTNUM_C, 13, false);
void setup_board()
{
	led1pin.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);
}
```

GPIO Setup on Arduino DUE (ATSAM3X8E):
```C++
TGpioPin  led1pin(1, 27, false); // D13
void setup_board()
{
	led1pin.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);
}
```

GPIO Setup on LPC4330-XPlorer (independent Pin and GPIO numbering):
```C++
TGpioPin  led1pin(1, 12, true); // D2
TGpioPin  led2pin(1, 11, true); // D3
void setup_board()
{
	hwpinctrl.PinSetup(2, 12, PINCFG_OUTPUT | PINCFG_AF_0);  // D2: GPIO_1_12, pad B9
	hwpinctrl.PinSetup(2, 11, PINCFG_OUTPUT | PINCFG_AF_0);  // D3: GPIO_1_11, pad A9
	led1pin.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);
	led2pin.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1);
}
```
GPIO Usage:
```C++
++hbcounter;
led1pin.SetTo(hbcounter >> 0);
led1pin.Set0();  // fast inline code on most MCUs
led1pin.Set1();
```
UART Setup (STM32F103) and Usage:
```C++
hwpinctrl.PinSetup(PORTNUM_A,  9,  PINCFG_OUTPUT | PINCFG_AF_0);  // USART1_TX
hwpinctrl.PinSetup(PORTNUM_A, 10,  PINCFG_INPUT  | PINCFG_AF_0);  // USART1_RX

THwUart   conuart;
conuart.Init(1);  // USART1, use default settings: 115200, 8, 1, n
conuart.printf("Hello World %i !\r\n", 1);
```
