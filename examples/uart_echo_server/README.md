# UART Echo Server Example

1. UartEchoServer class inherits UartServer class and overrides HandleRequest method to implement echo functionality.
2. UART0 is initialized.
3. UART0 parameters are set to 115200 bps, 8 data bits, no parity, 1 stop bit and no flow control.
4. UART0 is enabled.
5. UartEchoServer is created with UART0 as a port.
6. UartEchoServer is enabled.
