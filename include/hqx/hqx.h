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

#ifndef HQX_HQX_HH
#define HQX_HQX_HH

#include <stdint.h>
#include <stdbool.h>

struct hqx_parameters
{
	uint32_t trY;
	uint32_t trU;
	uint32_t trV;
	uint32_t trA;
	bool wrapX;
	bool wrapY;
};

uint32_t *hqx_scale2x(
	const uint32_t *image,
	uint32_t width,
	uint32_t height,
	uint32_t *output,
	const struct hqx_parameters *params );

uint32_t *hqx_scale3x(
	const uint32_t *image,
	uint32_t width,
	uint32_t height,
	uint32_t *output,
	const struct hqx_parameters *params );

#endif  // HQX_HQX_HH
