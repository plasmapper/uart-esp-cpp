#include "uart_port.h"
#include "unity.h"

//==============================================================================

const uart_port_t portNumber = UART_NUM_1;
const uint32_t baudRate = 19200;
const PL::UartParity parity = PL::UartParity::even;
const PL::UartStopBits stopBits = PL::UartStopBits::two;
const PL::UartFlowControl flowControl = PL::UartFlowControl::rtsCts;
const TickType_t timeout = 1000 / portTICK_PERIOD_MS;
const uint8_t dataToSend[] = {1, 2, 3, 4, 5};

//==============================================================================

void TestUartPort() {
  PL::UartPort port (portNumber);
  TEST_ASSERT (port.GetName() == PL::UartPort::defaultName + "1");
  TEST_ASSERT_EQUAL (PL::UartPort::defaultReadTimeout, port.GetReadTimeout());
  TEST_ASSERT_EQUAL (PL::UartPort::defaultBaudRate, port.GetBaudRate());
  TEST_ASSERT_EQUAL (PL::UartPort::defaultDataBits, port.GetDataBits());
  TEST_ASSERT_EQUAL (PL::UartPort::defaultParity, port.GetParity());
  TEST_ASSERT_EQUAL (PL::UartPort::defaultStopBits, port.GetStopBits());
  TEST_ASSERT_EQUAL (PL::UartPort::defaultFlowControl, port.GetFlowControl());

  TEST_ASSERT (port.Initialize() == ESP_OK);

  TEST_ASSERT (port.EnableLoopback() == ESP_OK);
  TEST_ASSERT (port.SetMode (UART_MODE_UART) == ESP_OK);
  
  TEST_ASSERT (port.SetBaudRate (baudRate) == ESP_OK);
  TEST_ASSERT_EQUAL (baudRate, port.GetBaudRate());
  TEST_ASSERT (port.SetParity (parity) == ESP_OK);
  TEST_ASSERT_EQUAL (parity, port.GetParity());
  TEST_ASSERT (port.SetStopBits (stopBits) == ESP_OK);
  TEST_ASSERT_EQUAL (stopBits, port.GetStopBits());
  TEST_ASSERT (port.SetFlowControl (flowControl) == ESP_OK);
  TEST_ASSERT_EQUAL (flowControl, port.GetFlowControl());
  TEST_ASSERT (port.SetReadTimeout (timeout) == ESP_OK);
  TEST_ASSERT_EQUAL (timeout, port.GetReadTimeout());

  TEST_ASSERT (port.Enable() == ESP_OK);
  TEST_ASSERT (port.IsEnabled());

  TEST_ASSERT (port.Write (dataToSend, sizeof (dataToSend)) == ESP_OK);
  vTaskDelay (10);
  TEST_ASSERT_EQUAL (sizeof (dataToSend), port.GetReadableSize());
  uint8_t receivedData[sizeof(dataToSend)];
  TEST_ASSERT (port.Read (receivedData, sizeof (receivedData)) == ESP_OK);
  for (int i = 0; i < sizeof (dataToSend); i++)
    TEST_ASSERT_EQUAL (dataToSend[i], receivedData[i]);

  TEST_ASSERT (port.Disable() == ESP_OK);
  TEST_ASSERT (port.DisableLoopback() == ESP_OK);
  TEST_ASSERT (!port.IsEnabled());
}