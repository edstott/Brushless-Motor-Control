#include "mbed.h"
#include "messages.h"
#include "bitcoin.h"
#include "decode.h"
#include "motor.h"
#include "Crypto.h"
#include "rtos.h"
#include <stdio.h>


Thread outputMessagesT(osPriorityNormal,1024);
Thread decodeCommandT(osPriorityNormal,1024);
Thread motorCtrlT(osPriorityHigh,1024);

int main() {
    
    initialSetup();
    
    setUpISR();

    
    //Initialise all threads
    outputMessagesT.start(output);
    decodeCommandT.start(decode);
    motorCtrlT.start(motorCtrlFn);

    //void tune();
    //Run the motor synchronisation
    orState = motorHome();
    
    computeHash();
}
/*
A=440
A#/B^=466.16
B=493.88
C=523.25.37
C#/D^=554.37
D=587.33
D#/E^=622.25
E=659.25
F=698.46
F#/G^=739.99
G=783.99
G#=830.61
*/
