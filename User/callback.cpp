//
// Created by zhu_y on 2025/5/10.
//
#include "main.h"
#include "tim.h"
#include "control.h"
#include "ammo_buyer.h"

HAL_StatusTypeDef flag = HAL_ERROR;
extern AmmoBuyer ammo_buyer;

uint32_t counter; //25Hz计数器
// controlLoop为1000Hz，当按键触发时，在一次controlLoop内的buf[]中设置完所有待发送的数据帧，并启动按键保护，在该按键任务完成前无法触发其他任务
// 在25Hz的txhandle中结合offset依次发送数据帧，帧发送间缓冲40ms
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim6) {
        counter++;
        controlLoop();
        if (counter>=40) {
            ammo_buyer.txHandler();
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
