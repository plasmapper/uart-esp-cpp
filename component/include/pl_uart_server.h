#pragma once
#include "pl_common.h"
#include "pl_uart_types.h"
#include "pl_uart_port.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief UART server class
class UartServer : public Server {
public:
  /// @brief Default UART server task parameters
  static const TaskParameters defaultTaskParameters;

  /// @brief Create an UART server
  /// @param port UART port
  UartServer (std::shared_ptr<UartPort> port);
  ~UartServer();
  UartServer (const UartServer&) = delete;
  UartServer& operator= (const UartServer&) = delete;

  esp_err_t Lock (TickType_t timeout = portMAX_DELAY) override;
  esp_err_t Unlock() override;

  esp_err_t Enable() override;
  esp_err_t Disable() override;

  bool IsEnabled() override;

  /// @brief Get the UART port
  /// @return UART port
  std::shared_ptr<UartPort> GetPort();
  
  /// @brief Set the UART port
  /// @param port UART port
  /// @return error code
  esp_err_t SetPort (std::shared_ptr<UartPort> port);

  /// @brief Set the server task parameters
  /// @param taskParameters task parameters
  /// @return error code
  esp_err_t SetTaskParameters (const TaskParameters& taskParameters);

protected:
  /// @brief Handle the UART client request
  /// @param port UART port
  /// @return error code
  virtual esp_err_t HandleRequest (UartPort& port) = 0;

private:
  Mutex mutex;
  enum class Status {stopped, starting, started, stopping} status = Status::stopped;
  std::shared_ptr<UartPort> port;
  TaskParameters taskParameters = defaultTaskParameters;

  static void TaskCode (void* parameters);
};

//==============================================================================

}