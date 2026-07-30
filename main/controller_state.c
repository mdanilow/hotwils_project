#include "controller_state.h"

static controller_state_t g_state = {0};
static SemaphoreHandle_t g_mutex;

void controller_state_init(void) {
    g_mutex = xSemaphoreCreateMutex();
}

void controller_state_set_connection_status(bool is_connected){
    g_state.connected = is_connected;
}

void controller_state_write(const uni_gamepad_t* gp) {
    xSemaphoreTake(g_mutex, portMAX_DELAY);
    g_state.axis_x = gp->axis_x;
    g_state.axis_y = gp->axis_y;
    g_state.axis_rx = gp->axis_rx;
    g_state.axis_ry = gp->axis_ry;
    g_state.buttons = gp->buttons;
    g_state.misc_buttons = gp->misc_buttons;
    g_state.throttle = gp->throttle;
    g_state.cross_button = gp->buttons & CONTROLLER_CROSS_BUTTON_MASK;
    g_state.circle_button = gp->buttons & CONTROLLER_CIRCLE_BUTTON_MASK;
    g_state.square_button = gp->buttons & CONTROLLER_SQUARE_BUTTON_MASK;
    g_state.triangle_button = gp->buttons & CONTROLLER_TRIANGLE_BUTTON_MASK;
    xSemaphoreGive(g_mutex);
}

void controller_state_read(controller_state_t* out) {
    xSemaphoreTake(g_mutex, portMAX_DELAY);
    *out = g_state;
    xSemaphoreGive(g_mutex);
}