#ifndef EXTERNAL_DISPLAY_H
#define EXTERNAL_DISPLAY_H
#include "m.h"
#include "i2c_setup.h"

const uint8_t FIXED_SIZE = 18;
char data[24];
char dataFake[24];
enum scrollingDirection {NONE, LEFT, RIGHT, DOWN, UP};
//int halfScreenElem[] = {0x4E61,0x535C};
//reverse a 6-bit number
char reverse(char x)
{
    char c = x;
    char rev;
    int count = 8;
    while(c)
    {
        rev = rev << 1;
        rev |= c & 1;
        c = c >> 1;
        count--;
    }
    return (rev << count)>>2;    
}
// convert utf8 to unicode
void deleteArray(int original[], int origLen, int newArray[], int index){

    for(int i = 0; i < index; i++)
        newArray[i] = original[i];    
    for(int i = index; i < origLen-1; i++)
        newArray[i] = original[i+1];        
}

int count_hex(int num){
    int temp, i = 1, j, r;
    int counter = -1;
    char hex[50];
    temp = num;
    while (temp != 0)
    {
        r = temp % 16;
        if (r < 10)
            hex[i++] = r + 48;
        else
            hex[i++] = r + 55;
        temp = temp / 16;
    }
    for (j = i; j > 0; j--)
        counter++;
    counter = (counter/2)+(counter%2);
    return counter;
}
// convert utf8 to unicode
int utf_8_array(int offset)
{
    int pairs;
    pairs = count_hex(offset);
    int offset_copy = offset;
    int bytes = 0;
    int utf8[pairs] ;
    int index = pairs - 1;
            
    while(offset_copy){
        bytes++;
        utf8[index] = (offset_copy&0xFF);
        offset_copy >>= 8;
        index--;
    } 
    int charcode = 0;
    int t = utf8[0];
    deleteArray(utf8,pairs,utf8,0);
    pairs = pairs -1;
    if (t < 128)
        return t;
    int high_bit_mask = (1 << 6) -1;
    int high_bit_shift = 0;
    int total_bits = 0;
    const int other_bits = 6;
    while((t & 0xC0) == 0xC0)
    {
        t <<= 1;
        t &= 0xff;
        total_bits += 6;
        high_bit_mask >>= 1; 
        high_bit_shift++;
        charcode <<= other_bits;
        charcode |= utf8[0] & ((1 << other_bits)-1);
        deleteArray(utf8,pairs,utf8,0);
        pairs = pairs - 1;
    } 
    charcode |= ((t >> high_bit_shift) & high_bit_mask) << total_bits;
    return charcode;
}
void writeLEDs(){
    memLoc ledReg = {PAGE0, 0x00, 0x00};
    for(int i=11; i>=0; i--){
      ledReg.reg = i*2+1; 
      ledReg.val = (int)data[23-ledReg.reg];//24
      setReg2(ledReg);
      ledReg.reg = i*2;
      ledReg.val = (int)data[23-ledReg.reg];//22
      setReg2(ledReg);
    }
}
void clearDisplay(int delay){
    for(int y=0; y<24; y++)
        data[y] = 0x0;
    writeLEDs();
    wait_ms(delay);
}

