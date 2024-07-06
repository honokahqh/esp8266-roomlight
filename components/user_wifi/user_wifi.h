#ifndef __USER_WIFI_H__
#define __USER_WIFI_H__

int wifi_init_sta(const char *ssid, const char *pass);
void wifi_deinit();

#endif