/*
 * Copyright © 2011 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
 *
 * This file is part of GWAtoolbox.
 *
 * GWAtoolbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GWAtoolbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GWAtoolbox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "include/BitArray.h"

const unsigned long int BitArray::INITIAL_SIZE = 4194304;
const unsigned long int BitArray::INCREMENT = 1048576;

BitArray::BitArray(unsigned long int size) throw (BitArrayException):
		bit_array(NULL), new_bit_array(NULL), bit_array_size(size), total_bytes(0)  {

	if (size > 0) {
		total_bytes = (unsigned long int)ceil(bit_array_size / 8.0);

		bit_array = (unsigned char*)malloc(total_bytes * sizeof(unsigned char));
		if (bit_array == NULL) {
			throw BitArrayException("BitArray", "BitArray( unsigned long int )", __LINE__, 2, total_bytes * sizeof(unsigned char));
		}

		for (unsigned int i = 0; i < total_bytes; i++) {
			bit_array[i] = 0x00;
		}
	}
}

BitArray::~BitArray() {
	if (bit_array != NULL) {
		free(bit_array);
		bit_array = NULL;
	}
}

void BitArray::set_bit(unsigned long int bit) throw (BitArrayException) {
	unsigned long int byte = bit >> 3;

	if (byte >= total_bytes) {
		unsigned long int first_byte = total_bytes;
		unsigned long int increment = (unsigned long int)ceil(INCREMENT / 8.0);

		total_bytes += ((increment > (byte - total_bytes + 1)) ? increment : (byte - total_bytes + 1));
		bit_array_size = total_bytes * sizeof(unsigned char) * 8;

		new_bit_array = (unsigned char*)realloc(bit_array, total_bytes * sizeof(unsigned char));
		if (new_bit_array == NULL) {
			free(bit_array);
			bit_array = NULL;
			throw BitArrayException("BitArray", "set_bit( unsigned long int )", __LINE__, 3, total_bytes * sizeof(unsigned char));
		}

		bit_array = new_bit_array;

		for (unsigned long int i = first_byte; i < total_bytes; i++) {
			bit_array[i] = 0x00;
		}
	}

	bit_array[byte] |= (0x80 >> (bit & 0x07));
}

unsigned char BitArray::get_bit(unsigned long int bit) {
	return *(bit_array + (bit >> 3)) & (0x80 >> (bit & 0x07));
}

unsigned long int BitArray::get_total_bytes() {
	return total_bytes;
}

void BitArray::filter(const double* data, double* filtered_data, unsigned long int size) throw (BitArrayException) {
	unsigned long int upper_byte = bit_array_size >> 3;
	unsigned long int current_chunk = 0;
	unsigned long int current_filtered = 0;

	if (data == NULL) {
		throw BitArrayException("BitArray", "filter( const double* , double* , unsigned long int )", __LINE__, 0, "data");
	}

	if (filtered_data == NULL) {
		throw BitArrayException("BitArray", "filter( const double* , double* , unsigned long int )", __LINE__, 0, "filtered_data");
	}

	if (size == 0) {
		throw BitArrayException("BitArray", "filter( const double* , double* , unsigned long int )", __LINE__, 1, "size");
	}

	for (unsigned long int i = 0u; i < upper_byte; ++i) {
		current_chunk = i * 8;
		for (unsigned int j = 0u; j < 8u; ++j) {
			if ((bit_array[i] & (0x80 >> j)) != 0x00) {
				if (current_chunk + j >= size) {
					throw BitArrayException("BitArray", "filter( const double* , double* , unsigned long int )", __LINE__, 1, "size");
				}

				filtered_data[current_filtered] = data[current_chunk + j];
				current_filtered += 1;
			}
		}
	}
}
