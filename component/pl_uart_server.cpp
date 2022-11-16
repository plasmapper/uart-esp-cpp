#include "pl_uart_server.h"
#include "esp_check.h"

//==============================================================================

static const char* TAG = "pl_uart_server";

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
  esp_err_t error = mutex.Lock (timeout);
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR (error, TAG, "uart server lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Unlock() {
  ESP_RETURN_ON_ERROR (mutex.Unlock(), TAG, "uart server unlock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Enable() {
  LockGuard lg (*this);
  if (status == Status::started)
    return ESP_OK;
  
  status = Status::starting;
  if (xTaskCreatePinnedToCore (TaskCode, GetName().c_str(), taskParameters.stackDepth, this, taskParameters.priority, NULL, taskParameters.coreId) != pdPASS) {
    status = Status::stopped;
    ESP_RETURN_ON_ERROR (ESP_FAIL, TAG, "uart server create task failed");
  }
  while (status == Status::starting)
    vTaskDelay(1);
  ESP_RETURN_ON_FALSE (status == Status::started, ESP_FAIL, TAG, "uart server enable failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Disable() {
  LockGuard lg (*this);
  if (status == Status::stopped)
    return ESP_OK;

  status = Status::stopping;
  while (status == Status::stopping)
    vTaskDelay(1);
  ESP_RETURN_ON_FALSE (status == Status::stopped, ESP_FAIL, TAG, "uart server disable failed");
  return ESP_OK;
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
  ESP_RETURN_ON_ERROR (RestartIfEnabled(), TAG, "uart server restart failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::SetTaskParameters (const TaskParameters& taskParameters) {
  LockGuard lg (*this);
  this->taskParameters = taskParameters;
  ESP_RETURN_ON_ERROR (RestartIfEnabled(), TAG, "uart server restart failed");
  return ESP_OK;
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
  ESP_RETURN_ON_ERROR (Disable(), TAG, "uart server disable failed");
  ESP_RETURN_ON_ERROR (Enable(), TAG, "uart server enable failed");
  return ESP_OK;
}

//==============================================================================

}