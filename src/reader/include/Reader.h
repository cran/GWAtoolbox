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

#ifndef READER_H_
#define READER_H_

#include "ReaderException.h"

class Reader {
protected:
	char* file_name;
	bool compressed;

public:
	static const int SEPARATORS_NUMBER;
	static const char separators[];

	char* const* line;

	Reader(char** buffer);
	virtual ~Reader();

	void set_file_name(const char* file_name) throw (ReaderException);
	const char* get_file_name();

	virtual void open() throw (ReaderException) = 0;
	virtual void close() throw (ReaderException) = 0;
	virtual int read_line() throw (ReaderException) = 0;
	virtual void reset() throw (ReaderException) = 0;
	virtual bool eof() = 0;
	virtual bool is_open() = 0;
	virtual bool is_compressed() = 0;
	virtual unsigned long int estimate_lines_count() throw (ReaderException);
	virtual void detect_field_separators(char* header_separator, char* data_separator) throw (ReaderException);
};

#endif
