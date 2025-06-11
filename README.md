### ESP RS485
This component provides a very simple way to use RS485 communication with an ESP device. It is a wrapper around the built-in uart driver in ESP-IDF. It currently only does not have non-blocking mode support.

### Features
- Simple API for sending and receiving data over RS485.
- Supports seperate transmit and receive pins.
- Supports low-power shutdown modes common in most RS485 transceivers.
  
### Usage
Recieve data:
```c
#include "RS485.h"
#include <stdio.h>
#include "driver/gpio.h"

#define UART_PORT   UART_NUM_1
#define TX_PIN      GPIO_NUM_17
#define RX_PIN      GPIO_NUM_16
#define DE_PIN      GPIO_NUM_18
#define RE_PIN      GPIO_NUM_19
#define BAUD_RATE   9600

rs485_uart_t uart_dev = {
    .uart_port = UART_NUM_1,
    .tx_pin = GPIO_NUM_17,
    .rx_pin = GPIO_NUM_16,
    .de_pin = GPIO_NUM_18,
    .re_pin = GPIO_NUM_19,
    .baud_rate = 9600
};

void app_main(void)
{
    if (rs485_uart_init(&uart_dev) != ESP_OK) {
    printf("Failed to initialise RS485\n");
    return;
    }

    while(true) {
        if(rs482_uart_available(&uart_dev)) {
            printf("Received: %s\n", rs485_uart_read(&uart_dev));
    }
    
}
```