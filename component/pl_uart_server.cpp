#include "pl_uart_server.h"

//==============================================================================

namespace PL {

//==============================================================================

const TaskParameters UartServer::defaultTaskParameters = {4096, tskIDLE_PRIORITY + 5, 0};

//==============================================================================

UartServer::UartServer (std::shared_ptr<UartPort> port) : port (port) {}

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
  return mutex.Lock (timeout);
}

//==============================================================================

esp_err_t UartServer::Unlock() {
  return mutex.Unlock();
}

//==============================================================================

esp_err_t UartServer::Enable() {
  LockGuard lg (*this);
  if (status == Status::started)
    return ESP_OK;
  
  status = Status::starting;
  if (xTaskCreatePinnedToCore (TaskCode, GetName().c_str(), taskParameters.stackDepth, this, taskParameters.priority, NULL, taskParameters.coreId) != pdPASS) {
    status = Status::stopped;
    return ESP_FAIL;
  }
  while (status == Status::starting)
    vTaskDelay(1);
  return (status == Status::started)?(ESP_OK):(ESP_FAIL);
}

//==============================================================================

esp_err_t UartServer::Disable() {
  LockGuard lg (*this);
  if (status == Status::stopped)
    return ESP_OK;

  status = Status::stopping;
  while (status == Status::stopping)
    vTaskDelay(1);
  return (status == Status::stopped)?(ESP_OK):(ESP_FAIL);
}

//==============================================================================

bool UartServer::IsEnabled() {
  LockGuard lg (*this);
  return status == Status::started;
}

//==============================================================================

std::shared_ptr<UartPort> UartServer::GetPort() {
  LockGuard lg (*this);
  return port;
}

//==============================================================================

esp_err_t UartServer::SetPort (std::shared_ptr<UartPort> port) {
  LockGuard lg (*this);
  this->port = port;
  return RestartIfEnabled();
}

//==============================================================================

esp_err_t UartServer::SetTaskParameters (const TaskParameters& taskParameters) {
  LockGuard lg (*this);
  this->taskParameters = taskParameters;
  return RestartIfEnabled();
}

//==============================================================================

void UartServer::TaskCode (void* parameters) {
  UartServer& server = *(UartServer*)parameters;

  server.status = Status::started;
  server.enabledEvent.Generate();

  while (server.status != Status::stopping) {
    if (server.Lock(0) == ESP_OK) {
      LockGuard lg (*server.port);
      if (server.port->GetReadableSize())
        server.HandleRequest(*server.port);
      server.Unlock();
    }
    vTaskDelay(1);
  }
  server.status = Status::stopped;
  server.disabledEvent.Generate();

  vTaskDelete (NULL);
}

//==============================================================================

esp_err_t UartServer::RestartIfEnabled() {
  if (status == Status::stopped)
    return ESP_OK;
  PL_RETURN_ON_ERROR (Disable());
  return Enable();
}

//==============================================================================

}