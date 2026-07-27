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
#include "servo_control.h"


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
    register_servo_console_cmd();
    controller_state_init();
    controller_state_t controller;

    //control loop
    while(true){
        controller_state_read(&controller);
        if(controller.connected){
            // axis_x is from -512 to 511, scale it to angle 0 - 180
            static int servo_safe_range = SERVO_MAX_SAFE_ANGLE - SERVO_MIN_SAFE_ANGLE;
            float servo_angle = SERVO_MIN_SAFE_ANGLE + servo_safe_range * (controller.axis_x + 512) / 1024;
            servo_set_angle(servo_angle);
            logi("throttle = %d, axis_x = %d, servo_angle = %f\n", controller.throttle, controller.axis_x, servo_angle);
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz control
    }

    return 0;
}
