#include "pl_uart.h"

//==============================================================================

extern "C" void app_main(void) {
  PL::Uart uart(UART_NUM_0);
  uart.Initialize();

  uart.SetBaudRate(115200);
  uart.SetDataBits(8);
  uart.SetParity(PL::UartParity::none);
  uart.SetStopBits(PL::UartStopBits::one);
  uart.SetFlowControl(PL::UartFlowControl::none);
  uart.Enable();

  char testData[] = "Test\n";
  uart.Write(testData, sizeof(testData));
}