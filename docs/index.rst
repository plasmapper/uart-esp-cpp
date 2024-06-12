UART Component
==============

.. |COMPONENT| replace:: uart

.. |VERSION| replace:: 1.0.1

.. include:: ../../../installation.rst

.. include:: ../../../sdkconfig_common.rst

Features
--------

1. :cpp:class:`PL::Uart` - a :cpp:class:`PL::HardwareInterface` and :cpp:class:`PL::Stream` implementation for ESP internal UART ports.
   :cpp:func:`PL::Uart::Initialize` installs the UART driver. :cpp:func:`PL::Uart::Enable` enables the port (discarding the incoming data).
   A number of Get And Set methods get and set UART port parameters.
   A number of :cpp:func:`PL::Uart::Read` and :cpp:func:`PL::Uart::Write` functions read and write from/to the port.
   Reading and writing to/from :cpp:class:`PL::Buffer` object checks the data size and locks the object so these methods can
   be used in multithreaded applications. 
2. :cpp:class:`PL::UartServer` - a :cpp:class:`PL::Server` implementation for ESP internal UART ports. The descendant class should override
   :cpp:func:`PL::UartServer::HandleRequest` to handle the client request. :cpp:func:`PL::UartServer::HandleRequest` is only called when the UART port is enabled
   and there is incoming data in the internal buffer.

Examples
--------
| `UART <https://components.espressif.com/components/plasmapper/pl_uart/versions/1.0.1/examples/uart>`_
| `UART echo server <https://components.espressif.com/components/plasmapper/pl_uart/versions/1.0.1/examples/uart_echo_server>`_

API reference
-------------

.. toctree::
  
  api/types      
  api/uart
  api/uart_server