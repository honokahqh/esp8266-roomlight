#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "user_gpio.h"
#include "user_mqtt.h"
#include "user_nvs.h"
#include "user_ota.h"
#include "user_pwm.h"
#include "user_softap.h"
#include "user_test.h"
#include "user_wifi.h"

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
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
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

    char time_str[64];
    sprintf(time_str, "%s:%d", NVS_TimeStamp, (int)now);
    nvs_write_data_to_flash(time_str);
    ESP_LOGI(TAG, "Time stamp: %s", time_str);
}

void pwm_update_task(void *pvParameters) {
    static int lightness, period;
    while (1) {
        switch (dev_state) {
        case DEV_SOFTAP:
            ESP_LOGI(TAG, "Device in SoftAP mode: Flashing red");
            set_rgb_color(0xf800);
            vTaskDelay(pdMS_TO_TICKS(700));
            set_rgb_color(0x0000);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        case DEV_STA_CONNECTING:
            ESP_LOGI(TAG, "Device connecting to WiFi: Flashing green");
            set_rgb_color(0x07e0);
            vTaskDelay(pdMS_TO_TICKS(700));
            set_rgb_color(0x0000);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        case DEV_MQTT_CONNECTING:
            ESP_LOGI(TAG, "Device connecting to MQTT: Flashing blue");
            set_rgb_color(0x001f);
            vTaskDelay(pdMS_TO_TICKS(700));
            set_rgb_color(0x0000);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        case DEV_MQTT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected: Adjusting light brightness and period");
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
            ESP_LOGE(TAG, "Unhandled device state");
            break;
        }
    }
}

void netState_check_task(void *pvParameters) {
    static dev_state_t last_state = DEV_SOFTAP;
    static int timeout_count, i;
    static int mqtt_init_flag = 0;

    if (strcmp(nvs_data.ssid, "default") == 0 ||
        strcmp(nvs_data.pass, "default") == 0 ||
        strcmp(nvs_data.roomID, "default") == 0 ||
        strcmp(nvs_data.userID, "default") == 0) {
        ESP_LOGW(TAG, "Default network settings found, switching to SoftAP mode");
        wifiSwitch(WIFI_MODE_AP);
        // Delete task
        vTaskDelete(NULL);
    }

    wifiSwitch(WIFI_MODE_STA);

    while (1) {
        if (last_state != dev_state || user_softap_state.newDeviceConnect ||
            user_softap_state.newDeviceBindTCP) {
            last_state = dev_state;
            user_softap_state.newDeviceConnect = false;
            user_softap_state.newDeviceBindTCP = false;
            timeout_count = 0;
            ESP_LOGI(TAG, "State change detected, resetting timeout");
        } else {
            timeout_count++;
        }

        if (dev_state == DEV_STA_CONNECTING && user_wifi_state.getNewIP == true) {
            user_wifi_state.getNewIP = false;
            ESP_LOGI(TAG, "New IP acquired, obtaining time and initializing MQTT");
            if (strcmp(nvs_data.timeStamp, "default") == 0) {
                obtain_time();
            }
            if (!mqtt_init_flag) {
                mqtt_init_flag = true;
                mqtt_app_start();
            }
        }

        switch (dev_state) {
        case DEV_SOFTAP:
            if (timeout_count > 200)
                wifiSwitch(WIFI_MODE_STA);
            break;
        case DEV_STA_CONNECTING:
            if (timeout_count > 60)
                wifiSwitch(WIFI_MODE_AP);
            break;
        case DEV_MQTT_CONNECTING:
            if (timeout_count > 30)
                esp_restart();
            break;
        case DEV_MQTT_CONNECTED:
            break;
        default:
            ESP_LOGE(TAG, "Error state detected");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    ESP_LOGI(TAG, "Starting application");
    size_t flash_size = spi_flash_get_chip_size();
    ESP_LOGI(TAG, "Flash size: %d bytes", flash_size);

    gpio_init();
    uint32_t duty_cycle[] = {0, 0, 0};
    init_pwm(io_pins, 3, 1000, duty_cycle);
    xTaskCreate(pwm_update_task, "PWM Update Task", 2048, NULL, 10, NULL);

    init_nvs();
    nvs_read_data_from_flash();

    user_wifi_init();
    xTaskCreate(netState_check_task, "Net State Check Task", 2048, NULL, 10, NULL);
}
