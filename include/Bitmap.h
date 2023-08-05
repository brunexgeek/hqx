#include <cstdint>
#include <iostream>

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


class Bitmap {
public:
	uint32_t* data;
	uint32_t width;
	uint32_t height;
	uint16_t bitCount;

	Bitmap(
		uint32_t* data,
		uint32_t width,
		uint32_t height,
		uint16_t bitCount
	);

	Bitmap(std::istream& input);

	void save(std::ostream& output);
	int load(std::istream& input);

private:
	void save24bit(std::ostream& output) const;
	void save32bit(std::ostream& output) const;
	int loadDibHeader(std::istream& input, const BitmapHeader& bh);
	int loadV5Header(std::istream& input, const BitmapHeader& bh);
	int read24bit(std::istream& input);
	int read32bit(std::istream& input);
};