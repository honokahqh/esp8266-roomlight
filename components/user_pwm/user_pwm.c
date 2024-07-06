#include "user_pwm.h"
#include "driver/pwm.h"
#include "esp_err.h"
#include "esp_log.h"
#include <string.h>

#define MAX_PWM_CHANNELS 3

static const char *TAG = "user_pwm";

static uint32_t g_io_num[MAX_PWM_CHANNELS];
static uint32_t g_channel_num = 0;
static uint32_t g_period = 0;

esp_err_t init_pwm(const uint32_t *io_num, uint32_t channel_num,
                   uint32_t frequency, const uint32_t *duty_cycle) {
    if (frequency == 0) {
        ESP_LOGE(TAG, "Invalid frequency");
        return ESP_ERR_INVALID_ARG;
    }

    if (channel_num > MAX_PWM_CHANNELS) {
        ESP_LOGE(TAG, "Too many PWM channels, maximum is %d", MAX_PWM_CHANNELS);
        return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < channel_num; i++) {
        if (duty_cycle[i] > 100) {
            ESP_LOGE(TAG, "Invalid duty cycle for channel %d", i);
            return ESP_ERR_INVALID_ARG;
        }
    }

    g_period = 1000000 / frequency; // PWM period in microseconds
    uint32_t duties[channel_num];   // Duty cycles in microseconds

    for (int i = 0; i < channel_num; i++) {
        duties[i] = (g_period * duty_cycle[i]) / 100;
        g_io_num[i] = io_num[i];
    }
    g_channel_num = channel_num;

    float phase[channel_num];        // Phases
    memset(phase, 0, sizeof(phase)); // Initialize phases to 0

    pwm_init(g_period, duties, channel_num, io_num);
    pwm_set_phases(phase);
    pwm_start();

    for (int i = 0; i < channel_num; i++) {
        ESP_LOGI(TAG,
                 "PWM initialized on GPIO %d with frequency %d Hz and duty "
                 "cycle %d%%",
                 io_num[i], frequency, duty_cycle[i]);
    }

    return ESP_OK;
}

int gamma_corrected_values[32] = {0,   1,   2,   5,   11,  18,  26,  37,
                                  50,  65,  82,  102, 123, 147, 173, 202,
                                  233, 266, 302, 340, 381, 424, 470, 518,
                                  569, 622, 679, 737, 799, 863, 930, 1000};

esp_err_t set_rgb_color(uint16_t lightness) {
    uint32_t r = gamma_corrected_values[(lightness >> 11) & 0x1f];
    uint32_t g = gamma_corrected_values[((lightness >> 5) & 0x3f) / 2];
    uint32_t b = gamma_corrected_values[lightness & 0x1f];
    // ESP_LOGI(TAG, "Setting RGB color to %d %d %d", r, g, b);
    pwm_stop(0);
    pwm_set_duty(0, r);
    pwm_set_duty(1, b);
    pwm_set_duty(2, g);
    pwm_start();
    return ESP_OK;
}

esp_err_t set_pwm_duty(uint32_t io_num, uint32_t duty_cycle) {
    if (duty_cycle > 100) {
        ESP_LOGE(TAG, "Invalid duty cycle");
        return ESP_ERR_INVALID_ARG;
    }

    int channel = -1;
    for (int i = 0; i < g_channel_num; i++) {
        if (g_io_num[i] == io_num) {
            channel = i;
            break;
        }
    }

    if (channel == -1) {
        ESP_LOGE(TAG, "IO %d not found in initialized PWM channels", io_num);
        return ESP_ERR_NOT_FOUND;
    }

    uint32_t duty = (g_period * duty_cycle) / 100;
    pwm_set_duty(channel, duty);
    pwm_start();

    return ESP_OK;
}
