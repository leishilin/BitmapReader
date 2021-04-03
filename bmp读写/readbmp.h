#pragma once
#ifndef _READBMP_H_
#define _READBMP_H_

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <iomanip>
#pragma warning(disable:4996)
using namespace std;
struct color
{
    short int Blue = 0, Green = 0, Red = 0;
};
bool operator ==(color A, color B)
{
    if (A.Blue == B.Blue && A.Green == B.Green && A.Red == B.Red) return 1; else return 0;
}
class bitmapT
{
private:
    FILE* fp;
    int EmBits;
    void Headwrite24Bit();
    void pixelread();
    void pixelwrite();
    void newbitmap(int w, int h);
public:
    bitmapT(const char* filename);
	~bitmapT();
    void enlarge(int newwidth, int newheight);
    void showpixels();
    void fill_all(color fill);
    void point(int x, int y, int r, color fill);
    void circle(int x, int y, int r, int thick, color fill);
    void change(color last, color fill);
    void black_white();

    const char* filename;
    color* pixels;
    int width, height, linelength;
    BITMAPINFOHEADER infoHead;
    BITMAPFILEHEADER fileHead;
};
bitmapT::bitmapT(const char* name)
{
    filename = name;
    if (fp = fopen(filename, "rb+"))
    {
        fseek(fp, sizeof(BITMAPFILEHEADER), 0);

        BITMAPINFOHEADER bmpHead;
        fread(&bmpHead, sizeof(BITMAPINFOHEADER), 1, fp);

        width = bmpHead.biWidth;
        height = bmpHead.biHeight;
        if (width <= 0 or height <= 0) {
            cout << "未发现" << filename << "\n将创建一个" << filename << "\n请输入新图片的宽度和高度\n";
            cin >> width >> height;
            newbitmap(width, height);
        }
        linelength = ((3 * width - 1) / 4 + 1) * 4;

        if (bmpHead.biBitCount != 24)
        {
            std::cout << "不是24位的bmp图像";
        }
    }
    else { 
    cout << "未发现" << filename << "\n将创建一个" << filename << "\n请输入新图片的宽度和高度\n";
    cin >> width >> height;
    newbitmap(width, height);
    }

    EmBits = 3 - (3 * width - 1) % 4;
    pixels = new color[width * height];
    pixelread();

}
bitmapT::~bitmapT()
{
    pixelwrite();
    delete[] pixels;
    fclose(fp);   //关闭文件
}
inline void bitmapT::Headwrite24Bit()
{
    linelength = ((3 * width - 1) / 4 + 1) * 4;
    int dataSize = height * linelength;
    EmBits = 3 - (3 * width - 1) % 4;
  
    fileHead.bfType = 0x4D42;
    fileHead.bfSize = 54 + dataSize;
    fileHead.bfReserved1 = 0;
    fileHead.bfReserved2 = 0;
    fileHead.bfOffBits = 54;

    infoHead.biSize = 40;
    infoHead.biWidth = width;
    infoHead.biHeight = height;
    infoHead.biPlanes = 1;
    infoHead.biBitCount = 24;
    infoHead.biCompression = 0;
    infoHead.biSizeImage = dataSize;
    infoHead.biClrImportant = 0;
    infoHead.biXPelsPerMeter = 0;
    infoHead.biYPelsPerMeter = 0;

    fseek(fp, 0, 0);
    fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);
};
inline void bitmapT::enlarge(int newwidth, int newheight)
{
    if (newwidth > width or newheight > height)
    {
        color* newpixels = new color[newwidth * newheight];

        double a, b;
        a = newheight;
        a /= height;
        b = newwidth;
        b /= width;
        for (int i = 0; i < height; i++)
        {
            for (int j = i * a; j < (i + 1) * a; j++)
            {
                for (int k = 0; k < width; k++)
                {
                    for (int l = k * b; l < (k + 1) * b; l++)
                    {
                        newpixels[j * newwidth + l] = pixels[i * width + k];
                    }
                }
            }
        }

        fclose(fp);
        newbitmap(newwidth, newheight);
        delete[] pixels;
        pixels = newpixels;

        pixelwrite();
    }
}
inline void bitmapT::pixelread()
{
    fseek(fp, 54, 0);
    int i, j, n = 0, * p, * s;
    s = new int[width * height]{0};
    p = s;

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            fread(p, 1, 1, fp);
            pixels[i * width + j].Blue = *p;
            fread(p, 1, 1, fp);
            pixels[i * width + j].Green = *p;
            fread(p, 1, 1, fp);
            pixels[i * width + j].Red = *p;
            p += 1;
        }
        fseek(fp, EmBits, 1);
    }

    delete[] s;
}//把指针file指向的bmp图像读取为一维数组存储在数组中。
inline void bitmapT::pixelwrite()
{
    fseek(fp, 54, 0);
    int c, d = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            c = 0x010000 * pixels[i * width + j].Red + 0x0100 * pixels[i * width + j].Green + pixels[i * width + j].Blue;
            fwrite(&c, 3, 1, fp);
        }
        fwrite(&d, EmBits, 1, fp);
    }
}
inline void bitmapT::newbitmap(int w, int h)
{
    fp = fopen(filename, "wb+");

    width = w;
    height = h;

    Headwrite24Bit();
}
inline void bitmapT::showpixels()
{
    int i, j;
    for (i = height - 1; i >= 0; i--)
    {
        for (j = 0; j < width; j++)
        {
            cout
                << 'R' << setw(3) << setfill('0') << pixels[width * i + j].Red
                << 'G' << setw(3) << setfill('0') << pixels[width * i + j].Green
                << 'B' << setw(3) << setfill('0') << pixels[width * i + j].Blue
                << ' ';
        }
        cout << '\n';
    }
}//按图片顺序以数字形式输出每个像素
inline void bitmapT::fill_all(color fill)
{
    int i, j, c;
    c = 0x010000 * fill.Red + 0x0100 * fill.Green + fill.Blue;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixels[i * width + j] = fill;
        }
    }
}//将整个图片变成同一颜色
inline void bitmapT::point(int x, int y, int r, color fill)
{
    y = height - y;
    x -= 1;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int n = i - y, m = j - x;
            if (n * n + m * m <= r * r - r)
            {
                pixels[i * width + j] = fill;
            }
        }
    }
}
inline void bitmapT::circle(int x, int y, int r, int thick, color fill)
{
    y = height - y;
    x -= 1;
    int a = r + thick, b = r - thick, ad = a * a - a, bd = b * b - b;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int n = i - y, m = j - x;
            int s = n * n + m * m;
            if (s <= a and s > b)
            {
                pixels[i * width + j] = fill;
            }
        }
    }
}
inline void bitmapT::change(color last, color fill)
{
    for (int i = 0; i < width * height; i++)
    {
        if (pixels[i] == last)
        {
            pixels[i] = fill;
        }
    }
}
int f(int x, int w) {
    int i, j;
    switch (x)
    {
    case 0:i = 0; j = 0; break;
    case 1:i = 2; j = 2; break;
    case 2:i = 2; j = 0; break;
    case 3:i = 0; j = 2; break;
    case 4:i = 1; j = 1; break;
    case 5:i = 3; j = 3; break;
    case 6:i = 1; j = 3; break;
    case 7:i = 3; j = 1; break;
    case 8:i = 0; j = 3; break;
    case 9:i = 2; j = 1; break;
    case 10:i = 0; j = 1; break;
    case 11:i = 2; j = 3; break;
    case 12:i = 1; j = 2; break;
    case 13:i = 3; j = 0; break;
    case 14:i = 3; j = 2; break;
    default:i = 1; j = 0; break;
    }
    return i * w + j;
}
inline void bitmapT::black_white()
{
    int nw = width * 4, nh = height * 4;
    color* newpixels = new color[nw * nh];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int darkness = pixels[i * width + j].Blue + pixels[i * width + j].Green + pixels[i * width + j].Red;
            darkness /= 45;
            for (int k = 0; k < 16; k++)
            {
                if (k < darkness) newpixels[f(k, nw) + 4 * (i * nw + j)] = { 255,255,255 };
                else newpixels[f(k, nw) + 4 * (i * nw + j)] = { 0,0,0 };
            }
        }
    }
    width = nw; height = nh;
    Headwrite24Bit();
    delete[] pixels;
    pixels = newpixels;
    pixelwrite();
}

#endif // !_READBMP_H_