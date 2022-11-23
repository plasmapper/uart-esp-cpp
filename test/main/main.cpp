#include "unity.h"
#include "uart_base.h"
#include "uart_server.h"

//==============================================================================

extern "C" void app_main(void) {
  UNITY_BEGIN();
  RUN_TEST (TestUart);
  RUN_TEST (TestUartServer);
  UNITY_END();
}