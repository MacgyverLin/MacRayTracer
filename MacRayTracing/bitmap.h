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
		this->buffer.resize(w * h);
	}

	~bitmap()
	{
	}

    bitmap(const bitmap& other)
    {
        this->w = other.w;
        this->h = other.h;
        this->buffer = other.buffer;
    }

    bitmap& operator=(const bitmap& other)
    {
        this->w = other.w;
        this->h = other.h;
        this->buffer = other.buffer;

        return *this;
    }

    bitmap& operator+=(const bitmap& other)
    {
        this->w = other.w;
        this->h = other.h;
        this->buffer = other.buffer;

        return *this;
    }

	void setPixel(int x, int y, const vec3& c)
	{
        buffer[y * w + x] = c;
	}

    vec3 maxBrightness()
    {
        vec3 max(0, 0, 0);
        for (int i = 0; i < w * h; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (max[k] < buffer[i][k])
                    max[k] = buffer[i][k];
            }
        }

        return max;
    }

    void tonemap()
    {
        vec3 max = maxBrightness();

        for (int i = 0; i < w * h; i++)
        {
            buffer[i] /= max;

            //buffer[i][0] = log(buffer[i][0] + 1) / log(max[0] + 1);
            //buffer[i][1] = log(buffer[i][1] + 1) / log(max[1] + 1);
            //buffer[i][2] = log(buffer[i][2] + 1) / log(max[2] + 1);
        }
    }

    void tonemap2(float A, float gamma)
    {
        for (int i = 0; i < w * h; i++)
        {
            buffer[i][0] = A * pow(buffer[i][0], gamma);
            buffer[i][1] = A * pow(buffer[i][1], gamma);
            buffer[i][2] = A * pow(buffer[i][2], gamma);
        }
    }

    bitmap scale()
    {
        bitmap result(w/2, h/2);

        for (int j = 0; j < h; j+=2)
        {
            vec3* ptr0 = &buffer[w * j];
            vec3* ptr1 = ptr0 + w;
            for (int i = 0; i < w; i+=2)
            {
                vec3 c = (*ptr0 + *(ptr0 + 1) + *ptr1 + *(ptr1 + 1)) / 4;
                result.setPixel(i >> 1, j >> 1, c);
            }
        }

        return result;
    }

    void gamma_correction(float gamma)
    {
        for (int i = 0; i < w * h; i++)
        {
            buffer[i][0] = pow(buffer[i][0], gamma);
            buffer[i][1] = pow(buffer[i][1], gamma);
            buffer[i][2] = pow(buffer[i][2], gamma);
        }
    }

    bool savePPM(const char* filename)
    {
        FILE* f = fopen(filename, "wt");
        if (!f)
            return false;

        fprintf(f, "P3\n");
        fprintf(f, "%d %d\n", w, h);
        fprintf(f, "255\n");

        unsigned char c[3];
        for (int j = 0; j < h; j++)
        {
            vec3* ptr = &buffer[w * (h - 1 - j)];
            for (int i = 0; i < w; i++)
            {
                c[0] = floor(255 * ptr->x());
                c[1] = floor(255 * ptr->y());
                c[2] = floor(255 * ptr->z());
                ptr++;
                fprintf(f, "%d %d %d\n", c[0], c[1], c[2]);
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

        unsigned char c[3];
        for (int j = 0; j < h; j++)
        {
            vec3* ptr = &buffer[w * j];
            for (int i = 0; i < w; i++)
            {
                c[2] = floor(255 * ffclamp(ptr->x(), 0, 1));
                c[1] = floor(255 * ffclamp(ptr->y(), 0, 1));
                c[0] = floor(255 * ffclamp(ptr->z(), 0, 1));

                ptr++;
                fwrite(c, sizeof(c[0]), 3, f);
            }
        }
        fclose(f);

        return true;
    }


	vector<vec3> buffer;
	int w;
	int h;
};

#endif