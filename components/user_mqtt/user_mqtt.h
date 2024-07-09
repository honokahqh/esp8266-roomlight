#ifndef USER_MQTT_H
#define USER_MQTT_H

    
#define MQTT_SubscribeTopic "roomlight/sn/ctrl"
#define MQTT_UpdateTopic "roomlight/update"

// TOPIC1   CTRL    TOPIC2   CTRLACK
// TOPIC3   SYN     TOPIC4   SYNACK
// TOPIC5   DATA    TOPIC6   DATAACK
void mqtt_app_start(void);
void publish_roomlight_update(const char *topic , const char *data);

typedef struct 
{
    int is_connected;
    int timeout;
}user_mqttState_t;
extern user_mqttState_t user_mqttState;  

#endif // USER_MQTT_H
