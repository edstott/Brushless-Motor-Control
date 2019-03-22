#ifndef _messages_h_
#define  _messages_h_

#include "mbed.h"
#include "rtos.h"
#include "decode.h"

#define HASH_RATE       1
#define NONCE           2
#define TUNE            3
#define KEY             4
#define MOT_VELOCITY    5
#define MOT_POSITION    6
#define VELOCITY        7
#define ROTATION        8
#define CONFIRM_BITCOIN_KEY 9
#define TARGET          10


/* Mail */
typedef struct {
    uint32_t val;
    uint8_t type;

}mail_t;

void output();
extern void putMessage(int32_t val, uint8_t type);

#endif
