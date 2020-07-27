#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
using namespace std;

struct RGBType
{
    double r;
    double g;
    double b;
};

void savebmp(const char* filename, int w, int h, int dpi, RGBType* data)
{
    FILE* f;
    int k = w * h;
    int s = 4 * k;
    int filesize = 54 + s;
    double factor = 39.375;
    int m = static_cast<int>(factor);

    int ppm = dpi * m;
    unsigned char bmpfileheader[14] = {'B','M',     0,0,0,0, 0,0,0,0, 54,0, 0,0 };
    unsigned char bmpinfoheader[40] = { 40,0,0,0,   0,0,0,0, 0,0,0,0,  1,0,24,0 };

    bmpfileheader[2]  = (unsigned char)(filesize);
    bmpfileheader[3]  = (unsigned char)(filesize>>8);
    bmpfileheader[4]  = (unsigned char)(filesize>>16);
    bmpfileheader[5]  = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(w);
    bmpinfoheader[ 5] = (unsigned char)(w >> 8);
    bmpinfoheader[ 6] = (unsigned char)(w >> 16);
    bmpinfoheader[ 7] = (unsigned char)(w >> 24);
    
    bmpinfoheader[ 8] = (unsigned char)(h);
    bmpinfoheader[ 9] = (unsigned char)(h >> 8);
    bmpinfoheader[10] = (unsigned char)(h >> 16);
    bmpinfoheader[11] = (unsigned char)(h >> 24);

    bmpinfoheader[21] = (unsigned char)(s);
    bmpinfoheader[22] = (unsigned char)(s >> 8);
    bmpinfoheader[23] = (unsigned char)(s >> 16);
    bmpinfoheader[24] = (unsigned char)(s >> 24);

    bmpinfoheader[25] = (unsigned char)(ppm);
    bmpinfoheader[26] = (unsigned char)(ppm >> 8);
    bmpinfoheader[27] = (unsigned char)(ppm >> 16);
    bmpinfoheader[28] = (unsigned char)(ppm >> 24);

    bmpinfoheader[29] = (unsigned char)(ppm);
    bmpinfoheader[30] = (unsigned char)(ppm >> 8);
    bmpinfoheader[31] = (unsigned char)(ppm >> 16);
    bmpinfoheader[32] = (unsigned char)(ppm >> 24);

    f = fopen(filename, "wb");
    fwrite(bmpfileheader, 1, sizeof(bmpfileheader)/ sizeof(char), f);
    fwrite(bmpinfoheader, 1, sizeof(bmpinfoheader) / sizeof(char), f);

    unsigned char color[3];
    for (int i = 0; i < k; i++)
    {
        color[0] = (int)floor(data[i].b * 255);
        color[1] = (int)floor(data[i].g * 255);
        color[2] = (int)floor(data[i].r * 255);

        fwrite(color, 1, 3, f);
    }

    fclose(f);
}

int main()
{
    cout << "Rendering..." << endl;

    int dpi = 72;
    int width = 640;
    int height = 480;

    RGBType* pixels = new RGBType[width * height];

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            // start with no antialiasing

            int idx = y * width + x;
            if((x>200 && x<440) && (y > 200 && y < 280))
                pixels[idx] = { 0, 0, 1.0 };
            else
                pixels[idx] = { 1.0, 0, 0.0 };
        }
    }

    savebmp("1.bmp", width, height, dpi, pixels);

    delete[] pixels;

    return 0;
}