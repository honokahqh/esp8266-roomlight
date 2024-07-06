idf_component_register(SRCS "user_mqtt.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "mqtt_eclipse_org.pem"
                    REQUIRES nvs_flash mqtt app_update user_nvs )
