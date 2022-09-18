#include "uart_server.h"
#include "unity.h"

//==============================================================================

const uart_port_t port1Number = UART_NUM_1;
const uart_port_t port2Number = UART_NUM_2;
const uint8_t dataToSend[] = {1, 2, 3, 4, 5};
static uint8_t receivedData[sizeof(dataToSend)] = {};

//==============================================================================

void TestUartServer() {
  auto port1 = std::make_shared<PL::UartPort>(port1Number);
  TEST_ASSERT (port1->Initialize() == ESP_OK);
  TEST_ASSERT (port1->EnableLoopback() == ESP_OK);
  TEST_ASSERT (port1->Enable() == ESP_OK);
  
  auto port2 = std::make_shared<PL::UartPort>(port2Number);
  TEST_ASSERT (port2->Initialize() == ESP_OK);
  TEST_ASSERT (port2->EnableLoopback() == ESP_OK);
  TEST_ASSERT (port2->Enable() == ESP_OK);

  UartServer server(port1);
  TEST_ASSERT (port1 == server.GetPort());

  TEST_ASSERT (server.Enable() == ESP_OK);
  TEST_ASSERT (server.IsEnabled());
  TEST_ASSERT_EQUAL (sizeof (dataToSend), uart_write_bytes (port1Number, dataToSend, sizeof (dataToSend)));
  vTaskDelay (10);
  for (int i = 0; i < sizeof (dataToSend); i++)
    TEST_ASSERT_EQUAL (dataToSend[i], receivedData[i]);

  for (int i = 0; i < sizeof (dataToSend); i++)
    receivedData[i] = 0;
  TEST_ASSERT (server.SetPort (port2) == ESP_OK);
  TEST_ASSERT (port2 == server.GetPort());
  TEST_ASSERT_EQUAL (sizeof (dataToSend), uart_write_bytes (port2Number, dataToSend, sizeof (dataToSend)));
  vTaskDelay (10);
  for (int i = 0; i < sizeof (dataToSend); i++)
    TEST_ASSERT_EQUAL (dataToSend[i], receivedData[i]);
  
  TEST_ASSERT (server.Disable() == ESP_OK);
  TEST_ASSERT (!server.IsEnabled());
}

//==============================================================================

esp_err_t UartServer::HandleRequest (PL::UartPort& port) {
  return port.Read (receivedData, sizeof (receivedData));
}