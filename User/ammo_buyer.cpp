//
// Created by zhu_y on 2025/5/14.
//
#include "ammo_buyer.h"
#include "config.h"

extern KEY key_array[4];
AmmoBuyer ammo_buyer(&huart2, key_array, 4);

//仅首次按键触发时，会进行发送任务发送
bool AmmoBuyer::keyMonitor() {
    for (uint32_t i = 0; i < num_of_keys_; i++) {
        if (keys_[i].is_rise_()==true) {
            return true;
        }
    }
    return false;
}

AmmoBuyer::Task_t AmmoBuyer::Find_task() {
    for (uint32_t i = 0; i < num_of_keys_; i++) {
        if (keys_[i].is_rise_()==true) {
            if (i==1) { //reserve
            }else if(i==2) {
              return REMOTE_BLOOD;
            }else if(i==3) {
              return REMOTE_AMMO;
            }else if(i==4) {
#ifdef HERO
              return TEN42;
#elifdef INFANTRY
              return FIFTY17;
#endif
            }
        }
    }
    return ERROR;
}

//TODO
//修改适配lock，在内部添加已发送数据长度作为偏置，逻辑同pack_size_
void AmmoBuyer::txHandler() {
    //header
    tx_.frame_.header.sof = SOF;
    tx_.frame_.header.data_len = DATA_LEN;
    tx_.frame_.header.seq++;
    memcpy(tx_.tx_buf_, &tx_.frame_.header, sizeof(tx_.frame_.header) - 1);
    tx_.pack_size_ = sizeof(tx_.frame_.header);
    CRC8_Append(tx_.tx_buf_, tx_.pack_size_);

    //cmd_id
    tx_.frame_.cmd_id = Custom_Client_ID;
    memcpy(tx_.tx_buf_ + tx_.pack_size_, &tx_.frame_.cmd_id, sizeof(tx_.frame_.cmd_id));
    tx_.pack_size_ += sizeof(tx_.frame_.cmd_id);

    //data
    tx_.pack_size_ += tx_.frame_.header.data_len;
    //tail
    tx_.pack_size_ += sizeof(tx_.frame_.crc16);
    CRC16_Append(tx_.tx_buf_, tx_.pack_size_);

    HAL_UART_Transmit(&huart2, tx_.tx_buf_, tx_.pack_size_, 100);

    tx_.transmitted_data_len_ += DATA_LEN;
    // 所有数据发送完成，关闭保护，重启keyMonitor，等待下一次触发
    if(tx_.transmitted_data_len_ == tx_.target_data_len_) {
        lock_ = false;
    }
}

void AmmoBuyer::handle() {
    Task_t task_type;
    task_type = ammo_buyer.Find_task();
    if (task_type != ERROR) {
        //txhandler任务，在此处将所有待发送帧数据载入buf
        if (task_type == REMOTE_AMMO) {

        }else if (task_type == REMOTE_BLOOD) {

        }else if (task_type == FIFTY17) {

        }else if (task_type == TEN42) {

        }
        lock_ = true;// 找到任务，触发保护
        tx_.transmitted_data_len_ = 0;
    }
}

