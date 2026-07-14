#pragma once
#include <stdlib.h>

#include <argtable3/argtable3.h>
#include <esp_console.h>
#include <driver/ledc.h>

#define SERVO_GPIO          GPIO_NUM_9
#define SERVO_TIMER         LEDC_TIMER_0
#define SERVO_CHANNEL       LEDC_CHANNEL_0
#define SERVO_FREQ_HZ       50
#define SERVO_RES_BITS      LEDC_TIMER_14_BIT
#define SERVO_PULSE_MIN_US  800
#define SERVO_PULSE_MAX_US  2200
#define SERVO_DUTY_US       1000000 / SERVO_FREQ_HZ;


void servo_init(void);
void servo_set_pulse_us(int pulse_us);
void servo_set_angle(float angle);
void register_servo_console_cmd(void);