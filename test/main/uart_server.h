#include "pl_common.h"

//==============================================================================

class UartServer : public PL::StreamServer {
public:
  using PL::StreamServer::StreamServer;
  esp_err_t HandleRequest(PL::Stream& stream) override;
};

//==============================================================================

void TestUartServer();