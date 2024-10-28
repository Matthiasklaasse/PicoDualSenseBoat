#include <btstack_run_loop.h>
#include <pico/cyw43_arch.h>
#include <stdio.h>
#include <uni.h>
#include "global_vars.h"
#include "hardware/pwm.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "pico/mutex.h"
#include "pico/stdlib.h"
#include "sdkconfig.h"

#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Pico W must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

struct uni_platform* get_my_platform(void);

#define DEFAULT_SPEED 0.17
#define BOOST1_MULTIPLIER 1.2
#define BOOST2_MULTIPLIER 1.3
#define BACKWARDS_THROTTLE 7
#define STEERING_ANGLE 0.1
#define STEP_SPEED 1

#define LEFT_POSITIVE_TERMINAL 14
#define LEFT_NEGATIVE_TERMINAL 15
#define RIGHT_POSITIVE_TERMINAL 17
#define RIGHT_NEGATIVE_TERMINAL 16

#define VERBOSE_DEBUGGING true

bool boost1 = false;
bool boost1Toggle = false;

int actualSpeedLeft = 0;
int actualSpeedRight = 0;

mutex_t mutexMainThread;

void bt_thread() {
    btstack_run_loop_execute();
}

void init_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, 255);
    pwm_set_enabled(slice_num, true);
}

void analogWrite(uint gpio, uint16_t value) {
    if (value > 255) {
        value = 255;
    }
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), value);
}

int main() {
    stdio_init_all();
    watchdog_enable(5000, true);

    if (cyw43_arch_init()) {
        loge("Failed to initialize cyw43_arch\n");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    uni_platform_set_custom(get_my_platform());
    uni_init(0, NULL);

    mutex_init(&mutexMainThread);
    multicore_launch_core1(bt_thread);

    init_pwm(LEFT_NEGATIVE_TERMINAL);
    init_pwm(RIGHT_NEGATIVE_TERMINAL);
    init_pwm(LEFT_POSITIVE_TERMINAL);
    init_pwm(RIGHT_POSITIVE_TERMINAL);

    int loopsSinceLastRumbleSync = 0;
    int loopsSinceLastLedSync = 0;

    while (true) {
        if (mutex_try_enter(&mutexMainThread, NULL)) {
            int speed = RightTrigger * DEFAULT_SPEED;
            int leftSpeed = RightTrigger;
            int rightSpeed = RightTrigger;

            int oldRumble = Rumble;
            int oldSoftRumble = SoftRumble;
            int backwards = BACKWARDS_THROTTLE;
            loopsSinceLastRumbleSync++;
            loopsSinceLastLedSync++;

            SoftRumble = 0;
            Rumble = 0;
            R = 0;
            G = 255;
            B = 0;

            if (boost1) {
                speed *= BOOST1_MULTIPLIER;
                SoftRumble += 70;
                R = 255;
                G = 255;
            }

            if (OIsDown) {
                speed *= BOOST2_MULTIPLIER;
                SoftRumble = 255;
                backwards -= 5;
                G = 100;
                R = 255;
            }

            if (!boost1Toggle && TriangleIsDown)
                boost1 = !boost1;

            if (SquareIsDown)
                Battery = ControllerBattery;
            else
                Battery = BoatBattery;

            if (XIsDown)
                BoatBattery++;
            if (BoatBattery > 100)
                BoatBattery = 0;

            boost1Toggle = TriangleIsDown;

            speed -= LeftTrigger / backwards;
            leftSpeed = speed - ((RightStickX / 15) * 15 * (STEERING_ANGLE) * -1);
            rightSpeed = speed - ((RightStickX / 15) * 15 * STEERING_ANGLE);

            if (SoftRumble != oldSoftRumble || Rumble != oldRumble || loopsSinceLastRumbleSync > 50) {
                RumbleIsInSync = false;
                loopsSinceLastRumbleSync = 0;
            }

            if (speed > 0)
                SoftRumble += speed / 15;
            if (SoftRumble > 255)
                SoftRumble = 255;

            if (BoatBattery < 4 || ControllerBattery < 4) {
                Rumble = 255;
                SoftRumble = 255;

                R = 255;
                G = 0;
                B = 0;
            }

            if (loopsSinceLastLedSync > 10) {
                loopsSinceLastLedSync = 0;
                BatteryLedIsInSync = false;
            }

            if(VERBOSE_DEBUGGING)
                logi("Speed: %d, Left Speed: %d, Right Speed: %d, actual speed left: %d right: %d, battery: %d\n", speed, leftSpeed, rightSpeed, actualSpeedLeft, actualSpeedRight, Battery);
            
            if(actualSpeedLeft>leftSpeed)
                actualSpeedLeft -= STEP_SPEED;
            if(actualSpeedLeft<leftSpeed)
                actualSpeedLeft += STEP_SPEED;
            if(actualSpeedRight>rightSpeed)
                actualSpeedRight -= STEP_SPEED;
            if(actualSpeedRight<rightSpeed)
                actualSpeedRight += STEP_SPEED;

            if (actualSpeedLeft > 0) {
                analogWrite(LEFT_POSITIVE_TERMINAL, actualSpeedLeft);
                analogWrite(LEFT_NEGATIVE_TERMINAL, 0);
            } else {
                analogWrite(LEFT_POSITIVE_TERMINAL, 0);
                analogWrite(LEFT_NEGATIVE_TERMINAL, -actualSpeedLeft);
            }

            if (actualSpeedRight > 0) {
                analogWrite(RIGHT_POSITIVE_TERMINAL, actualSpeedRight);
                analogWrite(RIGHT_NEGATIVE_TERMINAL, 0);
            } else {
                analogWrite(RIGHT_POSITIVE_TERMINAL, 0);
                analogWrite(RIGHT_NEGATIVE_TERMINAL, -actualSpeedRight);
            }

            if (R > 255)
                R = 255;
            if (G > 255)
                G = 255;
            if (B > 255)
                B = 255;

            mutex_exit(&mutexMainThread);
            watchdog_update();
        }
        sleep_ms(10);
    }

    return 1;
}
