#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include "input_iot.h"

input_callback_t input_callback = NULL;

static void IRAM_ATTR gpio_input_handler(void* arg) {
    int gpio_num=(uint32_t) arg;
    input_callback(gpio_num);//khi xảy ra sự kiện ngắt gọi truyền tham số cho hàm ở main
}
 
void input_io_creat(gpio_num_t gpio_num, interrupt_type_edge_t type)
{
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num,GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num,GPIO_PULLDOWN_ONLY);
    gpio_set_intr_type(gpio_num,type);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num,gpio_input_handler,(void*)gpio_num);//phát hiejn sự kiện ngắt và truyền cho hàm xử lý

}

void input_set_callback(void *cb)
{
input_callback = cb;//gán địa chỉ của hàm cho con trỏ input_callback
}