void allDisplay(int delay){
    for(int y=0; y<24; y++)
        data[y] = 0x3f;
    writeLEDs();
    wait_ms(delay);
}
// print to the display by modifying the on/off registers
void updateDisplay(int inputChar, int scroll, int delay=5000)
{
    int offset = inputChar;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
    offset = utf_8_array(offset);
    if(offset > 0xA8)
        offset = offset - 0x4E00 + 0x67;
    else if(offset > 0xA0)
        offset = offset - 0xA1 + 0x5F;
    else
        offset = offset - 0x20;
    // get the positon of the char in the font array
    offset = offset * FIXED_SIZE;
    int idx = 0;    
    
    if(scroll == NONE)
    {   while(idx < 6){
        data[idx*4] = (font[idx*3+offset]&0xFC)>>2;
        data[idx*4+1] = (font[idx*3+offset]&0x03)<<4 | (font[idx*3+1+offset]&0xF0)>>4;
        data[idx*4+2] = (font[idx*3+1+offset]&0x0F)<<2 | (font[idx*3+2+offset]&0xC0)>>6;
        data[idx*4+3] = font[idx*3+2+offset]&0x3F;
        
//       data[idx*4] = reverse((font[idx*3+offset]&0xFC)>>2);
//        data[idx*4+1] = reverse((font[idx*3+offset]&0x03)<<4 | (font[idx*3+1+offset]&0xF0)>>4);
//        data[idx*4+2] = reverse((font[idx*3+1+offset]&0x0F)<<2 | (font[idx*3+2+offset]&0xC0)>>6);
//        data[idx*4+3] = reverse(font[idx*3+2+offset]&0x3F);
        idx++;
    }
        writeLEDs();
        wait_ms(delay);
        clearDisplay(20);
    }
    else 
    {
        while (idx < 6){
        dataFake[idx*4] = (font[idx*3+offset]&0xFC)>>2;
        dataFake[idx*4+1] = (font[idx*3+offset]&0x03)<<4 | (font[idx*3+1+offset]&0xF0)>>4;
        dataFake[idx*4+2] = (font[idx*3+1+offset]&0x0F)<<2 | (font[idx*3+2+offset]&0xC0)>>6;
        dataFake[idx*4+3] = font[idx*3+2+offset]&0x3F;
        
//        dataFake[idx*4] = reverse((font[idx*3+offset]&0xFC)>>2);
//        dataFake[idx*4+1] = reverse((font[idx*3+offset]&0x03)<<4 | (font[idx*3+1+offset]&0xF0)>>4);
//        dataFake[idx*4+2] = reverse((font[idx*3+1+offset]&0x0F)<<2 | (font[idx*3+2+offset]&0xC0)>>6);
//        dataFake[idx*4+3] = reverse(font[idx*3+2+offset]&0x3F);
        idx++;
        }
    idx = 12;
    while(scroll && idx > 0){
        for(int i = 11; i>=0; i--){
            if(scroll == LEFT){
                data[i*2] = ((data[i*2]<<1)&0x3f) | ((data[i*2+1]&0x20)>>5);
                data[i*2+1] = (data[i*2+1]<<1)&0x3f | ((dataFake[i*2]&0x20)>>5);// & ((data[idx*2]&0x1)<<5);
                dataFake[i*2] = ((dataFake[i*2]<<1)&0x3f) | ((dataFake[i*2+1]&0x20)>>5);
                dataFake[i*2+1] = (dataFake[i*2+1]<<1)&0x3f;
            }
            if(scroll == RIGHT){
                data[i*2+1] = (data[i*2+1]>>1) | ((data[i*2]&0x1)<<5);
                data[i*2] = (data[i*2]>>1) | ((dataFake[i*2]&0x1)<<5);
                dataFake[i*2] = ((dataFake[i*2]>>1)&0x3f) | ((dataFake[i*2+1]&0x20)<<5);
                dataFake[i*2+1] = (dataFake[i*2+1]>>1)&0x3f;
            }
            if(scroll == UP){
                if(i > 0){
                    data[(11-i)*2+1] = data[(11-i)*2+3];
                    data[(11-i)*2] = data[(11-i)*2+2];
                    dataFake[(11-i)*2+1] = dataFake[(11-i)*2+3];
                    dataFake[(11-i)*2] = dataFake[(11-i)*2+2];
                }
                else{
                    data[22] = dataFake[0];
                    data[23] = dataFake[1];
                    dataFake[22] = 0;
                    dataFake[23] = 0;                    
                }
            }
            if(scroll == DOWN){
                if(i > 0){
                    data[i*2+1] = data[i*2-1];
                    data[i*2] = data[i*2-2];
                    dataFake[i*2+1] = dataFake[i*2-1];
                    dataFake[i*2] = dataFake[i*2-2];
                }
                else{
                    data[0] = dataFake[22];
                    data[1] = dataFake[23];
                    dataFake[0] = 0;
                    dataFake[1] = 0;
                }            
            }
        }
        idx--;
        writeLEDs();
        wait_ms(delay);
    }
    }
}
void printString(char* str,int delay=500)
{
    int size = strlen(str);
    int i = 0;
    while(i < size)
    {   
        int x = str[i];
        updateDisplay(x,NONE,delay);
        i++;
    }
}
void scrollString(char* str, int direction, int delay=50)
{
    int size = strlen(str);
    int i = 0;
    while(i < size)
    {   
        int x = str[i];
        updateDisplay(x,direction,delay);
        i++;
    }
}
void printChineseChars(int* str,int size,int delay=500)
{
    int i = 0;
    while(i < size)
    {   
        int x = str[i];
        updateDisplay(x,NONE,delay);
        i++;
    }
}
#endif 
