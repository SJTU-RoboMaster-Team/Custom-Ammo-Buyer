//
// Created by zhu_y on 2025/5/14.
//
#include "ammo_buyer.h"
#include "config.h"

extern KEY key_array[4];
AmmoBuyer ammo_buyer(&huart2, key_array, NUMBER_OF_KEYS);

//仅首次按键触发时，会进行发送任务发送
bool AmmoBuyer::keyMonitor() {
    for (uint32_t i = 0; i < NUMBER_OF_KEYS; i++) {
        if (keys_[i].is_rise_() == true) {
            return true;
        }
    }
    return false;
}

AmmoBuyer::Task_t AmmoBuyer::Find_task() {
    for (uint32_t i = 0; i < NUMBER_OF_KEYS; i++) {
        if (keys_[i].is_rise_() == true) {
            if (i == 0) {
#ifdef HERO
                return THIRTY42;
#elifdef INFANTRY
                return SET_STATUS;
#endif
            } else if (i == 1) {
                return REMOTE_BLOOD;
            } else if (i == 2) {
                return REMOTE_AMMO;
            } else if (i == 3) {
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
    memcpy(tx_.tx_buf_ + tx_.pack_size_, tx_.data_package_ + tx_.transmitted_data_len_, tx_.frame_.header.data_len);
    tx_.pack_size_ += tx_.frame_.header.data_len;
    tx_.transmitted_data_len_ += tx_.frame_.header.data_len;
    //tail
    tx_.pack_size_ += sizeof(tx_.frame_.crc16);
    CRC16_Append(tx_.tx_buf_, tx_.pack_size_);

    HAL_UART_Transmit(&huart2, tx_.tx_buf_, tx_.pack_size_, 100);

    // 所有数据发送完成，关闭保护，重启keyMonitor，等待下一次触发
    if (tx_.transmitted_data_len_ == tx_.number_of_steps_ * DATA_LEN) {
        lock_ = false;
        prohibit_tx();
    }
}

AmmoBuyer::Task_t task_type;

void AmmoBuyer::handle() {
    task_type = ammo_buyer.Find_task();
    if (task_type != ERROR) {
        //txhandler任务，在此处将所有待发送帧数据载入buf
        if (task_type == REMOTE_BLOOD) {
            tx_.number_of_steps_ = 6;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                // 打开面板
                {'H', 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0},
                // ctrl+1
                {0x11, 0x31, 0, 0, 0, 0, 0}, //ctrl 0x11， 1 0x31
                {0, 0, 0, 0, 0, 0, 0},
                // yes
                {'Y', 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0},

            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);
        } else if (task_type == REMOTE_AMMO) {
            tx_.number_of_steps_ = 6;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                // 打开面板
                {'H', 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0},
                // ctrl+2
                {0x11, 0x32, 0, 0, 0, 0, 0}, //ctrl 0x11， 2 0x32
                {0, 0, 0, 0, 0, 0, 0},
                // yes
                {'Y', 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0},

            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);
        } else if (task_type == FIFTY17) {
            tx_.number_of_steps_ = 10;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                // 打开面板+十发
                {'O', 0, 1547, 0, 867, 0, 0},
                {0, 0, 1547, 1, 867, 0, 0},
                {0, 0, 1547, 0, 867, 0, 0},
                // 确定
                {0, 0, 1289, 0, 993, 0, 0},
                {0, 0, 1289, 1, 993, 0, 0},
                {0, 0, 1289, 0, 993, 0, 0},
                // 是+关闭面板
                {0, 0, 1207, 0, 857, 0, 0},
                {0, 0, 1207, 1, 857, 0, 0},
                {'O', 0, 1207, 0, 857, 0, 0},
                {0, 0, 1207, 0, 857, 0, 0},
            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);
        } else if (task_type == TEN42) {
            tx_.number_of_steps_ = 10;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                // 打开面板+十发
                {'O', 0, 1629, 0, 867, 0, 0},
                {0, 0, 1629, 1, 867, 0, 0},
                {0, 0, 1629, 0, 867, 0, 0},
                // 确定
                {0, 0, 1289, 0, 993, 0, 0},
                {0, 0, 1289, 1, 993, 0, 0},
                {0, 0, 1289, 0, 993, 0, 0},
                // 是+关闭面板
                {0, 0, 1207, 0, 857, 0, 0},
                {0, 0, 1207, 1, 857, 0, 0},
                {'O', 0, 1207, 0, 857, 0, 0},
                {0, 0, 1207, 0, 857, 0, 0},
            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);
        } else if (task_type == THIRTY42) {
            tx_.number_of_steps_ = 16;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                // 打开面板+十发
                {'O', 0, 1629, 0, 867, 0, 0},
                {0, 0, 1629, 1, 867, 0, 0},
                {0, 0, 1629, 0, 867, 0, 0},
                // 一发
                {0, 0, 1400, 0, 867, 0, 0},
                {0, 0, 1400, 1, 867, 0, 0},
                {0, 0, 1400, 0, 867, 0, 0},
                // 两发
                {0, 0, 1472, 0, 867, 0, 0},
                {0, 0, 1472, 1, 867, 0, 0},
                {0, 0, 1472, 0, 867, 0, 0},
                // 确定
                {0, 0, 1289, 0, 993, 0, 0},
                {0, 0, 1289, 1, 993, 0, 0},
                {0, 0, 1289, 0, 993, 0, 0},
                // 是+关闭面板
                {0, 0, 1207, 0, 857, 0, 0},
                {0, 0, 1207, 1, 857, 0, 0},
                {'O', 0, 1207, 0, 857, 0, 0},
                {0, 0, 1207, 0, 857, 0, 0},
            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);
        } else if (task_type == SET_STATUS) {
            tx_.number_of_steps_ = 12;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                // 客户端中，选中底盘/发射机构类型后，选择界面会缓慢消失，在30Hz实时响应中容易误触未完全消失的界面
                // 打开面板+发射机构类型
                {'P', 0, 1090, 0, 730, 0, 0},
                {0, 0, 1090, 1, 730, 0, 0},
                {0, 0, 1090, 0, 730, 0, 0},
                // 爆发优先
                {0, 0, 1090, 0, 846, 0, 0},
                {0, 0, 1090, 1, 846, 0, 0},
                {0, 0, 1090, 0, 846, 0, 0},
                 //底盘类型
                {0, 0, 1090, 0, 670, 0, 0},
                {0, 0, 1090, 1, 670, 0, 0},
                {0, 0, 1090, 0, 670, 0, 0},
                // 血量优先
                {0, 0, 1090, 0, 780, 0, 0},
                {0, 0, 1090, 1, 780, 0, 0},
                {0, 0, 1090, 0, 780, 0, 0},
                // 不关闭面板
            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);
        }

        tx_.allow_tx_ = true; //允许通讯
        lock_ = true; // 找到任务，触发保护
        tx_.transmitted_data_len_ = 0;
    }
}

