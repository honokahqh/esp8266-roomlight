#include "user_test.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "user_nvs.h"
#include <string.h>

#define EXAMPLE_ESP_WIFI_SSID "honoka"
#define EXAMPLE_ESP_WIFI_PASS "12345678"
#define EXAMPLE_MAX_STA_CONN 2
#define PORT 3333
#define MAX_PACKETS 4
#define MAX_PACKET_SIZE 128

#define EXAMPLE_ESP_MAXIMUM_RETRY 5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static int s_retry_num = 0;

static const char *TAG = "wifi ctrl";
wifi_config_t wifi_config;
user_wifi_state_t user_wifi_state;
static EventGroupHandle_t s_wifi_event_group;
static void tcp_server_task(void *pvParameters);

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    // ap event handler
    if (dev_state == DEV_SOFTAP) {
        if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            wifi_event_ap_staconnected_t *event =
                (wifi_event_ap_staconnected_t *)event_data;
            ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
                     MAC2STR(event->mac), event->aid);
            user_wifi_state.newDeviceConnect = true;
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            wifi_event_ap_stadisconnected_t *event =
                (wifi_event_ap_stadisconnected_t *)event_data;
            ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                     MAC2STR(event->mac), event->aid);
        }
    }
    else {
        // sta event handler
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
        } else if (event_base == WIFI_EVENT &&
                   event_id == WIFI_EVENT_STA_DISCONNECTED) {
            if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(TAG, "retry to connect to the AP");
            } else {
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
            ESP_LOGI(TAG, "connect to the AP fail");
        } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            user_wifi_state.getNewIP = true;
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }
}

// ap tcp process
void tcp_server_task(void *pvParameters) {
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (1) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err =
            bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket listening");

        while (1) {
            struct sockaddr_in source_addr;
            uint addr_len = sizeof(source_addr);
            int sock =
                accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
            if (sock < 0) {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Socket accepted");
            while (1) {
                int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                if (len < 0) {
                    ESP_LOGE(TAG, "recv failed: errno %d", errno);
                    break;
                } else if (len == 0) {
                    ESP_LOGI(TAG, "Connection closed");
                    break;
                } else {
                    rx_buffer[len] = 0;
                    ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
                    parse_data_packet(rx_buffer, len, &nvs_data);
                }
            }

            if (sock != -1) {
                ESP_LOGE(TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }
        }
    }
    vTaskDelete(NULL);
}

void user_wifi_init() {
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               &wifi_event_handler, NULL));

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                               &wifi_event_handler, NULL));

    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
}

void wifiSwitch(int mode) {
    esp_wifi_stop();
    esp_wifi_set_mode(mode);
    if (mode == WIFI_MODE_AP) {
        dev_state = DEV_SOFTAP;
        memset(&wifi_config, 0, sizeof(wifi_config_t));
        char user_ssid[32];
        sprintf(user_ssid, "%s-%04x", EXAMPLE_ESP_WIFI_SSID, uniqueId);
        strncpy((char *)wifi_config.ap.ssid, user_ssid, 32);
        wifi_config.ap.ssid_len = strlen(user_ssid);
        strncpy((char *)wifi_config.ap.password, EXAMPLE_ESP_WIFI_PASS, 64);
        wifi_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
            wifi_config.ap.authmode = WIFI_AUTH_OPEN;
        }
        esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
        ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
                 user_ssid, EXAMPLE_ESP_WIFI_PASS);
    } else if (mode == WIFI_MODE_STA) {
        dev_state = DEV_STA_CONNECTING;
        memset(&wifi_config, 0, sizeof(wifi_config_t));
        strncpy((char *)wifi_config.sta.ssid, nvs_data.ssid,
                sizeof(wifi_config.sta.ssid) - 1);
        strncpy((char *)wifi_config.sta.password, nvs_data.pass,
                sizeof(wifi_config.sta.password) - 1);
        if (strlen((char *)wifi_config.sta.password)) {
            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
        }
        esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
        ESP_LOGI(TAG, "wifi_init_sta finished.");
    }
    esp_wifi_start();
    // 如果是STA模式，尝试连接
    if (mode == WIFI_MODE_STA) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
}