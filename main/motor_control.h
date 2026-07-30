#pragma once
#include <stdlib.h>

#include <argtable3/argtable3.h>
#include <esp_console.h>
#include <uni.h>
#include <driver/ledc.h>
#include <driver/gpio.h>

#define MOTOR_IN1_GPIO      GPIO_NUM_23
#define MOTOR_IN2_GPIO      GPIO_NUM_19
#define MOTOR_PWM_GPIO      GPIO_NUM_18

#define MOTOR_PWM_TIMER     LEDC_TIMER_1
#define MOTOR_PWM_CHANNEL   LEDC_CHANNEL_1
#define MOTOR_PWM_FREQ_HZ   50
#define MOTOR_PWM_RES_BITS  10



void motor_init(void){
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = MOTOR_PWM_TIMER,
        .duty_resolution = MOTOR_PWM_RES_BITS,
        .freq_hz = MOTOR_PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = MOTOR_PWM_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = MOTOR_PWM_CHANNEL,
        .timer_sel = MOTOR_PWM_TIMER,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&channel_conf);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1 << MOTOR_IN1_GPIO) | (1 << MOTOR_IN2_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}

void motor_set_pwm_10bit(uint16_t duty_cycle){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL, duty_cycle);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR_PWM_CHANNEL);
}

void motor_stop(void){
    gpio_set_level(MOTOR_IN1_GPIO, 0);
    gpio_set_level(MOTOR_IN2_GPIO, 0);
}

void motor_forward(void){
    gpio_set_level(MOTOR_IN1_GPIO, 0);
    gpio_set_level(MOTOR_IN2_GPIO, 1);
}

void motor_reverse(void){
    gpio_set_level(MOTOR_IN1_GPIO, 1);
    gpio_set_level(MOTOR_IN2_GPIO, 0);
}

void motor_brake(void){
    gpio_set_level(MOTOR_IN1_GPIO, 1);
    gpio_set_level(MOTOR_IN2_GPIO, 1);
}