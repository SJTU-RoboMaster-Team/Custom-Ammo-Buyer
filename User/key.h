//
// Created by zhu_y on 2025/5/12.
//

#ifndef KEY_H
#define KEY_H
#include "main.h"

class KEY {
    GPIO_TypeDef *key_port_;
    uint16_t key_gpio_pin_;

    GPIO_TypeDef *led_port_;
    uint16_t led_gpio_pin_;

    GPIO_PinState led_state_;
    // bool lock_;

    typedef enum KEYstate {
        IDLE,
        BUSY
    } KeyState;

    typedef enum ERROR_CODE {
        OK,
        MISMATCH,
        ERR
    }Error_Status;
    Error_Status error_status_;

    KeyState key_state_;
    KeyState last_key_state_;
public:
    KEY(GPIO_TypeDef *KEY_GPIOx, uint16_t KEY_GPIO_Pin,
        GPIO_TypeDef *LED_GPIOx, uint16_t LED_GPIO_Pin): key_port_(KEY_GPIOx), key_gpio_pin_(KEY_GPIO_Pin),
                                                         led_port_(LED_GPIOx), led_gpio_pin_(LED_GPIO_Pin) {
        key_state_ = IDLE;
        led_state_ = GPIO_PIN_RESET;
        // lock_ = false;
    }

    void key_state_refresh_();
    bool check_status();
    bool is_rise_();
    bool is_fall_();
    void errorHandler();

    void set_state_() {
        HAL_GPIO_WritePin(led_port_, led_gpio_pin_, led_state_);
    }

    void switch_state_() {
        if (led_state_ == GPIO_PIN_RESET) {
            led_state_ = GPIO_PIN_SET;
        } else {
            led_state_ = GPIO_PIN_RESET;
        }
        set_state_();
    }
};
#endif //KEY_H
