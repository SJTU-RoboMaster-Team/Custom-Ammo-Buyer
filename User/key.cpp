//
// Created by zhu_y on 2025/5/12.
//
#include "key.h"

KEY key1(KEY1_GPIO_Port, KEY1_Pin,LED1_GPIO_Port, LED1_Pin); //reserve
KEY key2(KEY2_GPIO_Port,KEY2_Pin,LED2_GPIO_Port, LED2_Pin);
KEY key3(KEY3_GPIO_Port,KEY3_Pin,LED3_GPIO_Port, LED3_Pin);
KEY key4(KEY4_GPIO_Port,KEY4_Pin,LED4_GPIO_Port, LED4_Pin);

KEY key_array[4] = {key1, key2, key3, key4};

bool KEY::check_status() {
    if ((KeyState)HAL_GPIO_ReadPin(key_port_, key_gpio_pin_) == key_state_) {
        error_status_ = OK;
        return true;
    }
    error_status_ = MISMATCH;
    return false;
}

void KEY::errorHandler() {
       key_state_ = (KEYstate)HAL_GPIO_ReadPin(key_port_, key_gpio_pin_);
}

bool KEY::is_rise_() {
    if(last_key_state_ == IDLE && key_state_ == BUSY) return true;
    return false;
}

bool KEY::is_fall_() {
    if(last_key_state_ == BUSY && key_state_ == IDLE) return true;
    return false;
}

void KEY::key_state_refresh_() {
    last_key_state_ = key_state_;
    key_state_ = (KEYstate)HAL_GPIO_ReadPin(key_port_, key_gpio_pin_);
}
