#include "unity.h"
#include "uart_port.h"
#include "uart_server.h"

//==============================================================================

extern "C" void app_main(void) {
  UNITY_BEGIN();
  RUN_TEST (TestUartPort);
  RUN_TEST (TestUartServer);
  UNITY_END();
}