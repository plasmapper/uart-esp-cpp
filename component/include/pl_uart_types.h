#pragma once

//==============================================================================

namespace PL {

//==============================================================================

/// @brief UART parity
enum class UartParity {
  /// @brief no parity bit
  none,
  /// @brief even parity bit
  even,
  /// @brief odd parity bit
  odd
};

/// @brief Number of UART stop bits
enum class UartStopBits {
  /// @brief 1 stop bit
  one,
  /// @brief 1.5 stop bits
  onePointFive,
  /// @brief 2 stop bits
  two
};

/// @brief UART flow control type
enum class UartFlowControl {
  /// @brief no flow control
  none,
  /// @brief RTS flow control
  rts,
  /// @brief CTS flow control
  cts,
  /// @brief RTS and CTS flow control
  rtsCts
};

//==============================================================================

}