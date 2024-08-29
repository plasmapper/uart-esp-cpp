#include "pl_uart_server.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

UartServer::UartServer(std::shared_ptr<Uart> uart) : StreamServer(uart), uart(uart) {}

//==============================================================================

UartServer::~UartServer() {}

//==============================================================================

std::shared_ptr<Uart> UartServer::GetUart() {
  LockGuard lg(*this);
  return uart;
}

//==============================================================================

esp_err_t UartServer::SetUart(std::shared_ptr<Uart> uart) {
  LockGuard lg(*this);
  this->uart = uart;
  return SetStream(uart);
}

//==============================================================================

esp_err_t UartServer::HandleRequest(PL::Stream& stream) {
  return HandleRequest((PL::Uart&)stream);
}

//==============================================================================

}