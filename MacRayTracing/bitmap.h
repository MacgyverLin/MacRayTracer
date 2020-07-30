#ifndef _BITMAP_h_
#define _BITMAP_h_

#include "object3.h"

class bitmap
{
public:
	bitmap(int w, int h)
	{
		this->w = w;
		this->h = h;
		this->buffer = new unsigned char[w * h * 3];
	}

	~bitmap()
	{
		if (buffer)
		{
			delete[] buffer;
			buffer = nullptr;
		}
	}

	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
	{
		unsigned char* ptr = buffer + (y * w + x) * 3;

		*ptr++ = b;
		*ptr++ = g;
		*ptr++ = r;
	}

    bool savePPM(const char* filename)
    {
        FILE* f = fopen(filename, "wt");
        if (!f)
            return false;

        fprintf(f, "P3\n");
        fprintf(f, "%d %d\n", w, h);
        fprintf(f, "255\n");

        for (int j = 0; j < h; j++)
        {
            unsigned char* ptr = buffer + w * (h - 1 - j) * 3;
            for (int i = 0; i < w; i++)
            {
                unsigned char b = *ptr++;
                unsigned char g = *ptr++;
                unsigned char r = *ptr++;
                fprintf(f, "%d %d %d\n", r, g, b);
            }
        }
        fclose(f);

        return true;
    }

    int saveBMP(const char* filename)
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
        fwrite(buffer, 1, imagesize, f);
        fclose(f);

        return true;
    }


	unsigned char* buffer;
	int w;
	int h;
};

#endif