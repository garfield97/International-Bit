/* See
 * http://lancaster-university.github.io/microbit-docs/advanced/
 * for docs about using the micro:bit library
*/
#include "MicroBit.h"
#include "ExternalDisplay.h"
using namespace std;

int main()
{
    //setScreen(OLD);
    initialise();
    int var = '一';
    scrollString("He",LEFT,40);
    scrollString("llo",UP,40);
    scrollString("wor",RIGHT,40);
    scrollString("ld",DOWN,40);
    for(int i = 0; i < 200; i++)
     updateDisplay(var+i, NONE,500);
    display->print(":");
    checkError();
    release_fiber();
    reset();
}
 
