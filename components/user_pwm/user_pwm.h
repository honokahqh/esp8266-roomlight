#ifndef USER_PWM_H
#define USER_PWM_H

#include <stdint.h>
#include "esp_err.h"

esp_err_t init_pwm(const uint32_t *io_num, uint32_t channel_num, uint32_t frequency, const uint32_t *duty_cycle);
esp_err_t set_pwm_duty(uint32_t io_num, uint32_t duty_cycle);
esp_err_t set_rgb_color(uint16_t lightness);
#endif // USER_PWM_H
