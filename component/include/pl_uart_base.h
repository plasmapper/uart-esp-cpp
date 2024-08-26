#pragma once
#include "pl_common.h"
#include "pl_uart_types.h"
#include "driver/uart.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief UART class
class Uart : public HardwareInterface, public Stream {
public:
  /// @brief Default hardware interface name
  static const std::string defaultName;
  /// @brief Minimum RX and TX buffer size
  static const int minBufferSize = SOC_UART_FIFO_LEN + 4;
  /// @brief Default operation timeout in FreeRTOS ticks
  static const TickType_t defaultReadTimeout = 300 / portTICK_PERIOD_MS;
  /// @brief Default baud rate
  static const uint32_t defaultBaudRate = 115200;
  /// @brief Default number of data bits
  static const uint16_t defaultDataBits = 8;
  /// @brief Default parity
  static const UartParity defaultParity = UartParity::none;
  /// @brief Default number of stop bits
  static const UartStopBits defaultStopBits = UartStopBits::one;
  /// @brief Default flow control type
  static const UartFlowControl defaultFlowControl = UartFlowControl::none;
  /// @brief Default mode
  static const uart_mode_t defaultMode = UART_MODE_UART;

  /// @brief Creates an UART
  /// @param port port number
  /// @param rxBufferSize RX buffer size
  /// @param txBufferSize TX buffer size
  /// @param txPin TX pin
  /// @param rxPin RX pin
  /// @param rtsPin RTS pin
  /// @param ctsPin CTS pin
  Uart(uart_port_t port, int rxBufferSize = minBufferSize, int txBufferSize = minBufferSize,
            int txPin = UART_PIN_NO_CHANGE, int rxPin = UART_PIN_NO_CHANGE, int rtsPin = UART_PIN_NO_CHANGE, int ctsPin = UART_PIN_NO_CHANGE);
  ~Uart();
  Uart(const Uart&) = delete;
  Uart& operator=(const Uart&) = delete;
  
  esp_err_t Lock(TickType_t timeout = portMAX_DELAY) override;
  esp_err_t Unlock() override;

  esp_err_t Initialize() override;
  esp_err_t Enable() override;
  esp_err_t Disable() override;

  /// @brief Enables the loopback
  /// @return error code
  esp_err_t EnableLoopback();

  /// @brief Disables the loopback
  /// @return error code
  esp_err_t DisableLoopback();

  esp_err_t Read(void* dest, size_t size) override;
  esp_err_t Write(const void* src, size_t size) override;

  bool IsEnabled() override;
  
  size_t GetReadableSize() override;

  TickType_t GetReadTimeout() override;
  esp_err_t SetReadTimeout(TickType_t timeout) override;

  /// @brief Gets the baud rate
  /// @return baud rate
  uint32_t GetBaudRate();

  /// @brief Sets the baud rate
  /// @param baudRate baud rate
  /// @return error code
  virtual esp_err_t SetBaudRate(uint32_t baudRate);

  /// @brief Gets the number of data bits
  /// @return number of data bits
  uint16_t GetDataBits();

  /// @brief Sets the number of data bits
  /// @param dataBits number of data bits
  /// @return error code
  virtual esp_err_t SetDataBits(uint16_t dataBits);

  /// @brief Gets the parity
  /// @return parity
  UartParity GetParity();

  /// @brief Sets the parity
  /// @param parity parity
  /// @return error code
  virtual esp_err_t SetParity(UartParity parity);

  /// @brief Gets the number of stop bits
  /// @return number of stop bits
  
  UartStopBits GetStopBits();
  /// @brief Set the number of stop bits
  /// @param stopBits number of stop bits
  /// @return error code
  virtual esp_err_t SetStopBits(UartStopBits stopBits);
 
  /// @brief Gets the flow control type
  /// @return flow control type
  UartFlowControl GetFlowControl();

  /// @brief Sets the flow control type
  /// @param flowControl flow control type
  /// @return error code
  virtual esp_err_t SetFlowControl(UartFlowControl flowControl);

  /// @brief Sets the mode (UART/IRDA/RS485...)
  /// @param mode mode
  /// @return error code
  esp_err_t SetMode(uart_mode_t mode);

private:
  Mutex mutex;
  uart_port_t port;
  bool loopbackEnabled = false;
  int rxBufferSize, txBufferSize;
  int txPin, rxPin, rtsPin, ctsPin;
  bool enabled = false;
  TickType_t readTimeout = defaultReadTimeout;
  uint32_t baudRate = defaultBaudRate;
  int16_t dataBits = defaultDataBits;
  UartParity parity = defaultParity;
  UartStopBits stopBits = defaultStopBits;
  UartFlowControl flowControl = defaultFlowControl;
  uart_mode_t mode = defaultMode;

  esp_err_t SetConfiguration();
};

//==============================================================================

}