/*
Task Code. CV after: (else) if (task_type == TEN42) {…}

1. Image Transmission +Converse:
            tx_.number_of_steps_ = 8;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                {'P', 0, 1135, 0, 926, 0, 0},
                {0, 0, 1135, 1, 926, 0, 0},
                {0, 0, 1135, 0, 926, 0, 0},
                {0, 0, 1135, 0, 989, 0, 0},
                {0, 0, 1135, 1, 989, 0, 0},
                {0, 0, 1135, 0, 989, 0, 0},
                {'P', 0, 1135, 0, 989, 0, 0},
            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);

2. Image Transmission -Converse:
            tx_.number_of_steps_ = 8;
            custom_client_data_t data_[MAX_STEP_NUM] = {
                {'P', 0, 1135, 0, 926, 0, 0},
                {0, 0, 1135, 1, 926, 0, 0},
                {0, 0, 1135, 0, 926, 0, 0},
                {0, 0, 1135, 0, 989, 0, 0},
                {0, 0, 1135, 1, 989, 0, 0},
                {0, 0, 1135, 0, 989, 0, 0},
                {'P', 0, 1135, 0, 989, 0, 0},
            };
            memcpy(tx_.data_package_, &data_, tx_.number_of_steps_ * DATA_LEN);

*/
