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

#ifndef BITARRAY_H_
#define BITARRAY_H_

#include <iostream>
#include <cstdlib>
#include <cmath>

#include "BitArrayException.h"

using namespace std;

class BitArray {
private:
	static const unsigned long int INITIAL_SIZE;
	static const unsigned long int INCREMENT;

	unsigned char* bit_array;
	unsigned char* new_bit_array;
	unsigned long int bit_array_size;

	unsigned long int total_bytes;

public:
	BitArray(unsigned long int size = INITIAL_SIZE) throw (BitArrayException);
	virtual ~BitArray();

	void set_bit(unsigned long int bit) throw (BitArrayException);
	unsigned char get_bit(unsigned long int bit);
	unsigned long int get_total_bytes();

	void filter(const double* data, double* filtered_data, unsigned long int size) throw (BitArrayException);
};

#endif
