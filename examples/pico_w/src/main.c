#include <btstack_run_loop.h>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <uni.h>
#include "global_vars.h"
#include "sdkconfig.h"

#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Pico W must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

float speed = 0;
float speedLeft = 0;
float speedRight = 0;

bool Boost2 = false;
bool Boost2Toggle = false;

const float Boost1Multiplier = 1.3;
const float Boost2Multiplier = 1.3;

struct uni_platform* get_my_platform(void);
void UpdateMotors(void);

void btstack_thread() {
    btstack_run_loop_execute();
}

void motor_thread() {
    //if (ControllerConnected) 
    UpdateMotors();
}

void UpdateMotors() {
    speed = RightTrigger;
    if (OIsDown){
        speed *= Boost1Multiplier;
    }

    if (!Boost2Toggle && TriangleIsDown){
        Boost2 = !Boost2;
    }
    Boost2Toggle = TriangleIsDown;

    if (Boost2){
        speed *= Boost2Multiplier;
    }
    
    char log_message[50]; 
    snprintf(log_message, sizeof(log_message), "Speed: %.2f\n", speed); // Changed to float for accurate display
    loge(log_message);
    if (SquareIsDown){
        loge("Square Button Pressed\n");
    }
}

int main() {
    stdio_init_all(); 
  
    if (cyw43_arch_init()) { 
        loge("Failed to initialise cyw43_arch\n");
        return -1;
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    uni_platform_set_custom(get_my_platform());
    uni_init(0, NULL);

    //multicore_launch_core1(btstack_thread);
    //motor_thread();
    btstack_thread();

    return 0;
}
