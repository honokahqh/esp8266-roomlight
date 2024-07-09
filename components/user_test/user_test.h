#ifndef __USER_TEST_H__ 
#define __USER_TEST_H__

typedef struct 
{
    /* ap */
    int newDeviceConnect;
    int newDeviceBindTCP;

    /* sta */
    int getNewIP;
}user_wifi_state_t;
extern user_wifi_state_t user_wifi_state;

void user_wifi_init();
void wifiSwitch(int mode);

#endif