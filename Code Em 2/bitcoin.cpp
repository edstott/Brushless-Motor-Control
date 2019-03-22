#include "bitcoin.h"
#include "messages.h"

uint8_t sequence[] = {0x45,0x6D,0x62,0x65,0x64,0x64,0x65,0x64, 0x20,0x53,0x79,
                    0x73,0x74,0x65,0x6D,0x73, 0x20,0x61,0x72,0x65,0x20,0x66,
                    0x75,0x6E, 0x20,0x61,0x6E,0x64,0x20,0x64,0x6F,0x20, 0x61,
                    0x77,0x65,0x73,0x6F,0x6D,0x65,0x20, 0x74,0x68,0x69,0x6E,
                    0x67,0x73,0x21,0x20, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint64_t* key = (uint64_t*)((int)sequence + 48); 
uint64_t* nonce = (uint64_t*)((int)sequence + 56); 
uint8_t hash[32];

SHA256 mySHA256 = SHA256();




void computeHash(){
    Timer t;
    int count = 0;
    t.start();
    while (1) {
      
        newKey_mutex.lock();
        if(*key != newKey){
            *key = newKey;
            //putMessage(CONFIRM_BITCOIN_KEY, newKey);
        } 
        newKey_mutex.unlock();
        
        //sequence now contains the new key
        mySHA256.computeHash(hash,sequence,sizeof(sequence));
        count++;
        if (t.read() >= 1){
            t.stop();
            t.reset();
            t.start();
            putMessage((int64_t)count, HASH_RATE); 
            count = 0;
        }
        if((hash[0] == 0) && (hash[1] == 0)){
            putMessage(*nonce, NONCE);       
        }
        
        *nonce+=1;
    }
}
