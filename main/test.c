#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "uart_iot.h"
#include "output_iot.h"

#define LED_PIN 2
static int value = 100;

void blinkLed()
{
    int x = 0;
    while (1)
    {
        gpio_set_level(GPIO_NUM_2, x);
        vTaskDelay(value / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_2, 1-x);
        vTaskDelay(value / portTICK_PERIOD_MS);
    }
}

void callback_uart(uint8_t *data, size_t length)
{
    char data1[1000];
    printf("data1 : %s\n", data);
    printf("data2 : %d\n", length);
    for (int i = 0; i < length; i++)
    {
        data1[i] = (char)*(data + i);
    }
    // value = atoi(data);
    printf("data123: %s\n", data1);
    value = atoi(data1);
    bzero(data1, length);
    printf("data1235: %d\n", value);
}

void app_main()
{
    uart_io_creat(UART_NUM_0, 115200, 1, 3);
    output_io_creat(2);
    xTaskCreate(blinkLed, "blinkLed", 2048, NULL, 4, NULL);
    uart_enable_pattern_det_baud_intr(UART_NUM_0, '+', 3, 9, 0, 0);
    uart_pattern_queue_reset(UART_NUM_0, 20);
    uart_set_callback(callback_uart);
}
