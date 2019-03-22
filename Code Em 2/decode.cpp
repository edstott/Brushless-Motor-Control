#include "decode.h"
#include "bitcoin.h"
#include "motor.h"

RawSerial pc(SERIAL_TX, SERIAL_RX);



volatile float newV = 0;
volatile float newR = 0;

volatile int targetPosition = 0;
volatile bool newSpeed_entered = false;
volatile bool newRotation_entered = false;
volatile bool spinForever = false;

volatile uint64_t newKey;
volatile uint32_t newTorque;
Mutex newKey_mutex;
Mutex newTorque_mutex;
Mutex newRotation_mutex;


Queue<void, 8> inCharQ;
void serialISR(){
    uint8_t newChar = pc.getc();
    inCharQ.put((void*)newChar); 
}

int i = 0;
char message[18];

void decode (void){
    pc.attach(&serialISR);
   
    while(1){
        osEvent newEvt = inCharQ.get();
        uint8_t newChar = (uint8_t)newEvt.value.p;
        
        message[i] = newChar;
        
        
        if(newChar == '\r'){
            //message[i] = '\0';
            i = -1;
            string message_string;
            //In command decode thread
            switch(message[0]){
                case('K'):
                    newKey_mutex.lock(); 
                    sscanf(message, "K%x", &newKey);
                    newKey_mutex.unlock();
                    break;
                
                case('T'):
                    char tuner[10];
                    sscanf(message, "T%20s", &tuner);
                    //putMessage(tuner, TUNE);
                    break;
                    
                case('V'):
                    newSpeed_entered = true;
                    intvError = 0;
                    sscanf(message, "V%f", &newV);
                    putMessage(newV, VELOCITY);
                    break;
                    
                case('R'):
                    newRotation_mutex.lock();
                    newRotation_entered = true;
                    sscanf(message, "R%f", &newR);
                    putMessage(newR, ROTATION);
                    if(newR ==0){
                        spinForever = true;
                    }
                    else{
                        targetPosition = newR + (tmpPosition/6.0);
                    }
                    putMessage(targetPosition, TARGET);
                    newRotation_mutex.unlock();
                    break;
                    
                default:
                    break;
            }
        }
        i++;
        if(i >= 18) i = 0;
    }
}   
