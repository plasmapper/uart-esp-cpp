#include "pl_uart_server.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

UartServer::UartServer(std::shared_ptr<Uart> uart) : StreamServer(uart) {}

//==============================================================================

UartServer::~UartServer() {}

//==============================================================================

std::shared_ptr<Uart> UartServer::GetUart() {
  return std::static_pointer_cast<Uart>(GetStream());
}

//==============================================================================

esp_err_t UartServer::SetUart(std::shared_ptr<Uart> uart) {
  return SetStream(uart);
}

//==============================================================================

esp_err_t UartServer::HandleRequest(PL::Stream& stream) {
  return HandleRequest((PL::Uart&)stream);
}

//==============================================================================

}