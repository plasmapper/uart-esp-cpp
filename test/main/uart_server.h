#include "pl_uart.h"

//==============================================================================

class UartServer : public PL::UartServer {
public:
  using PL::UartServer::UartServer;
  esp_err_t HandleRequest (PL::Uart& uart) override;
};

//==============================================================================

void TestUartServer();