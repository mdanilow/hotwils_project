#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <uni.h>

#define CONTROLLER_CROSS_BUTTON_MASK (1 << 0)
#define CONTROLLER_CIRCLE_BUTTON_MASK (1 << 1)
#define CONTROLLER_SQUARE_BUTTON_MASK (1 << 2)
#define CONTROLLER_TRIANGLE_BUTTON_MASK (1 << 3)

typedef struct {
    int32_t axis_x, axis_y, axis_rx, axis_ry, throttle;
    uint16_t buttons;
    uint16_t misc_buttons;
    bool cross_button;
    bool circle_button;
    bool square_button;
    bool triangle_button;
    bool connected;
} controller_state_t;

void controller_state_init(void);
void controller_state_write(const uni_gamepad_t* gp);
void controller_state_read(controller_state_t* out);
void controller_state_set_connection_status(bool is_connected);