#include "motor.h"


//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};
 
//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};  
//const int8_t stateMap[] = {0x07,0x01,0x03,0x02,0x05,0x00,0x04,0x07}; //Alternative if phase order of input or drive is reversed

//PWMout class to control PWMpin 
PwmOut pin1(PWMpin);
 
//Phase lead to make motor spin
volatile int8_t lead = 2;  //2 for forwards, -2 for backwards

//Rotor offset at motor state 0
int8_t orState = 0;

//Motor position
volatile double motorPosition = 0;

volatile double motorTorque = 0;

volatile double tmpPosition = 0.0; 



//motor position and velocity
float v = 0.0;
float position = 0.0;
float rError = 0.0;
float oldError = 0.0;
float T = 0.0;
float vControl = 0.0;
float rControl = 0.0;
float diffvError = 0.0;
float vError = 0.0;
//float oldvError = 0.0;
float intvError = 0.0;
float rDiff = 0.0;

//timer to calculate velocity more acurately
Timer t_vel;

//counter for velocity iterations
int counter = 0;

//Status LED
DigitalOut led1(LED1);
 
//Photointerrupter inputs
InterruptIn I1(I1pin);
InterruptIn I2(I2pin);
InterruptIn I3(I3pin);
 
//Motor Drive outputs
DigitalOut L1L(L1Lpin);
DigitalOut L1H(L1Hpin);
DigitalOut L2L(L2Lpin);
DigitalOut L2H(L2Hpin);
DigitalOut L3L(L3Lpin);
DigitalOut L3H(L3Hpin);

extern void setUpISR(){
        
    I1.rise(&ISR); //rise or fall and others etc
    I2.rise(&ISR);
    I3.rise(&ISR);
    
    I1.fall(&ISR); //rise or fall and others etc
    I2.fall(&ISR);
    I3.fall(&ISR);
    
}
//Set a given drive state
void motorOut(int8_t driveState){
    
    //Lookup the output byte from the drive state.
    int8_t driveOut = driveTable[driveState & 0x07];
    
    //Turn off first
    if (~driveOut & 0x01) L1L = 0;
    if (~driveOut & 0x02) L1H = 1;
    if (~driveOut & 0x04) L2L = 0;
    if (~driveOut & 0x08) L2H = 1;
    if (~driveOut & 0x10) L3L = 0;
    if (~driveOut & 0x20) L3H = 1;

    //Then turn on
    if (driveOut & 0x01) L1L = 1;
    if (driveOut & 0x02) L1H = 0;
    if (driveOut & 0x04) L2L = 1;
    if (driveOut & 0x08) L2H = 0;
    if (driveOut & 0x10) L3L = 1;
    if (driveOut & 0x20) L3H = 0;
}

void initialSetup(){
    pin1.period(0.002f);
    pin1.write(1.0f);
}
    
void motorCtrlFn(){
    motorHome();
    Ticker motorCtrlTicker; 
    motorCtrlTicker.attach_us(&motorCtrlTick,100000); 
    float oldPosition = 0.0;
    
    double tVel = 0.0;
    t_vel.start();   
    while(1){
        oldPosition = ((float)motorPosition)/6.0;  
        motorCtrlT.signal_wait(0x1);    

        core_util_critical_section_enter();
        tmpPosition = motorPosition;
        core_util_critical_section_exit();
        t_vel.stop();
        tVel = t_vel.read();
        t_vel.reset();
        t_vel.start();
        v = (1/tVel)*((tmpPosition/6.0) - oldPosition);
        position =  ((float)tmpPosition/6.0);
        
        if(spinForever){
            if(newSpeed_entered) T = velocityControl();
            else T = maxPWM;
        }
        else if(newSpeed_entered && !newRotation_entered){
            T = velocityControl();
        }
        else if(newRotation_entered && !newSpeed_entered){
            T = positionControl();
        }
        else if(newRotation_entered && newSpeed_entered){
            vControl = velocityControl();
            rControl = positionControl();
            if(v >= 0){
                if(vControl >= rControl){
                    T = rControl;
                }
                else{
                    T = vControl;
                }
            }
            else{
                if(vControl <= rControl){
                    T = rControl;
                }
                 else{
                    T = vControl;
                }
              }
                
            
        }
        pin1.write(T);
            
        //every 1s outputs measurement
        if(counter == 9){
            putMessage(*(int64_t*)&v, MOT_VELOCITY);
            putMessage(*(int64_t*)&position, MOT_POSITION);
            counter = 0;
        }
    
        else counter++;
    }
   
}

//Interrupt routine - when the input changes motor output changes accordingly
void ISR(){
  static int8_t oldRotorState = 0;
  int8_t intState = 0;
  intState = readRotorState();
  motorOut((intState-orState+lead+6)%6); //+6 to make sure the remainder is positive
  if (intState - oldRotorState == 5) motorPosition--;
  else if (intState - oldRotorState == -5) motorPosition++;
  else motorPosition += (intState - oldRotorState);
  oldRotorState = intState;
}

//Convert photointerrupter inputs to a rotor state
inline int8_t readRotorState(){
    return stateMap[I1 + 2*I2 + 4*I3];
}
 
//Basic synchronisation routine    
int8_t motorHome() {
    //Put the motor in drive state 0 and wait for it to stabilise
    motorOut(0);
    wait(2.0);
    
    //Get the rotor state
    return readRotorState();
}

void motorCtrlTick(){ 
    motorCtrlT.signal_set(0x1); 
    
}

float velocityControl(){
    
    float Ts;
    if(newV == 0) Ts = maxPWM;
    else{
        if (newV <= 0){
            lead = -2;   
        }
        else{
            lead = 2;   
        }
        
        //error term
        vError = newV - abs(v);
        
        //Integral error
        intvError += vError/0.1;
        if(intvError > 880) intvError = 880;
        if(intvError < -880) intvError = -880;
        
        Ts = Kps*vError + Kis*intvError;
       
        if(Ts > maxPWM){
           Ts = maxPWM;   
        }
    }
    return Ts;
}

float positionControl(){
    float Tr;
    rError = targetPosition - motorPosition/6.0;
    rDiff = rError - oldError;
    oldError = rError;
    Tr = (Kpr*rError + Kdr*rDiff);
    lead = (Tr >= 0) ?  2 : -2;
    Tr = abs(Tr);
    if(Tr > maxPWM){
        Tr = maxPWM;   
    }

    return Tr;
   }  
