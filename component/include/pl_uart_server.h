#pragma once
#include "pl_common.h"
#include "pl_uart_types.h"
#include "pl_uart_base.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief UART server class
class [[deprecated("Use more generic StreamServer class instead.")]] UartServer : public StreamServer {
public:
  /// @brief Creates an UART server
  /// @param uart UART
  UartServer(std::shared_ptr<Uart> uart);
  ~UartServer();
  UartServer(const UartServer&) = delete;
  UartServer& operator=(const UartServer&) = delete;

  /// @brief Gets the UART
  /// @return UART
  std::shared_ptr<Uart> GetUart();
  
  /// @brief Sets the UART
  /// @param uart UART
  /// @return error code
  esp_err_t SetUart(std::shared_ptr<Uart> uart);

protected:
  esp_err_t HandleRequest(PL::Stream& stream) override;

  /// @brief Handles the UART client request
  /// @param uart UART
  /// @return error code
  virtual esp_err_t HandleRequest(Uart& uart) = 0;

private:
  std::shared_ptr<Uart> uart;
};

//==============================================================================

}