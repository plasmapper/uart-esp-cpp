#pragma once
#include "pl_common.h"
#include "pl_uart_types.h"
#include "pl_uart_base.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief UART server class
class UartServer : public Server {
public:
  /// @brief Default server task parameters
  static const TaskParameters defaultTaskParameters;

  /// @brief Create an UART server
  /// @param uart UART
  UartServer (std::shared_ptr<Uart> uart);
  ~UartServer();
  UartServer (const UartServer&) = delete;
  UartServer& operator= (const UartServer&) = delete;

  esp_err_t Lock (TickType_t timeout = portMAX_DELAY) override;
  esp_err_t Unlock() override;

  esp_err_t Enable() override;
  esp_err_t Disable() override;

  bool IsEnabled() override;

  /// @brief Get the UART
  /// @return UART
  std::shared_ptr<Uart> GetUart();
  
  /// @brief Set the UART
  /// @param uart UART
  /// @return error code
  esp_err_t SetUart (std::shared_ptr<Uart> uart);

  /// @brief Set the server task parameters
  /// @param taskParameters task parameters
  /// @return error code
  esp_err_t SetTaskParameters (const TaskParameters& taskParameters);

protected:
  /// @brief Handle the UART client request
  /// @param uart UART
  /// @return error code
  virtual esp_err_t HandleRequest (Uart& uart) = 0;

private:
  Mutex mutex;
  std::shared_ptr<Uart> uart;
  TaskParameters taskParameters = defaultTaskParameters;
  TaskHandle_t taskHandle = NULL;
  bool disable = false;
  bool disableFromRequest = false;
  bool enableFromRequest = false;

  static void TaskCode (void* parameters);
  esp_err_t RestartIfEnabled(); 
};

//==============================================================================

}