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

#include "include/Reader.h"

Reader::Reader(char** buffer) : file_name(NULL), line(buffer) {

}

Reader::~Reader() {
	free(file_name);
	file_name = NULL;
}

void Reader::set_file_name(const char* file_name) throw (ReaderException) {
	if (file_name == NULL) {
		throw ReaderException("Reader", "set_file_name( const char* )", __LINE__, 0, "file_name");
	}

	if (strlen(file_name) <= 0) {
		throw ReaderException("Reader", "set_file_name( const char* )", __LINE__, 1, "file_name");
	}

	free(this->file_name);
	this->file_name = NULL;
	this->file_name = (char*)malloc((strlen(file_name) + 1) * sizeof(char));
	if (this->file_name == NULL) {
		throw ReaderException("Reader", "set_file_name( const char* )", __LINE__, 2, (strlen(file_name) + 1) * sizeof(char));
	}
	strcpy(this->file_name, file_name);
}

const char* Reader::get_file_name() {
	return file_name;
}

unsigned long int Reader::estimate_lines_count() throw (ReaderException) {
	return 0;
}

void Reader::detect_field_separators(char* header_separator, char* data_separator) throw (ReaderException) {

}
