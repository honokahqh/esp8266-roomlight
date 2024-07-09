#include "user_nvs.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#define TAG "user nvs"

static const char *NVS_CUSTOMER = "customer_data";
dev_state_t dev_state = DEV_SOFTAP;

uint16_t uniqueId;
uint8_t mac[6] = {0};

static const char *LABELS[] = {
    NVS_SSID,         NVS_PASS,         NVS_USERID,
    NVS_ROOMID,       NVS_LightNormal,  NVS_LightPeriod,
    NVS_LightSwitch1, NVS_LightSwitch2, NVS_LightSwitch3};

nvs_data_t nvs_data;

void init_nvs() {
    uint8_t mac[6];
    esp_err_t ret = esp_efuse_mac_get_default(mac);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1],
               mac[2], mac[3], mac[4], mac[5]);
    } else {
        ESP_LOGE(TAG, "Failed to get MAC address: %s\n", esp_err_to_name(ret));
    }
    uniqueId = ((mac[0] + mac[1] + mac[2]) << 8) + mac[3] + mac[4] + mac[5];

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

int nvs_write_data_to_flash(const char *input) {
    if (input == NULL) {
        return 0;
    }

    char key[16];
    char value[128];
    const char *delimiter = ":";
    char *ptr = strstr(input, delimiter);
    if (ptr == NULL) {
        return 0;
    }

    int key_length = ptr - input;
    if (key_length >= sizeof(key)) {
        return 0;
    }

    strncpy(key, input, key_length);
    key[key_length] = '\0';
    strncpy(value, ptr + 1, sizeof(value) - 1);
    value[sizeof(value) - 1] = '\0';

    int is_valid_label = 0;
    for (int i = 0; i < sizeof(LABELS) / sizeof(LABELS[0]); i++) {
        if (strcmp(key, LABELS[i]) == 0) {
            is_valid_label = 1;
            break;
        }
    }

    if (!is_valid_label) {
        return 0;
    }

    nvs_handle handle;
    esp_err_t err = nvs_open(NVS_CUSTOMER, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return 0;
    }

    err = nvs_set_str(handle, key, value);
    if (err != ESP_OK) {
        nvs_close(handle);
        return 0;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    return (err == ESP_OK) ? 1 : 0;
}

void nvs_read_data_from_flash() {
    nvs_handle handle;
    esp_err_t err = nvs_open(NVS_CUSTOMER, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }
    char *data_pointers[] = {
        nvs_data.ssid,         nvs_data.pass,         nvs_data.userID,
        nvs_data.roomID,       nvs_data.lightNormal,  nvs_data.lightPeriod,
        nvs_data.lightSwitch1, nvs_data.lightSwitch2, nvs_data.lightSwitch3};
    for (int i = 0; i < sizeof(LABELS) / sizeof(LABELS[0]); i++) {
        size_t value_length =
            sizeof(nvs_data.ssid); // Assuming all fields are the same size
        err = nvs_get_str(handle, LABELS[i], data_pointers[i], &value_length);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "[%s]: %s", LABELS[i], data_pointers[i]);
        } else if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "[%s] not found.", LABELS[i]);
            // set as default
            strcpy(data_pointers[i], "default");
        } else {
            ESP_LOGI(TAG, "Error (%s) reading %s from NVS!",
                     esp_err_to_name(err), LABELS[i]);
        }
    }

    nvs_close(handle);
}

void parse_data_packet(const char *data_packet, int data_packet_len, nvs_data_t *nvs_data) {
    // 将数据包解析为 cJSON 对象
    cJSON *json = cJSON_Parse(data_packet);
    if (json == NULL) {
        ESP_LOGW(TAG, "Error parsing JSON data");
        return;
    }

    // 遍历所有标签并处理相应数据
    for (int i = 0; i < NVS_StringNum + NVS_IntNum; i++) {
        cJSON *item = cJSON_GetObjectItem(json, LABELS[i]);
        if (item != NULL && cJSON_IsString(item)) {
            const char *value = item->valuestring;
            if (strlen(value) > NVS_STORAGE_MAX) {
                ESP_LOGW(TAG, "Value too long for %s", LABELS[i]);
                cJSON_Delete(json);
                return;
            } else {
                strncpy((char *)nvs_data + offsetof(nvs_data_t, ssid) + i * NVS_STORAGE_MAX, value, NVS_STORAGE_MAX);
                ESP_LOGI(TAG, "Key: %s, Value: %s", LABELS[i], value);
                char storage_buffer[NVS_STORAGE_MAX + 10];
                sprintf(storage_buffer, "%s:%s", LABELS[i], value);
                nvs_write_data_to_flash(storage_buffer);
            }
        }
    }

    // 处理重启指令
    cJSON *reboot_item = cJSON_GetObjectItem(json, NVS_Reboot);
    if (reboot_item != NULL && cJSON_IsString(reboot_item) && strcmp(reboot_item->valuestring, "1") == 0) {
        ESP_LOGI(TAG, "Rebooting...");
        esp_restart();
    }

    // 释放 cJSON 对象
    cJSON_Delete(json);
}
