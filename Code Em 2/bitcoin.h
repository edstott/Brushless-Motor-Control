#ifndef _bitcoin_h_
#define  _bitcoin_h_
#include "mbed.h"
#include "SHA256.h"

void computeHash();

extern volatile uint64_t newKey;
extern Mutex newKey_mutex;

extern volatile uint32_t newTorque;
extern Mutex newTorque_mutex;

#endif

