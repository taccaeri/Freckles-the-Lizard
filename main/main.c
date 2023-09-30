#include "humidity.h"
#include "wifi.h"
#include "color.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "driver/gpio.h" 
#include "driver/ledc.h"
#include "sdkconfig.h"

#include "esp_check.h"
#include "esp_system.h"
#include "esp_log.h" 
#include "esp_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

float r, g, b;

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT 
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

#define LEDC_RED                (18) 
#define LEDC_GREEN              (17) 
#define LEDC_BLUE               (16) 


static void ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_red = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_RED,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_red));

        ledc_channel_config_t ledc_green = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_GREEN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_green));

        ledc_channel_config_t ledc_blue = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_BLUE,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_blue));
}

void ledc_output(uint8_t R, uint8_t G, uint8_t B)
{
    /* RED */
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, R);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    /* GREEN */
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, G);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
    /* BLUE */
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, B);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
}

void app_main(void)
{
    // blink();

    i2c_init();

    tcs_init();

    ledc_init();

    ledc_output(255, 128, 128);

    // uint8_t val = 0;

    // while (1) {
    //     /* Set the voltage every 100 ms */
    //     ledc_output(val, 0, 0);
    //     val += 10;
    //     val %= 250;
    //     vTaskDelay(pdMS_TO_TICKS(500));
    // }

    // get_color(r, g, b);

    // wifi_init();

    // int humidity = get_humidity(20190);
    // ESP_LOGI("humidity", "%d", humidity);
}