#ifndef __USER_SOFTAP_H__
#define __USER_SOFTAP_H__

#define SOFTAP_TIMEOUT 60
typedef struct 
{
    /* data */
    bool isEnable;
    bool isBind;
    int timeoutCount;
}user_softap_state_t;
extern user_softap_state_t user_softap_state;

void wifi_init_softap();
void wifi_deinit_softap();

#endif