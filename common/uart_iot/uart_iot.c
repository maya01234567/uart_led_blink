#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include "uart_iot.h"

#define PATTERN_CHR_NUM (3)

uart_callback_t uartDataReceivedCallback = NULL;
static uart_event_t event;
static QueueHandle_t uart_queue;
static uart_port_t uart_num;
static int numb = 0;

void uartEventHandlerTask(void *Parameter)
{
    uint8_t data[1024];
    size_t buffered_size;
    while (1)
    {
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY))
        {
            switch (event.type)
            {
            case UART_DATA:
                numb = 0;
            bzero(data, 1000); // xóa dữ liệu cũ tại
                numb = uart_read_bytes(uart_num, data, event.size, portMAX_DELAY);
                uartDataReceivedCallback(data, event.size);
                //   Event of HW FIFO overflow detected
                break;
            case UART_FIFO_OVF:
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(uart_num); // xóa bufer
                xQueueReset(&uart_queue);   // xóa event
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                // If buffer full happened, you should consider encreasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(uart_num);
                xQueueReset(&uart_queue);
                break;
            // Event of UART RX break detected
            case UART_BREAK:
                break;
            // Event of UART parity check error
            case UART_PARITY_ERR:
                break;
            // Event of UART frame error
            case UART_FRAME_ERR:
                break;
            // UART_PATTERN_DET
            case UART_PATTERN_DET:
                uart_get_buffered_data_len(uart_num, &buffered_size); // nhận độ dài dữ liệu trong thanh ghi ringbuff
                int pos = uart_pattern_pop_pos(uart_num);             // trả về vị trí mẫu ngắt trong ring buff
                if (pos == -1)
                { // tràn thanh ghi hàng đợi
                    // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                    // record the position. We should set a larger queue size.
                    // As an example, we directly flush the rx buffer here.
                    uart_flush_input(uart_num); // xóa dữ liệu ringbuff
                }
                else
                {
                    uart_read_bytes(uart_num, data, pos, 100 / portTICK_PERIOD_MS);
                    uint8_t pat[PATTERN_CHR_NUM + 1]; // tạo 1 mảng để xác nhận đã xóa
                    memset(pat, 0, sizeof(pat));      // sao chép 0 với số lần nhất định vào nơi con trỏ đang trỏ đến mục đích xóa bộ nhớ
                    uart_read_bytes(uart_num, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                }
                break;
            // Others
            default:
                break;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    free(data);
    vTaskDelete(NULL);
}
uint16_t uart_data_avaiable(void)
{
    return numb;
}
void uart_send_data(uart_port_t uart_num, const char *dtmp, size_t size)
{
    uart_write_bytes(uart_num, (const char *)dtmp, size);
}

void uart_io_creat(uart_port_t _uart_num, uint32_t baudrate, int tx_io_num, int rx_io_num)
{
    uart_num = _uart_num;
    uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(_uart_num, &uart_config);
    uart_driver_install(_uart_num, UART_FIFO_LEN * 2, UART_FIFO_LEN * 2, 10, &uart_queue, 0);
    uart_set_pin(_uart_num, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    xTaskCreate(uartEventHandlerTask, "uartEventHandlerTask", 2048 * 2, NULL, 4, NULL);
}
void uart_set_callback(void *cb)
{
    uartDataReceivedCallback = cb; // gán địa chỉ của hàm cho con trỏ input_callback
}