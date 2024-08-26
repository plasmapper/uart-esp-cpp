#include "uart_base.h"
#include "unity.h"

//==============================================================================

const uart_port_t portNumber = UART_NUM_1;
const uint32_t baudRate = 19200;
const uint16_t dataBits = 7;
const PL::UartParity parity = PL::UartParity::even;
const PL::UartStopBits stopBits = PL::UartStopBits::two;
const PL::UartFlowControl flowControl = PL::UartFlowControl::rtsCts;
const TickType_t timeout = 1000 / portTICK_PERIOD_MS;
const uint8_t dataToSend[] = {1, 2, 3, 4, 5};

//==============================================================================

void TestUart() {
  PL::Uart uart(portNumber);
  TEST_ASSERT(uart.GetName() == PL::Uart::defaultName + "1");
  TEST_ASSERT_EQUAL(PL::Uart::defaultReadTimeout, uart.GetReadTimeout());
  TEST_ASSERT_EQUAL(PL::Uart::defaultBaudRate, uart.GetBaudRate());
  TEST_ASSERT_EQUAL(PL::Uart::defaultDataBits, uart.GetDataBits());
  TEST_ASSERT_EQUAL(PL::Uart::defaultParity, uart.GetParity());
  TEST_ASSERT_EQUAL(PL::Uart::defaultStopBits, uart.GetStopBits());
  TEST_ASSERT_EQUAL(PL::Uart::defaultFlowControl, uart.GetFlowControl());

  TEST_ASSERT(uart.Initialize() == ESP_OK);

  TEST_ASSERT(uart.EnableLoopback() == ESP_OK);
  TEST_ASSERT(uart.SetMode(UART_MODE_UART) == ESP_OK);
  
  TEST_ASSERT(uart.SetBaudRate(baudRate) == ESP_OK);
  TEST_ASSERT_EQUAL(baudRate, uart.GetBaudRate());
  TEST_ASSERT(uart.SetDataBits(dataBits) == ESP_OK);
  TEST_ASSERT_EQUAL(dataBits, uart.GetDataBits());
  TEST_ASSERT(uart.SetParity(parity) == ESP_OK);
  TEST_ASSERT_EQUAL(parity, uart.GetParity());
  TEST_ASSERT(uart.SetStopBits(stopBits) == ESP_OK);
  TEST_ASSERT_EQUAL(stopBits, uart.GetStopBits());
  TEST_ASSERT(uart.SetFlowControl(flowControl) == ESP_OK);
  TEST_ASSERT_EQUAL(flowControl, uart.GetFlowControl());
  TEST_ASSERT(uart.SetReadTimeout(timeout) == ESP_OK);
  TEST_ASSERT_EQUAL(timeout, uart.GetReadTimeout());

  TEST_ASSERT(uart.Enable() == ESP_OK);
  TEST_ASSERT(uart.IsEnabled());

  TEST_ASSERT(uart.Write(dataToSend, sizeof(dataToSend)) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT_EQUAL(sizeof(dataToSend), uart.GetReadableSize());
  uint8_t receivedData[sizeof(dataToSend)];
  TEST_ASSERT(uart.Read(receivedData, sizeof(receivedData)) == ESP_OK);
  for (int i = 0; i < sizeof(dataToSend); i++)
    TEST_ASSERT_EQUAL(dataToSend[i], receivedData[i]);

  TEST_ASSERT(uart.Disable() == ESP_OK);
  TEST_ASSERT(uart.DisableLoopback() == ESP_OK);
  TEST_ASSERT(!uart.IsEnabled());
}