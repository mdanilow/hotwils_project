#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <uni.h>

typedef struct {
    int32_t axis_x, axis_y, axis_rx, axis_ry, throttle;
    uint16_t buttons;
    bool connected;
} controller_state_t;

void controller_state_init(void);
void controller_state_write(const uni_gamepad_t* gp);
void controller_state_read(controller_state_t* out);
void controller_state_set_connection_status(bool is_connected);