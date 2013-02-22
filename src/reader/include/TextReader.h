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

#ifndef TEXTTABLEREADER_H_
#define TEXTTABLEREADER_H_

#include <fstream>
#include <vector>
#include <set>
#include <math.h>
#include <limits>
#include <time.h>

#include "Reader.h"

class TextReader : public Reader {
private:
	ifstream ifile_stream;

	int buffer_size;
	char* buffer;

public:
	static const unsigned int DEFAULT_BUFFER_SIZE;
	static const unsigned int TOP_ROWS_NUMBER;
	static const unsigned int ROWS_SAMPLE_SIZE;
	static const unsigned int ROWS_SAMPLE_COUNT;

	TextReader(unsigned int buffer_size = DEFAULT_BUFFER_SIZE) throw (ReaderException);
	virtual ~TextReader();

	void open() throw (ReaderException);
	void close() throw (ReaderException);
	int read_line() throw (ReaderException);
	void reset() throw (ReaderException);
	bool eof();
	bool sof();
	bool is_open();
	bool is_compressed();
	int get_buffer_size();
	unsigned long int estimate_lines_count() throw (ReaderException);
	void detect_field_separators(char* header_separator, char* data_separator) throw (ReaderException);
};

#endif
