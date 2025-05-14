//
// Created by zhu_y on 2025/5/14.
//
#include "control.h"

extern AmmoBuyer ammo_buyer;

void ledTask(void) {

}

// tim6中断调用，25Hz
void controlLoop(void) {
    ammo_buyer.refresh_status();

    // monitor函数对idle状态的key进行周期性监视
    // 当监视到有按键触发时，该按键lock=true，在按键下降沿之前不在对其访问
    if(ammo_buyer.islocked()==false) {
      if (ammo_buyer.keyMonitor() == true) {
        ammo_buyer.handle();
      }
    }
    ledTask();
}

