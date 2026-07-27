#include "servo_control.h"



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

void servo_set_pulse_us(float pulse_us){
    logi("servo pulse us: %f", pulse_us);
    if(pulse_us < SERVO_SAFE_PULSE_MIN_US) pulse_us = SERVO_SAFE_PULSE_MIN_US;
    if(pulse_us > SERVO_SAFE_PULSE_MAX_US) pulse_us = SERVO_SAFE_PULSE_MAX_US;

    int max_duty = (1 << SERVO_RES_BITS) - 1;
    int duty = (pulse_us * max_duty) / SERVO_DUTY_US;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL);
}

void servo_set_angle(float angle) {
    if(angle < SERVO_MIN_SAFE_ANGLE) angle = SERVO_MIN_SAFE_ANGLE;
    if(angle > SERVO_MAX_SAFE_ANGLE) angle = SERVO_MAX_SAFE_ANGLE;

    static const int PULSE_RANGE_WIDTH = SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US;
    float pulse_us = SERVO_PULSE_MIN_US + (angle * PULSE_RANGE_WIDTH / 180);
    servo_set_pulse_us(pulse_us);
}
// ----------------------- console commands

static struct {
    struct arg_int* pulse_width_us;
    struct arg_end* end;
} set_servo_pulse_us_cmd_args;

static struct {
    struct arg_dbl* servo_angle;
    struct arg_end* end;
} set_servo_angle_cmd_args;


static int set_servo_pulse_us_cmd_func(int argc, char** argv) {
    int nerrors = arg_parse(argc, argv, (void**)&set_servo_pulse_us_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_servo_pulse_us_cmd_args.end, argv[0]);
        return 1;
    }
    servo_set_pulse_us(set_servo_pulse_us_cmd_args.pulse_width_us->ival[0]);
    return 0;
}

static int set_servo_angle_cmd_func(int argc, char** argv) {
    int nerrors = arg_parse(argc, argv, (void**)&set_servo_angle_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_servo_angle_cmd_args.end, argv[0]);
        return 1;
    }
    servo_set_angle(set_servo_angle_cmd_args.servo_angle->dval[0]);
    return 0;
}

void register_servo_console_cmd(void) {
    set_servo_pulse_us_cmd_args.pulse_width_us =
        arg_int1(NULL, NULL, "<pulse_us>", "Pulse width in microseconds");
    set_servo_pulse_us_cmd_args.end = arg_end(1);

    set_servo_angle_cmd_args.servo_angle =
        arg_dbl1(NULL, NULL, "<degrees>", "Angle in degrees [0 - 180]");
    set_servo_angle_cmd_args.end = arg_end(1);

    const esp_console_cmd_t cmd_set_servo_pulse_us = {
        .command = "set_servo_pulse_us",
        .help = "Set pulse width to control the servo",
        .hint = NULL,
        .func = &set_servo_pulse_us_cmd_func,
        .argtable = &set_servo_pulse_us_cmd_args,
    };
    const esp_console_cmd_t cmd_set_servo_angle = {
        .command = "set_servo_angle",
        .help = "Set servo position in degrees",
        .hint = NULL,
        .func = &set_servo_angle_cmd_func,
        .argtable = &set_servo_angle_cmd_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_set_servo_pulse_us));
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_set_servo_angle));
}
