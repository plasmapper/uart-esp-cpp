#include "uart_server.h"
#include "unity.h"
#include "esp_check.h"

//==============================================================================

const uart_port_t port1Number = UART_NUM_1;
const uart_port_t port2Number = UART_NUM_2;
const uint8_t dataToSend[] = {1, 2, 3, 4, 5};
const uint8_t disableDataToSend[] = {0xFE, 0, 0, 0, 0};
const uint8_t restartDataToSend[] = {0xFF, 0, 0, 0, 0};
static uint8_t receivedData[sizeof(dataToSend)] = {};
static const char* TAG = "pl_uart_server_test";

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

  // Test server disable and restart from request
  TEST_ASSERT_EQUAL (sizeof (disableDataToSend), uart_write_bytes (port2Number, disableDataToSend, sizeof (disableDataToSend)));
  vTaskDelay (10);
  TEST_ASSERT (!server.IsEnabled());
  
  TEST_ASSERT (server.Enable() == ESP_OK);
  TEST_ASSERT (server.IsEnabled());

  TEST_ASSERT_EQUAL (sizeof (restartDataToSend), uart_write_bytes (port2Number, restartDataToSend, sizeof (restartDataToSend)));
  vTaskDelay (10);
  TEST_ASSERT (server.IsEnabled());
  
  TEST_ASSERT (server.Disable() == ESP_OK);
  TEST_ASSERT (!server.IsEnabled());
}

//==============================================================================

esp_err_t UartServer::HandleRequest (PL::UartPort& port) {
  ESP_RETURN_ON_ERROR (port.Read (receivedData, sizeof (receivedData)), TAG, "port read failed");
  if (receivedData[0] == disableDataToSend[0])
    ESP_RETURN_ON_ERROR (Disable(), TAG, "server disable failed");
  if (receivedData[0] == restartDataToSend[0]) {
    ESP_RETURN_ON_ERROR (Disable(), TAG, "server disable failed");
    ESP_RETURN_ON_ERROR (Enable(), TAG, "server enable failed");
  }
  return ESP_OK;
}