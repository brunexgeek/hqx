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
#include <string>
#include "Bitmap.h"

using std::ifstream;
using std::ofstream;
using std::string;


int main(int argc, char **argv )
{
	uint32_t factor = 2;

	if (argc != 2 && argc != 3) return 1;
	if (argc == 3) factor = atoi(argv[2]);

	// loads the input image

	ifstream input(argv[1], std::ios_base::binary);
	if (!input.good()) {
		std::cout << "File not found" << std::endl;
		return -1;
	}
	Bitmap bitmap(input);
	input.close();
	
	std::cout << "Resizing '" << argv[1] << "' [" << bitmap.width << "x" << bitmap.height << "] by " <<
		factor << 'x' << std::endl;

	clock_t t = clock();

	// resize the input image using the given scale factor
	uint32_t outputSize = (bitmap.width * factor) * (bitmap.height * factor);
	uint32_t *output = new uint32_t[outputSize]();
	HQx *scale;
	if (factor == 2)
		scale = new HQ2x();
	else
		scale = new HQ3x();
	scale->resize(bitmap.data, bitmap.width, bitmap.height, output);
	delete scale;

	t = clock() - t;
	std::cout << "Processing time: " << t / (CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

	// saves the resized image
	ofstream outputFile(argv[1] + std::string(".") + std::to_string(factor) + "x.bmp", std::ios_base::binary);
	if (!outputFile.good()) return -1;

	Bitmap bitmapOut(output, bitmap.width * factor, bitmap.height * factor, bitmap.bitCount);
	bitmapOut.save(outputFile);
	outputFile.close();	

	delete[] bitmap.data;
	delete[] output;
}
