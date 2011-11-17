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

#ifndef COLUMN_H_
#define COLUMN_H_

#include <iostream>
#include <fstream>
#include <limits>
#include <cstdarg>

#include <R.h>
#include <Rmath.h>

#include "ColumnException.h"

using namespace std;

class Column {
protected:
	static const int HEADER_BUFFER_SIZE;

	char* header;
	int order;
	double numeric_value;
	char* end_ptr;

public:
	char* char_value;

	Column();
	virtual ~Column();

	void set_header(const char* header, ... ) throw (ColumnException);
	const char* get_header();

	void set_value(char* value);
	char* get_value();

	double get_numeric_value();

	void set_order(int order);
	int get_order();

	virtual void out(ofstream& output_stream) throw (ofstream::failure);
};

#endif
