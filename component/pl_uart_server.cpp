#include "pl_uart_server.h"
#include "esp_check.h"

//==============================================================================

static const char* TAG = "pl_uart_server";

//==============================================================================

namespace PL {

//==============================================================================

const TaskParameters UartServer::defaultTaskParameters = {4096, tskIDLE_PRIORITY + 5, 0};

//==============================================================================

UartServer::UartServer (std::shared_ptr<Uart> uart) : uart (uart) {}

//==============================================================================

UartServer::~UartServer() {
  if (status != Status::stopped) {
    status = Status::stopping;
    while (status == Status::stopping)
      vTaskDelay(1);
  }
}

//==============================================================================

esp_err_t UartServer::Lock (TickType_t timeout) {
  esp_err_t error = mutex.Lock (timeout);
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR (error, TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Unlock() {
  ESP_RETURN_ON_ERROR (mutex.Unlock(), TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Enable() {
  LockGuard lg (*this);
  if (handlingRequest) {
    enableFromRequest = true;
    return ESP_OK;
  }
  if (status == Status::started)
    return ESP_OK;

  status = Status::starting;
  if (xTaskCreatePinnedToCore (TaskCode, GetName().c_str(), taskParameters.stackDepth, this, taskParameters.priority, NULL, taskParameters.coreId) != pdPASS) {
    status = Status::stopped;
    ESP_RETURN_ON_ERROR (ESP_FAIL, TAG, "task create failed");
  }
  while (status == Status::starting)
    vTaskDelay(1);
  ESP_RETURN_ON_FALSE (status == Status::started, ESP_FAIL, TAG, "enable failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Disable() {
  LockGuard lg (*this);
  if (handlingRequest) {
    disableFromRequest = true;
    return ESP_OK;
  }
  if (status == Status::stopped)
    return ESP_OK;
  
  status = Status::stopping;
  while (status == Status::stopping)
    vTaskDelay(1);
  ESP_RETURN_ON_FALSE (status == Status::stopped, ESP_FAIL, TAG, "disable failed");
  return ESP_OK;
}

//==============================================================================

bool UartServer::IsEnabled() {
  LockGuard lg (*this);
  return status == Status::started;
}

//==============================================================================

std::shared_ptr<Uart> UartServer::GetUart() {
  LockGuard lg (*this);
  return uart;
}

//==============================================================================

esp_err_t UartServer::SetUart (std::shared_ptr<Uart> uart) {
  LockGuard lg (*this);
  this->uart = uart;
  ESP_RETURN_ON_ERROR (RestartIfEnabled(), TAG, "restart failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::SetTaskParameters (const TaskParameters& taskParameters) {
  LockGuard lg (*this);
  this->taskParameters = taskParameters;
  ESP_RETURN_ON_ERROR (RestartIfEnabled(), TAG, "restart failed");
  return ESP_OK;
}

//==============================================================================

void UartServer::TaskCode (void* parameters) {
  UartServer& server = *(UartServer*)parameters;

  server.status = Status::started;
  server.enabledEvent.Generate();

  while (server.status != Status::stopping && !server.disableFromRequest) {
    if (server.Lock(0) == ESP_OK) {
      LockGuard lg (*server.uart);
      if (server.uart->GetReadableSize()) {
        server.handlingRequest = true;
        server.HandleRequest(*server.uart);
        server.handlingRequest = false;
        if (server.enableFromRequest)
          server.disableFromRequest = false;
        server.enableFromRequest = false;
      }        
      server.Unlock();
    }
    vTaskDelay(1);
  }

  server.disableFromRequest = false;
  server.status = Status::stopped;
  server.disabledEvent.Generate();

  vTaskDelete (NULL);
}

//==============================================================================

esp_err_t UartServer::RestartIfEnabled() {
  if (status == Status::stopped || disableFromRequest)
    return ESP_OK;
  ESP_RETURN_ON_ERROR (Disable(), TAG, "disable failed");
  ESP_RETURN_ON_ERROR (Enable(), TAG, "enable failed");
  return ESP_OK;
}

//==============================================================================

}