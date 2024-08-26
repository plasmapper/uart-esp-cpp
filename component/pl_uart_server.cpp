#include "pl_uart_server.h"
#include "esp_check.h"

//==============================================================================

static const char* TAG = "pl_uart_server";

//==============================================================================

namespace PL {

//==============================================================================

const TaskParameters UartServer::defaultTaskParameters = {4096, tskIDLE_PRIORITY + 5, 0};

//==============================================================================

UartServer::UartServer(std::shared_ptr<Uart> uart) : uart(uart) {}

//==============================================================================

UartServer::~UartServer() {
  while (taskHandle) {
    disable = true;
    vTaskDelay(1);
  }
}

//==============================================================================

esp_err_t UartServer::Lock(TickType_t timeout) {
  esp_err_t error = mutex.Lock(timeout);
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR(error, TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Unlock() {
  ESP_RETURN_ON_ERROR(mutex.Unlock(), TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Enable() {
  LockGuard lg(*this);
  if (taskHandle == xTaskGetCurrentTaskHandle()) {
    enableFromRequest = true;
    return ESP_OK;
  }
  if (taskHandle)
    return ESP_OK;
  
  disable = false;
  if (xTaskCreatePinnedToCore(TaskCode, GetName().c_str(), taskParameters.stackDepth, this, taskParameters.priority, &taskHandle, taskParameters.coreId) != pdPASS) {
    taskHandle = NULL;
    ESP_RETURN_ON_ERROR(ESP_FAIL, TAG, "task create failed");
  }
  enabledEvent.Generate();
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::Disable() {
  LockGuard lg(*this);
  if (taskHandle == xTaskGetCurrentTaskHandle()) {
    enableFromRequest = false;
    disableFromRequest = true;
    return ESP_OK;
  }
  if (!taskHandle)
    return ESP_OK;
  
  while (taskHandle) {
    disable = true;
    vTaskDelay(1);
  }
  disabledEvent.Generate();
  return ESP_OK;
}

//==============================================================================

bool UartServer::IsEnabled() {
  LockGuard lg(*this);
  return taskHandle;
}

//==============================================================================

std::shared_ptr<Uart> UartServer::GetUart() {
  LockGuard lg(*this);
  return uart;
}

//==============================================================================

esp_err_t UartServer::SetUart(std::shared_ptr<Uart> uart) {
  LockGuard lg(*this);
  this->uart = uart;
  ESP_RETURN_ON_ERROR(RestartIfEnabled(), TAG, "restart failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartServer::SetTaskParameters(const TaskParameters& taskParameters) {
  LockGuard lg(*this);
  this->taskParameters = taskParameters;
  ESP_RETURN_ON_ERROR(RestartIfEnabled(), TAG, "restart failed");
  return ESP_OK;
}

//==============================================================================

void UartServer::TaskCode(void* parameters) {
  UartServer& server = *(UartServer*)parameters;
  bool firstRun = true;

  while (!server.disable) {
    if (server.Lock(0) == ESP_OK) {
      Uart& uart = *server.GetUart();

      if (firstRun) {
        LockGuard lg(uart);
        firstRun = false;
        uart.Read(NULL, uart.GetReadableSize());
      }
      else {
        LockGuard lg(uart);
        if (uart.GetReadableSize())
          server.HandleRequest(uart);
      }

      if (server.enableFromRequest)
        server.disableFromRequest = false;
      server.enableFromRequest = false;

      if (server.disableFromRequest) {
        server.disableFromRequest = false;
        server.disabledEvent.Generate();
        server.taskHandle = NULL;
        server.Unlock();
        vTaskDelete(NULL);
        return;
      }

      server.Unlock();
    }
    vTaskDelay(1);
  }

  server.taskHandle = NULL;
  vTaskDelete(NULL);
}

//==============================================================================

esp_err_t UartServer::RestartIfEnabled() {
  if (!taskHandle || disableFromRequest)
    return ESP_OK;
  ESP_RETURN_ON_ERROR(Disable(), TAG, "disable failed");
  ESP_RETURN_ON_ERROR(Enable(), TAG, "enable failed");
  return ESP_OK;
}

//==============================================================================

}