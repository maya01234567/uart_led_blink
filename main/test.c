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
#include "min.h"

#define LED_PIN 2

struct min_context min_ctx;
// static int value = 100;
struct min_context min_ctx;
void min_application_handler(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload, uint8_t port)
{
    ESP_LOGI("UART1", "DONE");
}
uint32_t min_time_ms(void)
{
    return xTaskGetTickCount();
}
uint16_t min_tx_space(uint8_t port) // phai lon hon 11 khoang trong du lieu
{
    return 512;
}
void min_tx_byte(uint8_t port, uint8_t byte)
{
    uart_send_data(UART_NUM_2, (const char *)&byte, 1);
}
void min_tx_start(uint8_t port) {}
void min_tx_finished(uint8_t port) {}
// void blinkLed()
// {
// int x = 0;
// while (1)
// {
// gpio_set_level(GPIO_NUM_2, x);
// vTaskDelay(value / portTICK_PERIOD_MS);
// gpio_set_level(GPIO_NUM_2, 1-x);
// vTaskDelay(value / portTICK_PERIOD_MS);
// }
// }
//
void callback_recive_uart(uint8_t *data, size_t length)
{
    ESP_LOGI("UART", "DONE");
    if (length > 0)
    {
        for (uint8_t i = 0; i < length; i++)
        {
            printf("0x%x  ", data[i]);
        }
        bzero(data, 1024);
    }
}

void app_main()
{
    uint8_t buff[] = {0x01, 0x02, 0x03, 0x04};
    uart_io_creat(UART_NUM_2, 115200, 17, 16);
    output_io_creat(2);
    min_init_context(&min_ctx, 0);
    // xTaskCreate(blinkLed, "blinkLed", 2048, NULL, 4, NULL);
    //  uart_enable_pattern_det_baud_intr(UART_NUM_2, '+', 3, 9, 0, 0);
    //  uart_pattern_queue_reset(UART_NUM_2, 20);
    uart_set_callback(callback_recive_uart);
    // while (1)
    // {
        // min_send_frame(&min_ctx, 0, buff, sizeof(buff));
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}
