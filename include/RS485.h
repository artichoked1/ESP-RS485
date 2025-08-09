#include "driver/uart.h"

typedef struct {
    uart_port_t uart_port;
    int tx_pin;
    int rx_pin;
    int de_pin;
    int re_pin;
    int baud_rate;
} rs485_uart_t;

esp_err_t rs485_uart_init(rs485_uart_t *dev);
esp_err_t rs485_uart_write(rs485_uart_t *dev, const uint8_t *data, size_t len);
esp_err_t rs485_uart_read_buffer(rs485_uart_t *dev, uint8_t *data, size_t max_len, TickType_t timeout);
int rs485_available(rs485_uart_t *dev);
void rs485_exit_shutdown(rs485_uart_t *dev);
void rs485_enter_shutdown(rs485_uart_t *dev);