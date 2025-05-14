//
// Created by zhu_y on 2025/5/10.
//
#include "main.h"
#include "tim.h"
#include "control.h"

HAL_StatusTypeDef flag = HAL_ERROR;

uint32_t counter; //25Hz计数器
//1000Hz
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim6) {
        counter++;
        if (counter>=40) {
            controlLoop();
            // key4.refresh_key_state_();
            // if (key4.is_rise_() == true) {
            //     key4.lock_ = true;
            //     HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
            // } else if (key4.is_fall_() == true) {
            //     key4.lock_ = false;
            //     HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
            // }
            // SetFrame();
            counter = 0;
        }

    }
}
