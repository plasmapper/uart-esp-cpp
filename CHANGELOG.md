# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Changed
- Lock timeout handling.

### Fixed
- Read timeout compounding when discarding data.
- UartServer UART reference not tracking stream set directly through SetStream.
- Write not verifying that all bytes were written.
- Read size underflow on read error.
- Declaration and usage of minBufferSize.

### Changed
- Remaining static const members to constexpr.

## [1.1.0] - 2024-08-29
### Changed
- Marked UartServer class as deprecated (StreamServer should be used instead).

## [1.0.1] - 2024-06-12
### Added
- Copying examples to component folder on upload.

## [1.0.0] - 2024-06-12
Initial release.