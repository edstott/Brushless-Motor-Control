#ifndef _decode_h_
#define  _decode_h_
#include "bitcoin.h"
#include "messages.h"
#include <string>


extern volatile float newV;
extern volatile float newR;
extern volatile int targetPosition;
extern volatile bool newSpeed_entered;
extern volatile bool newRotation_entered;
extern volatile double motorPosition;
extern volatile bool spinForever;
extern RawSerial pc;

void serialISR();
void decode (void);



#endif
