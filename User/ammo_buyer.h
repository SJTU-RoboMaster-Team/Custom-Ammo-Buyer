//
// Created by zhu_y on 2025/5/14.
//

#ifndef AMMO_BUYER_H
#define AMMO_BUYER_H
#include <cstring>
#include "crc.h"
#include "key.h"
#include "main.h"
#include "usart.h"

#define FRAME_LEN 17  // 5+2+8+2
#define DATA_LEN 8
#define SOF 0xA5
#define Custom_Client_ID 0x0306
#define CONTROLLER_TX_BUF_LEN 32
#define NUMBER_OF_KEYS 4
#define DATA_LEN_SUM_MAX 64 //最多8帧数据即可完成一个任务

typedef struct ClientCommFrameHeader {
  uint8_t sof;
  uint16_t data_len;
  uint8_t seq;
  uint8_t crc8;
} __packed ClientCommFrameHeader_t;

typedef struct ClientCommFrame {
  ClientCommFrameHeader_t header;
  uint16_t cmd_id;
  // uint8_t data[n];
  uint16_t crc16;
} __packed ClientCommFrame_t;

typedef struct CustomClientFrame {
  uint16_t key_value;
  uint16_t x_position : 12;
  uint16_t mouse_left : 4;
  uint16_t y_position : 12;
  uint16_t mouse_right : 4;
  uint16_t reserved;
} __packed custom_client_data_t;

class AmmoBuyer {
  UART_HandleTypeDef* huart_;
  // uint8_t locks_[NUMBER_OF_KEYS];
  uint32_t num_of_keys_;
  KEY* keys_;
  bool lock_;
  uint32_t offset_;

  struct {
    ClientCommFrame_t frame_;
    uint32_t pack_size_;
    uint8_t tx_buf_[CONTROLLER_TX_BUF_LEN];
    uint32_t target_data_len_;
    uint32_t transmitted_data_len_;
    uint8_t data_package_[DATA_LEN_SUM_MAX];
  } tx_;

  typedef enum state { IDLE, BUSY } KeyState;
  KeyState state_;

 public:
  typedef enum TASK {
    REMOTE_AMMO, //key3
    REMOTE_BLOOD, //key2
    FIFTY17, //key4
    TEN42, //key4
    ERROR
  } Task_t;
  Task_t task_;

  AmmoBuyer(UART_HandleTypeDef* huart, KEY keys[], uint32_t num)
      : huart_(huart), keys_(keys), num_of_keys_(num) {
    lock_ = false;
  }

  bool keyMonitor();
  Task_t Find_task();
  void refresh_status() {
    for (uint32_t i = 0; i < NumberOfKeys(); i++) {
      keys_[i].key_state_refresh_();
    }
  }
  uint32_t NumberOfKeys() { return num_of_keys_; }
  void txHandler();
  void handle();
  bool islocked() { return lock_; }
};

#endif  // AMMO_BUYER_H
