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
    g_state.throttle = gp->throttle;
    xSemaphoreGive(g_mutex);
}

void controller_state_read(controller_state_t* out) {
    xSemaphoreTake(g_mutex, portMAX_DELAY);
    *out = g_state;
    xSemaphoreGive(g_mutex);
}