/*
 * Copyright 2016 Bruno Ribeiro
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hqx/HQ2x.hh>
#include <hqx/HQ3x.hh>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <ctime>


using std::ifstream;
using std::ofstream;
using std::string;


#pragma pack(push, 1)

struct BitmapHeader
{
    uint16_t bfType;
    uint32_t bfSize;
    uint32_t bfRes1;
    uint32_t bfOffBits;
};

struct DibHeader
{
	uint32_t biSize;
	uint32_t biWidth;
	uint32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};

typedef struct
{
	int32_t ciexyzX;
	int32_t ciexyzY;
	int32_t ciexyzZ;
} DibCIEXYZ;

typedef struct 
{
	DibCIEXYZ  ciexyzRed;
	DibCIEXYZ  ciexyzGreen;
	DibCIEXYZ  ciexyzBlue;
} DibCIEXYZTRIPLE;

typedef struct {
	uint32_t        bV5Size;
	int32_t         bV5Width;
	int32_t         bV5Height;
	uint16_t        bV5Planes;
	uint16_t        bV5BitCount;
	uint32_t        bV5Compression;
	uint32_t        bV5SizeImage;
	int32_t         bV5XPelsPerMeter;
	int32_t         bV5YPelsPerMeter;
	uint32_t        bV5ClrUsed;
	uint32_t        bV5ClrImportant;
	uint32_t        bV5RedMask;
	uint32_t        bV5GreenMask;
	uint32_t        bV5BlueMask;
	uint32_t        bV5AlphaMask;
	uint32_t        bV5CSType;
	DibCIEXYZTRIPLE bV5Endpoints;
	uint32_t        bV5GammaRed;
	uint32_t        bV5GammaGreen;
	uint32_t        bV5GammaBlue;
	uint32_t        bV5Intent;
	uint32_t        bV5ProfileData;
	uint32_t        bV5ProfileSize;
	uint32_t        bV5Reserved;
} DibHeaderV5;
#pragma pack(pop)


/**
 * @brief Saves an Windows Bitmap image (24 BPP).
 */
int main_saveBitmap24(
	const uint32_t *data,
	uint32_t width,
	uint32_t height,
	const string &fileName )
{
	BitmapHeader bh;
	DibHeader dh;
	uint16_t suffix;
	uint32_t zero = 0;
	const uint32_t *ptr;

	ofstream output(fileName.c_str(), std::ios_base::binary);
	if (!output.good()) return -1;

	suffix = ((width + 3) & ~0x03) - width;

	dh.biSize          = sizeof(DibHeader);
	dh.biWidth         = width;
	dh.biHeight        = height;
	dh.biPlanes        = 1;
	dh.biBitCount      = 24;
	dh.biCompression   = 0;
	dh.biSizeImage     = (uint16_t) ( (width*3+suffix)*height );
	dh.biXPelsPerMeter = 0x2E23;
	dh.biYPelsPerMeter = dh.biXPelsPerMeter;
	dh.biClrUsed       = 0;
	dh.biClrImportant  = 0;

	bh.bfType    = 0x4D42;
	bh.bfSize    = dh.biSizeImage + 0x0036;
	bh.bfRes1    = 0;
	bh.bfOffBits = 0x0036;
	output.write( (char*) &bh, sizeof(BitmapHeader) );
	output.write( (char*) &dh, sizeof(DibHeader) );

	ptr = data + (width * height);
	for (uint32_t i = 0; i < height; i++)
	{
		ptr -= width;

		for (uint32_t j = 0; j < width; ++j)
			output.write( (char*) (ptr + j), 3 );

		if (suffix > 0)
			output.write( (char*) &zero, suffix );
	}

  output.close();

  return 0;
}

int main_saveBitmap32(
	const uint32_t* data,
	uint32_t width,
	uint32_t height,
	const string& fileName)
{
	BitmapHeader bh;
	DibHeaderV5 dh = { 0 };
	uint16_t suffix;
	uint32_t zero = 0;
	const uint32_t* ptr;

	ofstream output(fileName.c_str(), std::ios_base::binary);
	if (!output.good()) return -1;

	dh.bV5Size = sizeof(DibHeaderV5);
	dh.bV5Width = width;
	dh.bV5Height = height;
	dh.bV5Planes = 1;
	dh.bV5BitCount = 32;
	dh.bV5Compression = 3;
	dh.bV5SizeImage = 0;
	dh.bV5XPelsPerMeter = 0x2E23;
	dh.bV5YPelsPerMeter = dh.bV5XPelsPerMeter;
	dh.bV5ClrUsed = 0;
	dh.bV5ClrImportant = 0;
	dh.bV5RedMask = 0x00ff0000;
	dh.bV5GreenMask = 0x0000ff00;
	dh.bV5BlueMask = 0x000000ff;
	dh.bV5AlphaMask = 0xff000000;
	dh.bV5CSType = 0x57696e20;// 'Win '

	bh.bfType = 0x4D42; // 'BM'
	bh.bfSize = width * height * 4 + sizeof(DibHeaderV5) + sizeof(BitmapHeader);
	bh.bfRes1 = 0;
	bh.bfOffBits = sizeof(DibHeaderV5) + sizeof(BitmapHeader);
	output.write((char*)&bh, sizeof(BitmapHeader));
	output.write((char*)&dh, sizeof(DibHeaderV5));

	ptr = data + (width * height);
	for (uint32_t i = 0; i < height; i++)
	{
		ptr -= width;

		for (uint32_t j = 0; j < width; ++j)
			output.write((char*)(ptr + j), 4);
	}

	output.close();

	return 0;
}

