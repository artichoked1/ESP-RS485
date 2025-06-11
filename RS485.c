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
    gpio_set_level(dev->re_pin, 1);  // RE = 0 (enable receiver but not needed here)
    gpio_set_level(dev->de_pin, 1);  // DE = 1 (enable driver)
    vTaskDelay(pdMS_TO_TICKS(1)); // Allow time for driver to enable
}

void rs485_set_receive(rs485_uart_t *dev) {
    gpio_set_level(dev->de_pin, 0);  // DE = 0 (disable driver)
    gpio_set_level(dev->re_pin, 0);  // RE = 0 (enable receiver)
    vTaskDelay(pdMS_TO_TICKS(1)); // Allow time for receiver to enable
}

esp_err_t rs485_uart_write(rs485_uart_t *dev, const uint8_t *data, size_t len) {
    rs485_set_transmit(dev);
    uart_write_bytes(dev->uart_port, (const char*)data, len);
    uart_wait_tx_done(dev->uart_port, pdMS_TO_TICKS(100));
    rs485_set_receive(dev);
    return ESP_OK;
}

void rs485_enter_shutdown(rs485_uart_t *dev) {
    gpio_set_level(dev->re_pin, 1); // Receiver disable
    gpio_set_level(dev->de_pin, 0); // Driver disable
    vTaskDelay(pdMS_TO_TICKS(1)); // Allow time for shutdown
}

void rs485_exit_shutdown(rs485_uart_t *dev) {
    gpio_set_level(dev->re_pin, 0); // Enable receiver
    gpio_set_level(dev->de_pin, 0); // Keep driver off
    // Wait tZH(SHDN)/tZL(SHDN) if needed before using
}
