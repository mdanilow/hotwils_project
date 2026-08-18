#pragma once
#include <stdlib.h>

#include <argtable3/argtable3.h>
#include <esp_console.h>
#include <uni.h>
#include <driver/ledc.h>

#define SERVO_GPIO          GPIO_NUM_9
#define SERVO_TIMER         LEDC_TIMER_0
#define SERVO_CHANNEL       LEDC_CHANNEL_0
#define SERVO_FREQ_HZ       50
#define SERVO_RES_BITS      LEDC_TIMER_14_BIT
// 0 - 180 degrees range NOT SAFE
#define SERVO_PULSE_MIN_US  1050
#define SERVO_PULSE_MAX_US  2950
// safe pulse range
#define SERVO_SAFE_PULSE_MIN_US  1419
#define SERVO_SAFE_PULSE_MAX_US  2580
// safe angle range
#define SERVO_MIN_SAFE_ANGLE 35
#define SERVO_MAX_SAFE_ANGLE 145
#define SERVO_DUTY_US       (1000000 / SERVO_FREQ_HZ);


void servo_init(void);
void servo_set_pulse_us(float pulse_us);
void servo_set_angle(float angle);
void servo_set_offset_degrees(float offset);
float get_servo_neutral_angle(void);
float get_servo_offset_degrees(void);
void register_servo_console_cmd(void);