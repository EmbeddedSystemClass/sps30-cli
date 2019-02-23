cmake_minimum_required(VERSION 3.13)

project(sensirion_sps30 C)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/sps_git_version.c.in" 
  "${CMAKE_CURRENT_BINARY_DIR}/sps_git_version.c" @ONLY)

add_library(sps30_uart "")
target_sources(sps30_uart
  PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}/embedded-uart-sps/sps30-uart/sps30.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/embedded-uart-sps/embedded-uart-common/sensirion_uart.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/sensirion_uart_port.h
  PRIVATE
    embedded-uart-sps/embedded-uart-common/sensirion_shdlc.c
#    "${CMAKE_CURRENT_SOURCE_DIR}/embedded-uart-sps/embedded-uart-common/sensirion_shdlc.h"
#    ${CMAKE_CURRENT_SOURCE_DIR}/embedded-uart-sps/embedded-uart-common/sensirion_arch_config.h
    embedded-uart-sps/sps30-uart/sps30.c
    sensirion_uart_implementation.c
    "${CMAKE_CURRENT_BINARY_DIR}/sps_git_version.c"
)
target_include_directories(sps30_uart
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/embedded-uart-sps/sps30-uart/
    ${CMAKE_CURRENT_SOURCE_DIR}/embedded-uart-sps/embedded-uart-common/
)


