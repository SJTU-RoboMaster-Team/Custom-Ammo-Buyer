//
// Created by zhu_y on 2025/5/10.
//
#include "main.h"
#include "tim.h"
#include "control.h"
#include "ammo_buyer.h"

HAL_StatusTypeDef flag = HAL_ERROR;
extern AmmoBuyer ammo_buyer;

uint32_t counter; //30Hz计数器
// controlLoop为1000Hz，当按键触发时，在一次controlLoop内的buf[]中设置完所有待发送的数据帧，并启动按键保护，在该按键任务完成前无法触发其他任务
// 在30Hz的txhandle中结合offset依次发送数据帧，帧发送间缓冲33.3ms
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim6) {
        counter++;
        controlLoop();
        if (counter >= 32) {
            if (ammo_buyer.is_tx_allowed() == true) {
                ammo_buyer.txHandler();
            }
            counter = 0;
        }
    }
}
