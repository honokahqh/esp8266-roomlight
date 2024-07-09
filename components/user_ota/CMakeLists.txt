idf_component_register(SRCS "user_ota.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "cert.pem"
                    REQUIRES nvs_flash app_update esp_http_client esp_https_ota)

                    