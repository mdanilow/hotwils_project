// Example file - Public Domain
// Need help? http://bit.ly/bluepad32-help

#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>

#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include <btstack_stdio_esp32.h>
#include <hci_dump.h>
#include <hci_dump_embedded_stdout.h>
#include <uni.h>

#include "sdkconfig.h"
#include "controller_state.h"

#define SERVO_GPIO      GPIO_NUM_9
#define SERVO_TIMER     LEDC_TIMER_0
#define SERVO_CHANNEL   LEDC_CHANNEL_0
#define SERVO_FREQ_HZ   50
#define SERVO_RES_BITS  LEDC_TIMER_14_BIT
const unsigned int servo_duty_us = 1000000 / SERVO_FREQ_HZ;

// Sanity check
#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

// Defined in my_platform.c
struct uni_platform* get_my_platform(void);

static void btstack_task(void* task_arg) {
    // Configure BTstack for ESP32 VHCI Controller
    btstack_init();
    // Must be called before uni_init()
    uni_platform_set_custom(get_my_platform());
    // Init Bluepad32.
    uni_init(0 /* argc */, NULL /* argv */);
    btstack_run_loop_execute();
}

void servo_init(void) {
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = SERVO_TIMER,
        .duty_resolution = SERVO_RES_BITS,
        .freq_hz = SERVO_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = SERVO_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = SERVO_CHANNEL,
        .timer_sel = SERVO_TIMER,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&channel_conf);
}

void servo_set_angle(float angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    float pulse_us = 1000 + (angle * 1000 / 180);
    int max_duty = (1 << SERVO_RES_BITS) - 1;
    int duty = (pulse_us * max_duty) / servo_duty_us;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL);
}

int app_main(void) {
    // If you enable HCI Dump better to disable "Bluepad32 USB Console" from "idf.py menuconfig".
    // hci_dump_init(hci_dump_embedded_stdout_get_instance());

    // Don't use BTstack buffered UART. It conflicts with the console.
#ifdef CONFIG_ESP_CONSOLE_UART
#ifndef CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE
    btstack_stdio_init();
#endif  // CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE
#endif  // CONFIG_ESP_CONSOLE_UART

    // Start infinite btstack task
    xTaskCreate(btstack_task, "btstack_task", 16384, NULL, 5, NULL);

    servo_init();
    controller_state_init();
    controller_state_t controller;

    //control loop
    while(true){
        controller_state_read(&controller);
        if(controller.connected){
            // axis_x is from -512 to 511, scale it to angle 0 - 180
            float servo_angle = 180 * (controller.axis_x + 512) / 1024;
            servo_set_angle(servo_angle);
            logi("throttle = %d, axis_x = %d, servo_angle = %f\n", controller.throttle, controller.axis_x, servo_angle);
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz control
    }

    return 0;
}
