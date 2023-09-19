#include "humidity.h"
#include "wifi.h"
#include "color.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "driver/gpio.h" /* Set gpio pins */
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_log.h" /* Logging library */
// #include "esp_chip_info.h"
#include "esp_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/* Bit mask of selected pin */
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_NUM_26)

float r, g, b;

/* simple function that turns led on and off to test microcontroller */
void blink(void)
{
    /* zero-initialize the config structure. */
    gpio_config_t io_conf = {};
    /* disable interrupt */
    io_conf.intr_type = GPIO_INTR_DISABLE;
    /* set as output mode */
    io_conf.mode = GPIO_MODE_OUTPUT;
    /* bit mask of the pins that you want to set */
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    /* disable pull-down mode */
    io_conf.pull_down_en = 0;
    /* disable pull-up mode */
    io_conf.pull_up_en = 0;
    /* configure GPIO with the given settings */
    gpio_config(&io_conf);

    /* pull pin high */
    gpio_set_level(GPIO_NUM_26, 1);
    /* delay for 1 second */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    /* pull pin low */
    gpio_set_level(GPIO_NUM_26, 0);
}


void app_main(void)
{
    // blink();

    i2c_init();

    tcs_init();

    get_color(r, g, b);

    // wifi_init();

    // int humidity = get_humidity(20190);
    // ESP_LOGI("humidity", "%d", humidity);

    // /* Print chip information */ 
    // esp_chip_info_t chip_info;
    // uint32_t flash_size;
    // esp_chip_info(&chip_info);
    // printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
    //        CONFIG_IDF_TARGET,
    //        chip_info.cores,
    //        (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
    //        (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    // unsigned major_rev = chip_info.revision / 100;
    // unsigned minor_rev = chip_info.revision % 100;
    // printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    // if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
    //     printf("Get flash size failed");
    //     return;
    // }

    // printf("%uMB %s flash\n", flash_size / (1024 * 1024),
    //        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    // printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();
}
