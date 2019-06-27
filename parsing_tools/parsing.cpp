#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
// #include <cstdlib> // for Windows Visual Studio

using namespace std;

bool compareMode = false; // can be turn on if only specific characters are wanted(e.g. the most common words): the list of unicodes of wanted characters must be provided in common.txt

uint16_t in[12]; // input converted full bitmap global array
uint8_t out[18]; // output compressed bitmap global array

void paddingYaxis(ifstream& inbdf, string& line, string& firstWord, int w, int h, int y); // Y-axis padding and get the bitmap
void paddingXaxis(int x); // X-axis padding
void removeExtraBits(); // To compress the data structure by removing used extra bits in bdf file.

int main(){
  bool commonIsOpen = true;

  /* ---------------- text files settings ---------------- */
  ifstream inbdf; // input file for bdf
  ofstream outmap; // output file for mapping
  // The below two are only used when compareMode is on:
  ifstream incommon; // the most common Chinese characters list
  ofstream outhash; // output file containing the Unicodes forthe most common Chinese characters

  string inbdffile = "bdf.txt";
  string outmapfile = "mapping.txt";
  // The below three are only used when compareMode is on:
  string incommonfile = "common.txt";
  string outhashfile = "hash.txt";
  string common;

  inbdf.open(inbdffile.c_str());
  outmap.open(outmapfile.c_str());

  if(compareMode){ // Only in compareMode
    incommon.open(incommonfile.c_str());
    outhash.open(outhashfile.c_str());

    if(incommon.fail()){
      cerr << "Could not open common file.\n";
      exit(1); // 1 indicates an error occurred
    }

    if(incommon.is_open() != 1){
      commonIsOpen = false;
    }
  }

  if(inbdf.fail()){
    cerr << "Could not open input file.\n";
    exit(1); // 1 indicates an error occurred
  }
  /* ----------------------------------------------------- */

  /* --------------- variables declaration --------------- */
  const int step = 18; // constant for the size of output array
  int w,h,x,y; // w: width, h: height, x: x-axis offset, y: y-axis offset

  string s_w,s_h,s_x,s_y; // string version of w, h, x, y
  string line; // a string variable to store the lines one by one from bdf file
  string firstWord; // a string variable to the first word separated by space in 'line'
  string unicode; // a string variable to store
  string secondWord; // a temp string variable to store the second word in 'line'

  bool startFlag = false; // a flag to indicate where the Latin words start
  bool latinFlag = true; // a flag to indicate where the Latin words end
  bool chineseFlag = false; // a flag to indicate where the Chinese words start
  /* ----------------------------------------------------- */

  if(inbdf.is_open() && commonIsOpen){

    if(compareMode) getline(incommon, common);

    while(getline(inbdf,line)){ // get the content from bdf file line by line and store them as string

      stringstream ssline(line); // convert the string to stream to be reused for getline for splitting operation
      getline(ssline,firstWord,' '); // split the line by the space character to extract specific elements

      if(firstWord == "CHARS") startFlag = true; // indicate where the Latin words start

      else if(firstWord == "STARTCHAR"){
        ssline>>secondWord; // get the next(second) word in the line
        if(secondWord.substr(0,3) == "uni") unicode = secondWord.substr(3); // truncate "uni" from "uniXXXX" leaving "XXXX" if it is unicode
        else unicode = secondWord; // else it is Latin word and just keep it

        if(unicode == "copyright") latinFlag = false; // indicate where the Latin words end
        else if(unicode == "4E00") chineseFlag = true; // indicate where the Chinese characters start

        if(compareMode){ // Only when compareMode is on
          chineseFlag = false;
          if(latinFlag) outhash<<unicode<<endl;
          else if(unicode == common){
            outhash<<unicode<<endl;
  				  chineseFlag = true;
            getline(incommon, common);
          }
        }
      }

      else if(firstWord == "BBX" && (chineseFlag || latinFlag)){ // get the BBX details of the character: w, h, x, y
        ssline>>s_w>>s_h>>s_x>>s_y;
        // convert string to integer:
        w = stoi(s_w);
        h = stoi(s_h);
        x = stoi(s_x);
        y = stoi(s_y);
      }

      // Case for bitmap
      else if((chineseFlag || latinFlag) && startFlag && firstWord != "ENCODING" && firstWord != "DWIDTH" && firstWord != "SWIDTH" && firstWord != "BITMAP" && firstWord != "ENDCHAR" && firstWord != "ENDFONT"){
        int index;

        /* Padding the small bit map to be full and complete */
        paddingYaxis(inbdf, line, firstWord, w, h, y);
        paddingXaxis(x);

        /* Compress the data structure by removing extra bits */
        removeExtraBits();

        /* Output the mapping to outmap with comments of unicodes*/
        index = 0;
        while(index<18)
        {
          outmap<<"0x"<<hex<<out[index]-0<<",";// hex format
          index++;
        }

        index = 0;
        while (index<18) {
          out[index]=0;
          index++;
        }
        outmap << "//"+unicode << "\n";

      }
    }
  }
  inbdf.close();
  return 0;
}

void paddingYaxis(ifstream& inbdf, string& line, string& firstWord, int w, int h, int y){
  int index = 0;
  int padding = 0;

  // Upper padding
  while(padding < (10 - h - y)){
    in[index] = 0;
    index++;
    padding++;
  }

  // get the bitmap
  while(index<(h+padding))
  {
    if(w<=8) in[index] = (stoi(firstWord,0,16))<<8; // If its mapping only has two columns(w<=8), then it needs to be shifted left by two columns(8 bits) to align the uniform format
    else in[index] = stoi(firstWord,0,16);

    getline(inbdf,line);
    stringstream ssline(line);
    getline(ssline,firstWord,' ');
    index++;
  }

  // Lower padding
  while(index < 12){
    in[index] = 0;
    index++;
  }
}

void paddingXaxis(int x){
  int index = 0;
  while(index<12){
    for(int xCount = 0; xCount < x; xCount++){
      in[index] = (in[index]>>1)&0x7FFF;
    }
    index++;
  }
}

void removeExtraBits(){
  int i = 0;
  while(i<12)
  {
    if(i%2==0)
    {
      out[3*i/2] = (in[i]&0xFF00)>>8;
      out[3*i/2+1] = in[i]&0xF0;
    }
    else
    {
      out[(i-1)/2*3+1] |= (in[i]&0xF000)>>12;
      out[(i-1)/2*3+2] = (in[i]&0xFF0)>>4;
    }
    i++;
  }
}
