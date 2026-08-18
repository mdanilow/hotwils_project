// Example file - Public Domain
// Need help? http://bit.ly/bluepad32-help

#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <driver/gpio.h>

#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include <btstack_stdio_esp32.h>
#include <hci_dump.h>
#include <hci_dump_embedded_stdout.h>
#include <uni.h>

#include "sdkconfig.h"
#include "controller_state.h"
#include "servo_control.h"
#include "motor_control.h"

// #define LED_GPIO GPIO_NUM_23

// Sanity check
#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

typedef enum {
    FORWARD,
    REVERSE
} GEAR_t;


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

    // servo and controller init
    servo_init();
    motor_init();
    register_servo_console_cmd();
    controller_state_init();
    controller_state_t controller;

    GEAR_t gear = FORWARD;

    // set steering wheels to neutral on startup
    servo_set_angle(get_servo_neutral_angle());

    //control loop
    while(true){
        controller_state_read(&controller);
        if(controller.connected){
            // gpio_set_level(LED_GPIO, 1);

            // servo control
            // axis_x is from -512 to 511, scale it to safe angle range
            static int servo_safe_range = SERVO_MAX_SAFE_ANGLE - SERVO_MIN_SAFE_ANGLE;
            float servo_angle = SERVO_MIN_SAFE_ANGLE + get_servo_offset_degrees() + servo_safe_range * (-controller.axis_x + 512) / 1024;
            servo_set_angle(servo_angle);

            // gear control
            if(controller.triangle_button) gear = FORWARD;
            else if(controller.cross_button) gear = REVERSE;

            // motor control
            if(gear == FORWARD){
                motor_forward();
                motor_set_pwm_10bit(controller.throttle);
            }
            else if(gear == REVERSE){
                motor_reverse();
                motor_set_pwm_10bit(controller.throttle);
            }

            // logi("throttle = %d, axis_x = %d, servo_angle = %f, buttons=0x%04x, misc=0x%02x\n",
            //     controller.throttle,
            //     controller.axis_x,
            //     servo_angle,
            //     controller.buttons,
            //     controller.misc_buttons
            // );
        }
        else{
            // gpio_set_level(LED_GPIO, 1);
            // vTaskDelay(pdMS_TO_TICKS(500));
            // gpio_set_level(LED_GPIO, 0);
            // vTaskDelay(pdMS_TO_TICKS(500));
            motor_set_pwm_10bit(0);
            servo_set_angle(get_servo_neutral_angle());
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz control
    }

    return 0;
}
