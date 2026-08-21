# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2026-08-21
### Removed
- Deprecated UartServer class.

### Fixed
- uart_echo_server example and test server not stopping the server task before destruction.
- Uart::SetMode not storing the mode.
- Uart::SetBaudRate configuring interrupts before the driver is installed.
- Uart dataBits member declared as int16_t instead of uint16_t.
- uart example destroying the UART when app_main returns.

## [1.3.1] - 2026-08-18
### Fixed
- RX FIFO interrupt configuration.

## [1.3.0] - 2026-08-14
### Added
- Write operation timeout getter and setter non-implemented stubs (since ESP-IDF uart_write_bytes does not support timeout).

### Fixed
- Buffer size rounding off-by-one when the requested size is already a multiple of 4.
- GetStopBits documentation not attaching to the declaration due to a blank line.
- Discard read performance when discarding data one byte at a time.

## [1.2.0] - 2026-08-07
### Added
- ESP-IDF v6.0 support.

## [1.1.1] - 2026-08-05
### Changed
- Lock timeout handling.
- Remaining static const members to constexpr.
- Buffer size rounding.

### Fixed
- Read timeout compounding when discarding data.
- UartServer UART reference not tracking stream set directly through SetStream.
- Write not verifying that all bytes were written.
- Read size underflow on read error.
- Declaration and usage of minBufferSize.
- Undocumented txBufferSize zero value.

## [1.1.0] - 2024-08-29
### Changed
- Marked UartServer class as deprecated (StreamServer should be used instead).

## [1.0.1] - 2024-06-12
### Added
- Copying examples to component folder on upload.

## [1.0.0] - 2024-06-12
Initial release.