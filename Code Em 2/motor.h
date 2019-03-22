#ifndef _motor_h_
#define  _motor_h_
#include "mbed.h"
#include "messages.h"
#include "decode.h"
#include "rtos.h"

//Photointerrupter input pins
#define I1pin D3
#define I2pin D6
#define I3pin D5
 
//Incremental encoder input pins
#define CHApin   D12
#define CHBpin   D11
 
//Motor Drive output pins   //Mask in output byte
#define L1Lpin D1           //0x01
#define L1Hpin A3           //0x02
#define L2Lpin D0           //0x04
#define L2Hpin A6           //0x08
#define L3Lpin D10          //0x10
#define L3Hpin D2           //0x20
 
#define PWMpin D9

 
//Motor current sense
#define MCSPpin   A1
#define MCSNpin   A0

//Controller
#define Kps 0.14
#define Kis 0.004
#define Kpr 0.1
#define Kdr 0.09

#define maxPWM 1.0


//Mapping from sequential drive states to motor phase outputs
/*
State   L1  L2  L3
0       H   -   L
1       -   H   L
2       L   H   -
3       L   -   H
4       -   L   H
5       H   L   -
6       -   -   -
7       -   -   -
*/



extern void setUpISR();
extern void ISR();
extern void motorOut(int8_t driveState);
extern inline int8_t readRotorState();
extern int8_t motorHome();
extern void motorCtrlTick();
extern void motorCtrlFn();
extern float velocityControl();
extern float positionControl();
extern volatile double tmpPosition; 
extern float intvError;

extern int8_t orState;

extern Thread motorCtrlT;

extern void initialSetup();

#endif