UART Component
==============

.. |COMPONENT| replace:: uart

.. include:: ../../../installation.rst

.. include:: ../../../sdkconfig_common.rst

Features
--------

1. :cpp:class:`PL::UartPort` - a :cpp:class:`PL::HardwareInterface` and :cpp:class:`PL::Stream` implementation for ESP internal UART ports.
   :cpp:func:`PL::UartPort::Initialize` installs the UART port driver. :cpp:func:`PL::UartPort::Enable` enables the port (discarding the incoming data).
   A number of Get And Set methods get and set UART port parameters.
   A number of :cpp:func:`PL::UartPort::Read` and :cpp:func:`PL::UartPort::Write` functions implement reading and writing to/from memory and
   :cpp:class:`PL::Buffer` objects. Reading and writing to/from :cpp:class:`PL::Buffer` object checks the data size and locks the object so these methods can
   be used in multithreaded applications. 
2. :cpp:class:`PL::UartServer` - a :cpp:class:`PL::Server` implementation for ESP internal UART ports. The descendant class should override
   :cpp:func:`PL::UartServer::HandleRequest` to handle the client request. :cpp:func:`PL::UartServer::HandleRequest` is only called when the UART port is enabled
   and there is incoming data in the internal buffer.

Examples
--------
| `UART port <https://github.com/plasmapper/uart-esp-cpp/tree/main/examples/port>`_
| `UART echo server <https://github.com/plasmapper/uart-esp-cpp/tree/main/examples/echo_server>`_

API reference
-------------

.. toctree::
  
  api/types      
  api/uart_port
  api/uart_server