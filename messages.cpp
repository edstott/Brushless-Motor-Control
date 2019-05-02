#include "messages.h"


Mail<mail_t, 16> mail_box;

void output() {
    
    while(true){
        osEvent Evt = mail_box.get();
        if (Evt.status == osEventMail) {
            mail_t *mail = (mail_t*)Evt.value.p;
            
            switch(mail->type){
             
                case(HASH_RATE):
                    pc.printf("\n\r Hashes per second: %d\r\n ", mail->val);
                    break;
        
                case(NONCE):
                    pc.printf("Successful nonce:%x \n\r", mail->val);
                    break;
                
                case(TUNE):
                    pc.printf("Set TUNE: %s\r\n", mail->val);
                    break;
                    
                case(KEY):
                    pc.printf("\n\r Key: ");
                    pc.printf("%x \n\r", mail->val);
                    break;
                
                case(MOT_VELOCITY):
                    pc.printf("Motor velocity: %f\r\n", *(float*)&(mail->val));
                    break;
                    
                case(MOT_POSITION):
                    pc.printf("Motor position: %f, \t", *(float*)&(mail->val));
                    break;
                
                case(VELOCITY):
                    pc.printf("Max velocity: %d \n\r", mail->val);
                    break;
                    
                case(ROTATION):
                    pc.printf("Target position: %d \n\r", mail->val);
                    break;
                    
                case(TARGET):
                    pc.printf("Target: %d \n\r", mail->val);
                    break;    
                    
                case(CONFIRM_BITCOIN_KEY):
                    pc.printf("CONFIRM KEY: 0x%x\n\r", mail->val);
                    break;
                    
                default:
                    break;
            }
            mail_box.free(mail);
        }
    }   
}

void putMessage(int32_t current_val, uint8_t type){ //value could be count or successful nonce
        mail_t *mail = mail_box.alloc();
        mail->val = current_val;
        mail->type = type;
        mail_box.put(mail);
}
