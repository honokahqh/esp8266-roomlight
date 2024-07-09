# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wifi_softAP)

list(APPEND EXTRA_COMPONENT_DIRS "components/user_gpio")
list(APPEND EXTRA_COMPONENT_DIRS "components/user_softap")
list(APPEND EXTRA_COMPONENT_DIRS "components/user_wifi")
list(APPEND EXTRA_COMPONENT_DIRS "components/user_nvs")
list(APPEND EXTRA_COMPONENT_DIRS "components/user_pwm")
list(APPEND EXTRA_COMPONENT_DIRS "components/user_mqtt")
list(APPEND EXTRA_COMPONENT_DIRS "components/user_ota")
list(APPEND EXTRA_COMPONENT_DIRS "components/user_test")