/**
 * @brief Loads an Windows Bitmap image (24 BPP).
 */
int main_loadBitmap24(
	const string &fileName,
	uint32_t *&data,
	uint16_t &width,
	uint16_t &height )
{
	BitmapHeader bh;
	DibHeader dh;
	uint16_t  suffix;
	uint32_t zero = 0;
	uint32_t *ptr;
	uint16_t bits;

	ifstream input(fileName.c_str(), std::ios_base::binary);
	if (!input.good()) return -1;

	input.read( (char*) &bh, sizeof(BitmapHeader) );
	if (bh.bfType != 0x4D42) return -1;
	input.read( (char*) &dh.biSize, sizeof(uint32_t) );
	if (dh.biSize != 40) return -1;

	input.read( (char*) &dh.biWidth, sizeof(DibHeader) - sizeof(uint32_t) );
	width  = dh.biWidth;
	height = dh.biHeight;
	if (dh.biBitCount != 24) return -1;

	suffix = ((width + 3) & ~0x03) - width;
	ptr = data = new uint32_t[width * height]();
	ptr += width * height;
	for (uint32_t i = 0; i < height; i++)
	{
		ptr -= width;

		for (uint32_t j = 0; j < width; ++j)
		{
			input.read( (char*) (ptr + j), 3 );
			*(ptr + j) |= 0xFF000000;
		}

		if (suffix > 0)
			input.read( (char*) &zero, suffix );
	}

	input.close();
	return 0;
}

int main_loadBitmap32(
	const string& fileName,
	uint32_t*& data,
	uint16_t& width,
	uint16_t& height)
{
	BitmapHeader bh;
	DibHeaderV5 dh;
	uint16_t  suffix;
	uint32_t zero = 0;
	uint32_t* ptr;
	uint16_t bits;

	ifstream input(fileName.c_str(), std::ios_base::binary);
	if (!input.good()) return -1;

	input.read((char*)&bh, 14);
	if (bh.bfType != 0x4D42) return -1;
	input.read((char*)&dh.bV5Size, sizeof(uint32_t));
	if (dh.bV5Size != sizeof(DibHeaderV5)) return -1;

	input.read((char*)&dh.bV5Width, sizeof(DibHeaderV5) - sizeof(uint32_t));
	width = dh.bV5Width;
	height = dh.bV5Height;
	if (dh.bV5BitCount != 32) return -1;
	
	input.seekg(bh.bfOffBits, std::ios_base::_Seekbeg);
	
	ptr = data = new uint32_t[width * height]();
	ptr += width * height;
	for (uint32_t i = 0; i < height; i++)
	{
		ptr -= width;

		for (uint32_t j = 0; j < width; ++j)
		{
			input.read((char*)(ptr + j), 4);
		}
	}

	input.close();
	return 0;
}


int main(int argc, char **argv )
{
	uint32_t factor = 2;

	if (argc != 2 && argc != 3) return 1;
	if (argc == 3) factor = atoi(argv[2]);

	// loads the input image
	uint16_t width, height;
	uint32_t *image = NULL;
	if ( main_loadBitmap32(argv[1], image, width, height) != 0) return 1;
	std::cout << "Resizing '" << argv[1] << "' [" << width << "x" << height << "] by " <<
		factor << 'x' << std::endl;

	clock_t t = clock();

	// resize the input image using the given scale factor
	uint32_t outputSize = (width * factor) * (height * factor);
	uint32_t *output = new uint32_t[outputSize]();
	HQx *scale;
	if (factor == 2)
		scale = new HQ2x();
	else
		scale = new HQ3x();
	scale->resize(image, width, height, output);
	delete scale;

	t = clock() - t;
	std::cout << "Processing time: " << t / (CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

	// saves the resized image
	if ( main_saveBitmap32(output, width * factor, height * factor, "output.bmp") != 0 ) return 1;

	delete[] image;
	delete[] output;
}
