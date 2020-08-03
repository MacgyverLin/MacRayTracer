#ifndef _UTIL_H_
#define _UTIL_H_

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <memory>
#include <vector>
#include <map>
using namespace std;

#include "vec3.h"

#define ONE_PI 3.141592654
#define HALF_PI (ONE_PI / 2)
#define TWO_PI (ONE_PI * 2)

#define DEG2RAD(r) ((r)*ONE_PI/180.0)
#define RAD2DEG(r) ((r)*180.0/ONE_PI)

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }
inline float ffclamp(float v, float min, float max) { return ffmax(ffmin(v, max), min); }

bool savePPM(const char* filename, int w, int h, unsigned char* data)
{
    FILE* f = fopen(filename, "wt");
    if (!f)
        return false;

    fprintf(f, "P3\n");
    fprintf(f, "%d %d\n", w, h);
    fprintf(f, "255\n");

    for (int i = 0; i < w * h; i++)
    {
        fprintf(f, "%d %d %d\n", *data++, *data++, *data++);
    }

    fclose(f);

    return true;
}

int saveBMP(const char* filename, int w, int h, unsigned char* data)
{
    //width, height, and bitcount are the key factors:
    int32_t width = w;
    int32_t height = h;
    uint16_t bitcount = 24;//<- 24-bit bitmap

    //take padding in to account
    int width_in_bytes = ((width * bitcount + 31) / 32) * 4;

    //total image size in bytes, not including header
    uint32_t imagesize = width_in_bytes * height;

    //this value is always 40, it's the sizeof(BITMAPINFOHEADER)
    const uint32_t biSize = 40;

    //bitmap bits start after headerfile, 
    //this is sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    const uint32_t bfOffBits = 54;

    //total file size:
    uint32_t filesize = 54 + imagesize;

    //number of planes is usually 1
    const uint16_t biPlanes = 1;

    //create header:
    //copy to buffer instead of BITMAPFILEHEADER and BITMAPINFOHEADER
    //to avoid problems with structure packing
    unsigned char header[54] = { 0 };
    memcpy(header, "BM", 2);
    memcpy(header + 2, &filesize, 4);
    memcpy(header + 10, &bfOffBits, 4);
    memcpy(header + 14, &biSize, 4);
    memcpy(header + 18, &width, 4);
    memcpy(header + 22, &height, 4);
    memcpy(header + 26, &biPlanes, 2);
    memcpy(header + 28, &bitcount, 2);
    memcpy(header + 34, &imagesize, 4);

    FILE* f = fopen(filename, "wb");
    if (!f)
        return false;

    fwrite(header, 1, 54, f);
    fwrite(data, 1, imagesize, f);
    fclose(f);

    return true;
}

float random()
{
    return rand() / (RAND_MAX + 1.0);
}


vec3 random_in_unit_sphere()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3(random(), random(), random()) - vec3(1, 1, 1);
    } while (dot(p, p) >= 1.0);

    return p;
}

vec3 random_in_unit_disk()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3(random(), random(), 0.0) - vec3(1, 1, 0.0);
    } while (dot(p, p) >= 1.0);

    return p;
}

#endif