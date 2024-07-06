#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := wifi_softAP

include $(IDF_PATH)/make/project.mk

target_add_binary_data(mqtt_ssl.elf "components/user_mqtt/mqtt_eclipse_org.pem" TEXT)
