#include "pl_uart.h"

//==============================================================================

extern "C" void app_main(void) {
  PL::UartPort port (UART_NUM_0);
  port.Initialize();

  port.SetBaudRate (115200);
  port.SetDataBits (8);
  port.SetParity (PL::UartParity::none);
  port.SetStopBits (PL::UartStopBits::one);
  port.SetFlowControl (PL::UartFlowControl::none);
  port.Enable();

  char testData[] = "Test\n";
  port.Write (testData, sizeof (testData));
}