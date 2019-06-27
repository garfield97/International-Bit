# International:bit
![62184062_305163543720234_4359735231024988160_n](https://user-images.githubusercontent.com/33202105/60281321-a8a3d300-98fc-11e9-84c8-6a7891ec31eb.jpg)


We are the international:bit, a project on behalf of the BBC micro:bit to make an extension board that will support the display of non-latin characters. The extension board includes a 12x12 LED matrix with the relevant display driver chip as well as memory extension modules in order to fit in extensive bitmap font libraries for the different languages we wish to implement. The repo contains all the software libraries and drivers in order to get the extension board running with the microbit through ARM mbed. To learn more about the project, here is a link to the report.

## Getting Started

This respostiroy has various things including:
 - Source code for the display driver/functions in the relevant directory
 - Program to parse in the BDF format into C++ readable format
 - Tools for compression and analysis
 
The main source code is an ARM mbed C++ program that needs the mbed compiler to build and compile in order to generate a hex file for the micro:bit to run.

## How to use 
To use the source code, insert the relevant include file to the header program that contains all the functions to support the extension in your main program. Then, have all the relevant codes available in the indicated paths and compile via the mbed compiler. It should generate the correct hex file for the micro:bit to run.

## Prerequisites
You need access to ARM mbed online compiler along with the micro:bit and the extension board.

## How to parse in the right bitmap libraries to map onto the LED matrix display
This C++ program aims to alter the Zpix library from BDF form to a form that ARM Mbed code(C++) can understand.
Here come the steps to use the parsing program:

1. Input the content in Zpix.bdf to bdf.txt so that it can be read by C++.
2. This parsing program has two modes which can be controlled by switching the global boolean variable "compareMode".
3. When "compareMode" is false, which means compare mode is off. The program just operates normally to generate bitmaps for Latin characters and all Chinese characters in order. The output bitmaps are stored in 'mapping.txt'.
4. When "compareMode" is true, which means compare mode is on. The program firstly generates the bitmaps for all Latin characters. Then, for Chinese characters, the program only generates the bitmaps for those Chinese characters whose unicodes are listed in 'common.txt'(unicodes must be listed in an ascending order). The output bitmaps are stored in 'mapping.txt'.
5. The outputs stored in 'mapping.txt' are in a compressed form. Each character takes 18 bytes.


## How to use the compression analysis tool
The program is for finding out 3 important information from the data sequence:
1. Total size of the data sequence
2. Frequencies of discrete values in the data sequence
3. Information entropy of the data sequence

A byte of data is read in when '0x' is detected as each byte of data is prefixed with '0x' in the bitmap.
The bytes of data are stored in temp_bit until it reaches the length specified.
temp_bit is a stringstream and will need to be converted into an integer.
Then the frequency of the corresponding integer value will increment by 1.
The frequencies of all discrete values wil be stored in the array freq[].
The total size of the data sequence can be found by adding up all the frequencies.
Information entropy will be calculated from the frequencies by the formula 


<img width="161" alt="Screen Shot 2019-06-27 at 19 29 08" src="https://user-images.githubusercontent.com/33202105/60291058-068ee580-9912-11e9-8f04-890817b776fa.png">


By altering the value of variable length, it can check the discrete values with different byte length, e.g. length = 1 represents measuring frequencies of 1-Byte values.
The frequencies of discrete values are stored in the array freq[].

File "aug_bitmap_full.txt" needs to be included in the same directory as the program.


## Licensing
Everything is built upon the BBC micro:bit and subject to the same copyright licensing standards(https://github.com/bbcmicrobit). All the software is open licensed except for the zpix font used which is subject to their own licensing policies outlined in their repository (https://github.com/SolidZORO/zpix-pixel-font).

![patrixkjun](https://user-images.githubusercontent.com/33202105/60281225-73978080-98fc-11e9-9789-9e029f930563.jpg)


