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

#ifndef GZIPREADER_H_
#define GZIPREADER_H_

#include "Reader.h"
#include "../../zlib/zlib.h"

class GzipReader: public Reader {
private:
	gzFile infile;

	bool opened;

	int buffer_size;
	char* buffer;

public:
	static const unsigned int DEFAULT_BUFFER_SIZE;
	static const unsigned int TOP_ROWS_NUMBER;

	GzipReader(unsigned int buffer_size = DEFAULT_BUFFER_SIZE) throw (ReaderException);
	virtual ~GzipReader();

	void open() throw (ReaderException);
	void close() throw (ReaderException);
	int read_line() throw (ReaderException);
	void reset() throw (ReaderException);
	bool eof();
	bool sof();
	bool is_open();
	bool is_compressed();
	int get_buffer_size();
	void detect_field_separators(char* header_separator, char* data_separator) throw (ReaderException);
};

#endif
