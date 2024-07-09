#ifndef USER_NVS_H
#define USER_NVS_H

#include "nvs_flash.h"

#define NVS_STORAGE_MAX 30

#define NVS_SSID "ssid"
#define NVS_PASS "pass"
#define NVS_USERID "user"
#define NVS_ROOMID "room"
#define NVS_TimeStamp "timestamp" // 基于第一次连接wifi获取到的时间戳作为延长sn码
#define NVS_StringNum   5
#define NVS_LightNormal "lightNormal" //常亮颜色
#define NVS_LightPeriod     "lightPeriod" // 切换周期和切换颜色
#define NVS_LightSwitch1    "lightSwitch1"
#define NVS_LightSwitch2    "lightSwitch2"        
#define NVS_LightSwitch3    "lightSwitch3"  
#define NVS_Reboot "reboot"
#define NVS_IntNum 5

typedef enum
{
    DEV_SOFTAP = 0,
    DEV_STA_CONNECTING,
    DEV_MQTT_CONNECTING,
    DEV_MQTT_CONNECTED,
}dev_state_t;
extern dev_state_t dev_state;

typedef struct
{
    char ssid[NVS_STORAGE_MAX];
    char pass[NVS_STORAGE_MAX];
    char userID[NVS_STORAGE_MAX];
    char roomID[NVS_STORAGE_MAX];
    char timeStamp[NVS_STORAGE_MAX];
    char lightNormal[NVS_STORAGE_MAX];
    char lightPeriod[NVS_STORAGE_MAX];
    char lightSwitch1[NVS_STORAGE_MAX];
    char lightSwitch2[NVS_STORAGE_MAX];
    char lightSwitch3[NVS_STORAGE_MAX];
} nvs_data_t;
extern nvs_data_t nvs_data;
extern uint8_t mac[6];
extern uint16_t uniqueId;

void init_nvs();
int nvs_write_data_to_flash(const char *input);
void nvs_read_data_from_flash(void);
void parse_data_packet(const char *data_packet, int data_packet_len, nvs_data_t *nvs_data);
#endif // USER_NVS_H
