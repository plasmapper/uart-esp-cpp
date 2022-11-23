#include "pl_uart.h"

//==============================================================================

class UartEchoServer : public PL::UartServer {
public:
  using PL::UartServer::UartServer;

protected:
  esp_err_t HandleRequest (PL::Uart& uart) override;
};

//==============================================================================

extern "C" void app_main(void) {
  auto uart = std::make_shared<PL::Uart>(UART_NUM_0);
  uart->Initialize();

  uart->SetBaudRate (115200);
  uart->SetDataBits (8);
  uart->SetParity (PL::UartParity::none);
  uart->SetStopBits (PL::UartStopBits::one);
  uart->SetFlowControl (PL::UartFlowControl::none);
  uart->Enable();

  UartEchoServer server (uart);
  server.Enable();

  while (1) {
    vTaskDelay (1);
  }
}

//==============================================================================

esp_err_t UartEchoServer::HandleRequest (PL::Uart& uart) {
  uint8_t dataByte;
  while (uart.GetReadableSize()) {
    if (uart.Read (&dataByte, 1) == ESP_OK)
      uart.Write (&dataByte, 1);
  }
  return ESP_OK;
}