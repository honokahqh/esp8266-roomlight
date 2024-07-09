# Install script for directory: D:/workspace/esp8266/ESP8266_RTOS_SDK

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/wifi_softAP")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/tcpip_adapter/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp_common/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp_event/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/mbedtls/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/wpa_supplicant/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp_ringbuf/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/bootloader_support/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/nvs_flash/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp8266/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/vfs/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/lwip/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/log/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/heap/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/freertos/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/newlib/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/spi_flash/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/partition_table/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/app_update/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/bootloader/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/coap/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/console/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/http_parser/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp-tls/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp-wolfssl/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp_gdbstub/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/tcp_transport/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp_http_client/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp_http_server/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esp_https_ota/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/esptool_py/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/wear_levelling/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/fatfs/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/freemodbus/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/jsmn/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/json/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/libsodium/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/mdns/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/mqtt/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/openssl/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/protobuf-c/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/protocomm/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/pthread/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/spi_ram/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/spiffs/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/wifi_provisioning/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/main/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_nvs/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_mqtt/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_gpio/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_ota/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_pwm/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_softap/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_test/cmake_install.cmake")
  include("D:/workspace/esp8266/ESP8266_RTOS_SDK/examples/honoka/softAP/build/esp-idf/user_wifi/cmake_install.cmake")

endif()

