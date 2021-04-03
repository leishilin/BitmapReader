#include <iostream>
#include "readbmp.h"

using namespace std;
int main()
{
    bitmapT ts("C:\\Users\\12739\\Desktop\\ta.bmp");
    const color white = { 255,255,255 };
    const color black = { 0,0,0 };
    ts.black_white();
    cout << "success\n";
}