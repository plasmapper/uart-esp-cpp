UART Component
==============

.. |COMPONENT| replace:: uart

.. |ESP_IDF_VERSION| replace:: 5.0
   
.. |VERSION| replace:: 1.1.0

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
2. :cpp:class:`PL::StreamServer` can be used with :cpp:class:`PL::Uart` to implement a stream server for ESP internal UART ports. The descendant class should override
   :cpp:func:`PL::StreamServer::HandleRequest` to handle the client request. :cpp:func:`PL::StreamServer::HandleRequest` is only called when there is incoming data in the internal buffer.

Thread safety
-------------

Class method thread safety is implemented by having the :cpp:class:`PL::Lockable` as a base class and creating the class object lock guard at the beginning of the methods.

Examples
--------
| `UART <https://components.espressif.com/components/plasmapper/pl_uart/versions/1.1.0/examples/uart>`_
| `UART echo server <https://components.espressif.com/components/plasmapper/pl_uart/versions/1.1.0/examples/uart_echo_server>`_

API reference
-------------

.. toctree::
  
  api/types      
  api/uart