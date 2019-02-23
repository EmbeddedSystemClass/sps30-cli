#ifndef SENSIRION_UART_PORT_
#define SENSIRION_UART_PORT_

#ifdef __cplusplus
extern "C" {
#endif

extern void sensirion_uart_set_dev(const char* dev);
extern const char* sensirion_uart_get_dev();

#ifdef __cplusplus
}
#endif

#endif /* SENSIRION_UART_PORT_ */
