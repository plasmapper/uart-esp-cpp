cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "pl_uart_base.cpp" "pl_uart_server.cpp" INCLUDE_DIRS "include" REQUIRES "driver" "pl_common")