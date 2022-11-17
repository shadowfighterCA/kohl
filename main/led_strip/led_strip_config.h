

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"

#define LED_STRIP_TAG = "LED_STRIP_TAG";

#define RMT_TX_CHANNEL RMT_CHANNEL_0

#define EXAMPLE_CHASE_SPEED_MS (10)

rmt_config_t rmt_config112;

uint32_t red,blue,green;

led_strip_config_t strip_config;

led_strip_t *strip;
#define EXAMPLE_CHASE_SPEED_MS (10)

