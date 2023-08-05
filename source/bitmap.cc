#include "Bitmap.h"

Bitmap::Bitmap(
	uint32_t* data,
	uint32_t width,
	uint32_t height,
	uint16_t bitCount
)
	: data(data)
	, width(width)
	, height(height)
	, bitCount(bitCount)
{
}

Bitmap::Bitmap(std::istream& input) {
	load(input);
}

void Bitmap::save(std::ostream& output) {
	if (bitCount == 24)
		save24bit(output);
	if (bitCount == 32)
		save32bit(output);
}

void Bitmap::save24bit(std::ostream& output) const
{
	BitmapHeader bh;
	DibHeader dh;
	uint16_t suffix;
	uint32_t zero = 0;
	const uint32_t* ptr;

	suffix = (4 - (3 * width) % 4) % 4;

	dh.biSize = sizeof(DibHeader);
	dh.biWidth = width;
	dh.biHeight = height;
	dh.biPlanes = 1;
	dh.biBitCount = 24;
	dh.biCompression = 0;
	dh.biSizeImage = (uint32_t)((width * 3 + suffix) * height);
	dh.biXPelsPerMeter = 0x2E23;
	dh.biYPelsPerMeter = dh.biXPelsPerMeter;
	dh.biClrUsed = 0;
	dh.biClrImportant = 0;

	bh.bfType = 0x4D42;
	bh.bfSize = dh.biSizeImage + 0x0036;
	bh.bfRes1 = 0;
	bh.bfOffBits = 0x0036;
	output.write((char*)&bh, sizeof(BitmapHeader));
	output.write((char*)&dh, sizeof(DibHeader));

	ptr = data + (width * height);
	for (uint32_t i = 0; i < height; i++)
	{
		ptr -= width;

		for (uint32_t j = 0; j < width; ++j)
			output.write((char*)(ptr + j), 3);

		if (suffix > 0)
			output.write((char*)&zero, suffix);
	}
}

void Bitmap::save32bit(std::ostream& output) const
{
	BitmapHeader bh;
	DibHeaderV5 dh = { 0 };
	uint32_t zero = 0;
	const uint32_t* ptr;

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
}

int Bitmap::load(std::istream& input)
{
	BitmapHeader bh;
	input.read((char*)&bh, sizeof(BitmapHeader));
	if (bh.bfType != 0x4D42) return -1;

	uint32_t biSize;
	input.read((char*)&biSize, sizeof(uint32_t));

	if (biSize == sizeof(DibHeader))
		return loadDibHeader(input, bh);

	if (biSize == sizeof(DibHeaderV5))
		return loadV5Header(input, bh);

	return 0;
}

int Bitmap::loadDibHeader(std::istream& input, const BitmapHeader& bh)
{
	DibHeader dh;
	input.read((char*)&dh.biWidth, sizeof(DibHeader) - sizeof(uint32_t));
	width = dh.biWidth;
	height = dh.biHeight;
	bitCount = dh.biBitCount;

	input.seekg(bh.bfOffBits, std::ios_base::_Seekbeg);
	if (dh.biBitCount == 24)
		return read24bit(input);

	if (dh.biBitCount == 32)
		return read32bit(input);

	return -1;
}

int Bitmap::loadV5Header(std::istream& input, const BitmapHeader& bh)
{
	DibHeaderV5 dh;
	input.read((char*)&dh.bV5Width, sizeof(DibHeaderV5) - sizeof(uint32_t));
	width = dh.bV5Width;
	height = dh.bV5Height;
	bitCount = dh.bV5BitCount;

	input.seekg(bh.bfOffBits, std::ios_base::_Seekbeg);

	if (dh.bV5BitCount == 24)
		return read24bit(input);

	if (dh.bV5BitCount == 32)
		return read32bit(input);

	return -1;
}

int Bitmap::read24bit(std::istream& input) {
	uint16_t  suffix;
	uint32_t* ptr;
	uint32_t zero = 0;

	suffix = (4 - (3 * width) % 4) % 4;
	ptr = data = new uint32_t[width * height]();
	ptr += width * height;
	for (uint32_t i = 0; i < height; i++)
	{
		ptr -= width;

		for (uint32_t j = 0; j < width; ++j)
		{
			input.read((char*)(ptr + j), 3);
			*(ptr + j) |= 0xFF000000;
		}

		if (suffix > 0)
			input.read((char*)&zero, suffix);
	}
	return 0;
}

int Bitmap::read32bit(std::istream& input) {
	uint32_t* ptr;
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

	return 0;
}
