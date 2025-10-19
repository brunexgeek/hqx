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

#include <hqx/hqx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

#pragma pack(pop)


/**
 * @brief Saves an Windows Bitmap image (24 BPP).
 */
static int main_saveBitmap(
	const uint32_t *data,
	int width,
	int height,
	const char *fileName )
{
	struct BitmapHeader bh;
	struct DibHeader dh;
	uint32_t zero = 0;
	const uint32_t *ptr;

	FILE *output = fopen(fileName, "wb");
	if (output == NULL)
		return false;

	int padding = ((width + 3) & (~0x03)) - width;

	dh.biSize          = sizeof(struct DibHeader);
	dh.biWidth         = (uint32_t) width;
	dh.biHeight        = (uint32_t) height;
	dh.biPlanes        = 1;
	dh.biBitCount      = 24;
	dh.biCompression   = 0;
	dh.biSizeImage     = (uint16_t) ( (width*3+padding)*height );
	dh.biXPelsPerMeter = 0x2E23;
	dh.biYPelsPerMeter = dh.biXPelsPerMeter;
	dh.biClrUsed       = 0;
	dh.biClrImportant  = 0;

	bh.bfType    = 0x4D42;
	bh.bfSize    = dh.biSizeImage + 0x0036;
	bh.bfRes1    = 0;
	bh.bfOffBits = 0x0036;
	fwrite((char*) &bh, sizeof(struct BitmapHeader), 1, output);
	fwrite((char*) &dh, sizeof(struct DibHeader), 1, output);

	ptr = data + (width * height);
	for (int i = 0; i < height; i++)
	{
		ptr -= width;

		for (int j = 0; j < width; ++j)
			fwrite( (char*) (ptr + j), 3, 1, output);

		if (padding > 0)
			fwrite( (char*) &zero, (size_t) padding, 1, output);
	}

	fclose(output);

	return true;
}


/**
 * @brief Loads an Windows Bitmap image (24 BPP).
 */
static int main_loadBitmap(
	const char *fileName,
	uint32_t **data,
	int *width,
	int *height )
{
	struct BitmapHeader bh;
	struct DibHeader dh;
	int  padding;
	uint32_t zero = 0;
	uint32_t *ptr;

	FILE *input = fopen(fileName, "rb");
	if (input == NULL)
		return false;

	fread( (char*) &bh, sizeof(struct BitmapHeader) , 1, input);
	if (bh.bfType != 0x4D42)
		return false;
	fread( (char*) &dh.biSize, sizeof(uint32_t) , 1, input);
	if (dh.biSize != 40)
		return false;

	fread( (char*) &dh.biWidth, sizeof(struct DibHeader) - sizeof(uint32_t) , 1, input);
	*width  = (int) dh.biWidth;
	*height = (int) dh.biHeight;
	if (dh.biBitCount != 24)
		return false;

	padding = ((*width + 3) & ~0x03) - *width;
	ptr = *data = (uint32_t*) malloc((size_t) *width * (size_t) *height * sizeof(uint32_t));
	ptr += *width * *height;
	for (int i = 0; i < *height; i++)
	{
		ptr -= *width;

		for (int j = 0; j < *width; ++j)
		{
			fread( (char*) (ptr + j), 3 , 1, input);
			*(ptr + j) |= 0xFF000000;
		}

		if (padding > 0)
			fread( (char*) &zero, (size_t) padding , 1, input);
	}

	fclose(input);
	return true;
}


static int main_help()
{
	puts("Usage: sample <input image> <output image> [ <factor> ]\n");
	puts("Factor must be '2' for hq2x or '3' for hq3x.");
	return 1;
}


int main(int argc, char **argv )
{
	int factor = 2;

	if (argc != 3 && argc != 4) return main_help();
	if (argc == 4)
		factor = atoi(argv[3]);
	if (factor != 2 && factor != 3) return main_help();

	const char *inputFileName = argv[1];
	const char *outputFileName = argv[2];

	// loads the input image
	int width, height;
	uint32_t *image = NULL;
	if (!main_loadBitmap(inputFileName, &image, &width, &height))
	{
		printf("Unable to open '%s'", inputFileName);
		return 1;
	}
	printf("Resizing '%s' [%dx%d] by %dx", inputFileName, width, height, factor);

	clock_t t = clock();

	// resize the input image using the given scale factor
	size_t output_size = (size_t) (width * factor) * (size_t) (height * factor) * sizeof(uint32_t);
	uint32_t *output = (uint32_t*) malloc(output_size);
	int result = 0;
	if (factor == 2)
		result = hqx_scale2x(image, width, height, output, output_size, NULL);
	else
		result = hqx_scale3x(image, width, height, output, output_size, NULL);
	if (result != HQXERR_OK)
		return 1;

	t = clock() - t;
	printf("Processing time: %ld ms", t / (CLOCKS_PER_SEC / 1000));

	// saves the resized image
	if (!main_saveBitmap(output, width * factor, height * factor, outputFileName) != 0 )
		return 1;

	free(image);
	free(output);
}
