#ifndef I2C_SETUP_H
#define I2C_SETUP_H
#include "MicroBit.h"
MicroBitI2C* i2c = new MicroBitI2C(I2C_SDA0, I2C_SCL0);
MicroBitDisplay* display = new MicroBitDisplay();
const int ADDR = 0xA0; //-> driver chip //0xA2 -> first half of memory chip //0xAA -> second half of memory chip
char buffLock[] = {0xFE,0xC5};
const char PAGE0 = 0x00; 
const char PAGE1 = 0x01;
const char PAGE2 = 0x02;
const char PAGE3 = 0x03;
char currentPage = 0;
bool err;

// struct that contains the page number, register address and value of a register
struct memLoc{
   char page;
   int reg;
   int val;
};
// set the Unlock Command Register before trying to change the value in the Command Register    
void unlockCommReg(){
    i2c->write(ADDR,buffLock,2);   
}
// set the Command Register to point to the correct page
void setCommReg(char page){
    unlockCommReg();
    char buffPage[] = {0xFD,page};
    currentPage = page;
    i2c->write(ADDR,buffPage,2);
}
// set the value of the register in loc.reg on page: loc.page to value: loc.val 
void setReg2(memLoc loc){
    if(currentPage != loc.page)
        setCommReg(loc.page);
    char buff[]={loc.reg,loc.val};
    if(i2c->write(ADDR,buff,2)!=MICROBIT_OK)
        err = true;
}
// set the value of the configuration register
void setConfig(int val){
    memLoc configReg = {PAGE3,0x00,val};
    setReg2(configReg);
}
// set the value of the GCC register; this modifies the brightness of the whole screen
void setBrightness(int val){
    memLoc gccReg = {PAGE3,0x01,val};
    setReg2(gccReg);
}
// read and display the value of a register
void readAndDisplay(int buff){
    char bu[]={buff};
    i2c->write(ADDR,bu,1);
    i2c->read(ADDR,bu,1);
    display->scroll(bu);
}
// set the value of all the pwm registers to "pwmVal"; this modifies the brightness of the LEDs
void updatePWM(int pwmVal){
    int er;
    memLoc pwmReg = {PAGE1,0x00,0x00}; 
    for(int a=0; a<12; a++){
        for(int b=0; b<14; b++)
        {
            if(b==6 || b==7)
                b = 8;
            er = a*16+b;
            pwmReg.reg = er;
            pwmReg.val = pwmVal;
            setReg2(pwmReg); 
        }
    } 
}
// reset the values of all the registers to the default
void reset(){
    if(currentPage!=PAGE3)
        setCommReg(PAGE3);
    char bu[]={0x11};
    i2c->write(ADDR, bu, 1);
    i2c->read(ADDR, bu, 1);
}
// initialise the map used for indexing the font array
void initialise(){     
    err = false;    
    display->scroll(':');
    setConfig(0x41);
    setBrightness(0x3f);   
    updatePWM(0x3f);   
}
void checkError(){
    if(err) 
        display->print('Q');
    else
        display->print('A');
}
//void removeExtraBits(uint16_t *in, int size, bool fullScreen){
//    int i = 0;
//    if(fullScreen){
//     while(i<size){
//        if(i%2==0){
//          out[3*i/2] = (in[i]&0xFF00)>>8;
//          out[3*i/2+1] = in[i]&0xF0;
//        }
//        else{
//          out[(i-1)/2*3+1] |= (in[i]&0xF000)>>12;
//          out[(i-1)/2*3+2] = (in[i]&0xFF0)>>4;  
//        }
//        i++;
//     }  
//    }
//    else{
//        while(i<size){
//            out[i] = in[i]&0xFF;
//            i++;
//        }
//    }
//}

//void checkUnlock(){
//   char lock[] = {0xFE};
//   char buf[1];
//   if(i2c->write(ADDR,lock,1)==MICROBIT_OK){
//        if(i2c->read(ADDR,buf,1)==MICROBIT_OK && (int)buf[0]==197)
//         display->scroll('U');        
//        else 
//         display->scroll('T');
//    }        
//   else 
//        display->scroll('S');        
//}
#endif
