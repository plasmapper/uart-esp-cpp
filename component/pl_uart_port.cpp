#include "pl_uart_port.h"
#include "esp_check.h"
#include <map>

//==============================================================================

static const char* TAG = "pl_uart_port";

//==============================================================================

namespace PL {

//==============================================================================

static std::map<uint16_t, uart_word_length_t> dataBitsMap {
  {5, UART_DATA_5_BITS}, {6, UART_DATA_6_BITS}, {7, UART_DATA_7_BITS}, {8, UART_DATA_8_BITS}};

static std::map<UartParity, uart_parity_t> parityMap {
  {UartParity::none, UART_PARITY_DISABLE}, {UartParity::even, UART_PARITY_EVEN}, {UartParity::odd, UART_PARITY_ODD}};

static std::map<UartStopBits, uart_stop_bits_t> stopBitsMap {
  {UartStopBits::one, UART_STOP_BITS_1}, {UartStopBits::onePointFive, UART_STOP_BITS_1_5}, {UartStopBits::two, UART_STOP_BITS_2}};

static std::map<UartFlowControl, uart_hw_flowcontrol_t> flowControlMap {
  {UartFlowControl::none, UART_HW_FLOWCTRL_DISABLE}, {UartFlowControl::rts, UART_HW_FLOWCTRL_RTS},
  {UartFlowControl::cts, UART_HW_FLOWCTRL_CTS}, {UartFlowControl::rtsCts, UART_HW_FLOWCTRL_CTS_RTS}};

//==============================================================================

const std::string UartPort::defaultName = "UART";

//==============================================================================

UartPort::UartPort (uart_port_t port, int rxBufferSize, int txBufferSize, int txPin, int rxPin, int rtsPin, int ctsPin) :
                    port (port), rxBufferSize (rxBufferSize), txBufferSize (txBufferSize), txPin (txPin), rxPin (rxPin), rtsPin (rtsPin), ctsPin (ctsPin) {
  SetName(defaultName + std::to_string (port - UART_NUM_0));
}

//==============================================================================

UartPort::~UartPort() {
  if (uart_is_driver_installed (port))
    uart_driver_delete (port);
}

//==============================================================================

esp_err_t UartPort::Lock (TickType_t timeout) {
  esp_err_t error = mutex.Lock (timeout);
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR (error, TAG, "lock uart port failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::Unlock() {
  ESP_RETURN_ON_ERROR (mutex.Unlock(), TAG, "unlock uart port failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::Initialize() {
  LockGuard lg (*this);
  if (uart_is_driver_installed (port))
    return ESP_OK;
  ESP_RETURN_ON_ERROR (SetConfiguration(), TAG, "uart port set configuration failed");
  ESP_RETURN_ON_ERROR (uart_set_pin (port, txPin, rxPin, rtsPin, ctsPin), TAG, "uart port set pins failed");
  ESP_RETURN_ON_ERROR (uart_driver_install (port, rxBufferSize, txBufferSize, 0, NULL, 0), TAG, "uart port driver install failed");
  ESP_RETURN_ON_ERROR (uart_set_mode (port, mode), TAG, "uart port set mode failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::Enable() {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (uart_is_driver_installed (port), ESP_ERR_INVALID_STATE, TAG, "uart port is not initialized");
  if (enabled)
    return ESP_OK;
  enabled = true; 
  Read (NULL, GetReadableSize());
  enabledEvent.Generate();
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::Disable() {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (uart_is_driver_installed (port), ESP_ERR_INVALID_STATE, TAG, "uart port is not initialized");
  if (!enabled)
    return ESP_OK;
  enabled = false;
  disabledEvent.Generate();
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::EnableLoopback() {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (uart_is_driver_installed (port), ESP_ERR_INVALID_STATE, TAG, "uart port is not initialized");
  ESP_RETURN_ON_ERROR (uart_set_loop_back (port, true), TAG, "uart port enable loopback failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::DisableLoopback() {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (uart_is_driver_installed (port), ESP_ERR_INVALID_STATE, TAG, "uart port is not initialized");
  ESP_RETURN_ON_ERROR (uart_set_loop_back (port, false), TAG, "uart port disable loopback failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::Read (void* dest, size_t size) {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (enabled, ESP_ERR_INVALID_STATE, TAG, "uart port is not enabled");
  if (!size)
    return ESP_OK;
  
  uint32_t baudrate;
  TickType_t totalTimeout = readTimeout;
  ESP_RETURN_ON_ERROR (uart_get_baudrate (port, &baudrate), TAG, "uart port get baudrate failed");
  if (baudrate)
    totalTimeout += (size * 1000 * 11) / baudrate / portTICK_PERIOD_MS;

  int res;
  if (dest)
    size -= (res = uart_read_bytes (port, dest, size, totalTimeout));
  else {
    uint8_t data;
    for (; size && (res = uart_read_bytes (port, &data, 1, totalTimeout)) == 1; size--);
  }
  ESP_RETURN_ON_FALSE (res >= 0, ESP_FAIL, TAG, "uart port read bytes failed");
  ESP_RETURN_ON_FALSE (size == 0, ESP_ERR_TIMEOUT, TAG, "uart port read timeout");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::Write (const void* src, size_t size) {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (enabled, ESP_ERR_INVALID_STATE, TAG, "uart port is not enabled");
  if (!size)
    return ESP_OK;
  ESP_RETURN_ON_FALSE (src, ESP_ERR_INVALID_ARG, TAG, "src is null");
  ESP_RETURN_ON_FALSE (uart_write_bytes (port, src, size) > 0, ESP_FAIL, TAG, "uart port write bytes failed");
  return ESP_OK;
}

//==============================================================================

bool UartPort::IsEnabled() {
  LockGuard lg (*this);
  return enabled;
}

//==============================================================================

size_t UartPort::GetReadableSize() {
  LockGuard lg (*this);
  if (!enabled)
    return 0;
  size_t size = 0;
  return uart_get_buffered_data_len (port, &size) == ESP_OK ? size : 0;
}

//==============================================================================

TickType_t UartPort::GetReadTimeout() {
  LockGuard lg (*this);
  return readTimeout;
}

//==============================================================================

esp_err_t UartPort::SetReadTimeout (TickType_t timeout) {
  LockGuard lg (*this);
  readTimeout = timeout;
  return ESP_OK;
}

//==============================================================================

uint32_t UartPort::GetBaudRate() {
  LockGuard lg (*this);
  return baudRate;
}

//==============================================================================

esp_err_t UartPort::SetBaudRate (uint32_t baudRate) {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (baudRate, ESP_ERR_INVALID_ARG, TAG, "invalid baud rate (%d)", baudRate);
  this->baudRate = baudRate;
  ESP_RETURN_ON_ERROR (SetConfiguration(), TAG, "uart port set configuration failed");
  return ESP_OK;
}

//==============================================================================

uint16_t UartPort::GetDataBits() {
  LockGuard lg (*this);
  return dataBits;
}

//==============================================================================

esp_err_t UartPort::SetDataBits (uint16_t dataBits) {
  LockGuard lg (*this);
  auto iterator = dataBitsMap.find (dataBits);
  ESP_RETURN_ON_FALSE (iterator != dataBitsMap.end(), ESP_ERR_INVALID_ARG, TAG, "invalid data bits (%d)", dataBits);
  this->dataBits = dataBits;
  ESP_RETURN_ON_ERROR (SetConfiguration(), TAG, "uart port set configuration failed");
  return ESP_OK;
}
  
//==============================================================================

UartParity UartPort::GetParity() {
  LockGuard lg (*this);
  return parity;
}

//==============================================================================

esp_err_t UartPort::SetParity (UartParity parity) {
  LockGuard lg (*this);
  auto iterator = parityMap.find (parity);
  ESP_RETURN_ON_FALSE (iterator != parityMap.end(), ESP_ERR_INVALID_ARG, TAG, "invalid parity (%d)", (int)parity);
  this->parity = parity;
  ESP_RETURN_ON_ERROR (SetConfiguration(), TAG, "uart port set configuration failed");
  return ESP_OK;
}

//==============================================================================

UartStopBits UartPort::GetStopBits() {
  LockGuard lg (*this);
  return stopBits;
}

//==============================================================================

esp_err_t UartPort::SetStopBits (UartStopBits stopBits) {
  LockGuard lg (*this);
  auto iterator = stopBitsMap.find (stopBits);
  ESP_RETURN_ON_FALSE (iterator != stopBitsMap.end(), ESP_ERR_INVALID_ARG, TAG, "invalid stop bits (%d)", (int)stopBits);
  this->stopBits = stopBits;
  ESP_RETURN_ON_ERROR (SetConfiguration(), TAG, "uart port set configuration failed");
  return ESP_OK;
}

//==============================================================================
 
UartFlowControl UartPort::GetFlowControl() {
  LockGuard lg (*this);
  return flowControl;
}

//==============================================================================

esp_err_t UartPort::SetFlowControl (UartFlowControl flowControl) {
  LockGuard lg (*this);
  auto iterator = flowControlMap.find (flowControl);
  ESP_RETURN_ON_FALSE (iterator != flowControlMap.end(), ESP_ERR_INVALID_ARG, TAG, "invalid flow control (%d)", (int)flowControl);
  this->flowControl = flowControl;
  ESP_RETURN_ON_ERROR (SetConfiguration(), TAG, "uart port set configuration failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::SetMode (uart_mode_t mode) {
  LockGuard lg (*this);
  ESP_RETURN_ON_FALSE (uart_is_driver_installed (port), ESP_ERR_INVALID_STATE, TAG, "uart port is not initialized");
  ESP_RETURN_ON_ERROR (uart_set_mode (port, mode), TAG, "uart set mode failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t UartPort::SetConfiguration() {
  LockGuard lg (*this);
  uart_config_t config = {};
  config.baud_rate = baudRate;
  config.data_bits = dataBitsMap.find (dataBits)->second;
  config.parity = parityMap.find (parity)->second;
  config.stop_bits = stopBitsMap.find (stopBits)->second;
  config.flow_ctrl = flowControlMap.find (flowControl)->second;
  ESP_RETURN_ON_ERROR (uart_param_config (port, &config), TAG, "uart set parameters failed");
  return ESP_OK;
}

//==============================================================================

}