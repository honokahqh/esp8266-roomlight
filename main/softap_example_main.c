/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "user_gpio.h"
#include "user_mqtt.h"
#include "user_nvs.h"
#include "user_ota.h"
#include "user_pwm.h"
#include "user_softap.h"
#include "user_wifi.h"
#include <string.h>
#define TAG "main"

uint32_t io_pins[] = {12, 13, 14};

void obtain_time(void) {
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET &&
           ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry,
                 retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    time(&now);
    localtime_r(&now, &timeinfo);

    // 设置时区为东八区
    setenv("TZ", "CST-8", 1);
    tzset();

    // 更新本地时间
    localtime_r(&now, &timeinfo);

    // Print the time to check if it's correct
    ESP_LOGI(TAG, "Current date/time: %s", asctime(&timeinfo));
}

void pwm_update_task(void *pvParameters) {
    static int lightness, period, wifi_retry = 5;
    while (1) {
        switch (dev_state) {
        case DEV_SOFTAP:
            set_rgb_color(0xf800);
            vTaskDelay(pdMS_TO_TICKS(700));
            set_rgb_color(0x0000);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        case DEV_STA_CONNECTING:
            set_rgb_color(0x07e0);
            vTaskDelay(pdMS_TO_TICKS(700));
            set_rgb_color(0x0000);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        case DEV_MQTT_CONNECTING:
            set_rgb_color(0x001f);
            vTaskDelay(pdMS_TO_TICKS(700));
            set_rgb_color(0x0000);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        case DEV_MQTT_CONNECTED:
            period = atoi(nvs_data.lightPeriod);
            if (period >= 500) {
                lightness = atoi(nvs_data.lightSwitch1);
                set_rgb_color(lightness);
                vTaskDelay(pdMS_TO_TICKS(period)); // 延时 period 毫秒
                lightness = atoi(nvs_data.lightSwitch2);
                set_rgb_color(lightness);
                vTaskDelay(pdMS_TO_TICKS(period)); // 延时 period 毫秒
            } else {
                lightness = atoi(nvs_data.lightNormal);
                set_rgb_color(lightness);
                vTaskDelay(pdMS_TO_TICKS(500)); // 延时 500 毫秒
            }
            break;
        default:
            break;
        }
    }
}

void app_main() {
    ESP_LOGI(TAG, "this is app!!!!!!!!!!!!!!");
    size_t flash_size = spi_flash_get_chip_size();
    printf("Flash size: %d bytes\n", flash_size);

    gpio_init();
    uint32_t duty_cycle[] = {0, 0, 0};
    init_pwm(io_pins, 3, 1000, duty_cycle);
    xTaskCreate(pwm_update_task, "PWM Update Task", 2048, NULL, 10, NULL);
    init_nvs();
    nvs_read_data_from_flash();
    
    // compare default values
    if (strcmp(nvs_data.ssid, "default") == 0 ||
        strcmp(nvs_data.pass, "default") == 0 ||
        strcmp(nvs_data.roomID, "default") == 0 ||
        strcmp(nvs_data.userID, "default") == 0) {
        ESP_LOGI(TAG, "no enough data, init as softap\n");    
        wifi_init_softap();
    } else {
        dev_state = DEV_STA_CONNECTING;
        ESP_LOGI(TAG, "wifi is not default value, init as station\n");
        if (wifi_init_sta(nvs_data.ssid, nvs_data.pass)) {
            dev_state = DEV_MQTT_CONNECTING;
            ESP_LOGI(TAG, "Connected to WiFi successfully.Init mqtt");
            // ota_init();
            // obtain_time();
            mqtt_app_start();
        } else {
            ESP_LOGI(TAG, "Failed to connect to WiFi\n");
        }
    }
}
