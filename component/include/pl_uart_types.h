#pragma once
#include "stdint.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief UART parity
enum class UartParity : uint8_t {
  /// @brief no parity bit
  none = 0,
  /// @brief even parity bit
  even = 1,
  /// @brief odd parity bit
  odd = 2
};

/// @brief Number of UART stop bits
enum class UartStopBits : uint8_t {
  /// @brief 1 stop bit
  one = 0,
  /// @brief 1.5 stop bits
  onePointFive = 1,
  /// @brief 2 stop bits
  two = 2
};

/// @brief UART flow control type
enum class UartFlowControl : uint8_t {
  /// @brief no flow control
  none = 0,
  /// @brief RTS flow control
  rts = 1,
  /// @brief CTS flow control
  cts = 2,
  /// @brief RTS and CTS flow control
  rtsCts = 3
};

//==============================================================================

}