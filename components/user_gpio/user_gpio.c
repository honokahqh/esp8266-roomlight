#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "user_nvs.h"
#include "user_mqtt.h"

#define GPIO_INPUT_IO_16 16
#define GPIO_INPUT_PIN_SEL (1ULL << GPIO_INPUT_IO_16)
#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "GPIO_Task";

#define factory_reset "ssid:default,pass:default,user:default,room:default"
void gpio_task(void *arg) {
    static int count = 0;
    while (1) {
        if (gpio_get_level(GPIO_INPUT_IO_16)) {
            count++;
            if(count == 1)
                publish_roomlight_update(MQTT_UpdateTopic, "keydown");
            if (count > 20) {
                parse_data_packet(factory_reset, strlen(factory_reset),
                                  &nvs_data);
                parse_data_packet("reboot:1", strlen("reboot:1"), &nvs_data);
            }
        } else {
            count = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 100 ms
    }
}

void gpio_init(void) {
    gpio_config_t io_conf;
    // 禁用中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // 设置为输入模式
    io_conf.mode = GPIO_MODE_INPUT;
    // 位掩码，只配置 GPIO16
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // 禁用上拉
    io_conf.pull_up_en = 1;
    // 启用下拉
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);
}
