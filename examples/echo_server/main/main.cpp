#include "pl_uart.h"

//==============================================================================

class UartEchoServer : public PL::UartServer {
public:
  using PL::UartServer::UartServer;
  esp_err_t HandleRequest (PL::UartPort& port) override;
};

//==============================================================================

extern "C" void app_main(void) {
  auto port = std::make_shared<PL::UartPort>(UART_NUM_0);
  port->Initialize();

  port->SetBaudRate (115200);
  port->SetDataBits (8);
  port->SetParity (PL::UartParity::none);
  port->SetStopBits (PL::UartStopBits::one);
  port->SetFlowControl (PL::UartFlowControl::none);
  port->Enable();

  UartEchoServer server (port);
  server.Enable();

  while (1) {
    vTaskDelay (1);
  }
}

//==============================================================================

esp_err_t UartEchoServer::HandleRequest (PL::UartPort& port) {
  uint8_t dataByte;
  while (port.GetReadableSize()) {
    PL_RETURN_ON_ERROR (port.Read (&dataByte, 1));
    PL_RETURN_ON_ERROR (port.Write (&dataByte, 1));
  }
  return ESP_OK;
}