#include <stdio.h>
#include "RS485.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

esp_err_t rs485_uart_init(rs485_uart_t *dev) {
    const uart_config_t uart_config = {
        .baud_rate = dev->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    esp_err_t err;

    // Configure UART
    err = uart_param_config(dev->uart_port, &uart_config);
    if (err != ESP_OK) return err;

    // Assign TX/RX pins (leave RTS/CTS unchanged)
    err = uart_set_pin(dev->uart_port, dev->tx_pin, dev->rx_pin,
                       UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) return err;

    // Install UART driver with RX and TX buffer sizes
    err = uart_driver_install(dev->uart_port, 256, 0, 0, NULL, 0);
    if (err != ESP_OK) return err;

    // Configure DE/RE pins
    gpio_set_direction(dev->de_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(dev->re_pin, GPIO_MODE_OUTPUT);

    // Put RS485 into receive mode by default
    gpio_set_level(dev->de_pin, 0);
    gpio_set_level(dev->re_pin, 0);

    return ESP_OK;
}

void rs485_set_transmit(rs485_uart_t *dev) {
    gpio_set_level(dev->re_pin, 1);
    gpio_set_level(dev->de_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
}

void rs485_set_receive(rs485_uart_t *dev) {
    gpio_set_level(dev->de_pin, 0);
    gpio_set_level(dev->re_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
}

esp_err_t rs485_uart_read_buffer(rs485_uart_t *dev, uint8_t *data, size_t len, TickType_t timeout) {
    rs485_set_receive(dev);
    int read_bytes = uart_read_bytes(dev->uart_port, data, len, timeout);
    if (read_bytes < 0) {
        return ESP_FAIL;
    }
    return ESP_OK;

esp_err_t rs485_uart_write(rs485_uart_t *dev, const uint8_t *data, size_t len) {
    rs485_set_transmit(dev);
    uart_write_bytes(dev->uart_port, (const char*)data, len);
    uart_wait_tx_done(dev->uart_port, pdMS_TO_TICKS(100));
    rs485_set_receive(dev);
    return ESP_OK;
}

void rs485_enter_shutdown(rs485_uart_t *dev) {
    gpio_set_level(dev->re_pin, 1);
    gpio_set_level(dev->de_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
}

void rs485_exit_shutdown(rs485_uart_t *dev) {
    gpio_set_level(dev->re_pin, 0);
    gpio_set_level(dev->de_pin, 0);
}

int rs485_available(rs485_uart_t *dev) {
    size_t available;
    uart_get_buffered_data_len(dev->uart_port, &available);
    return available > 0 ? 1 : 0; // Return 1 if data is available, else 0
}