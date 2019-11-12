#ifndef SETTINGS_H_
#define SETTINGS_H_

// put all settings that can be configured in the wheel here
typedef struct Configuration {
    int driver_index;
    int screen_brightness;
    int led_intensity;
    int power_mode_index;
    int power_bias;
    int regen_braking;
} Configuration;

#endif