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

#include "../../include/columns/Column.h"

const int Column::HEADER_BUFFER_SIZE = 8000;

Column::Column() : header(NULL), order(numeric_limits<int>::max()),
	numeric_value(numeric_limits<double>::quiet_NaN()), end_ptr(NULL), char_value(NULL) {

}

Column::~Column() {
	if (header != NULL) {
		free(header);
	}
	header = NULL;
}

void Column::set_header(const char* header, ... ) throw (ColumnException) {
	va_list arguments;

	if (header == NULL) {
		throw ColumnException("Column", "set_header( const char* , ... )", __LINE__, 0, "header");
	}

	if (strlen(header) <= 0) {
		throw ColumnException("Column", "set_header( const char* , ... )", __LINE__, 1, "header");
	}

	if (this->header != NULL) {
		free(this->header);
		this->header = NULL;
	}

	this->header = (char*)malloc(HEADER_BUFFER_SIZE * sizeof(char));
	if (this->header == NULL) {
		throw ColumnException("Column", "set_header( const char* , ... )", __LINE__, 2, HEADER_BUFFER_SIZE * sizeof(char));
	}

	va_start(arguments, header);
	if (vsprintf(this->header, header, arguments) < 0) {
		free(this->header);
		this->header = NULL;
		throw ColumnException("Column", "set_header( const char* , ...)", __LINE__, 3, header);
	}
	va_end(arguments);
}

const char* Column::get_header() {
	return header;
}

void Column::set_value(char* value) {
	char_value = value;
}

char* Column::get_value() {
	return char_value;
}

double Column::get_numeric_value() {
	numeric_value = R_strtod(char_value, &end_ptr);
	if ((*end_ptr != '\0') || (isnan(numeric_value))) {
		return numeric_limits<double>::quiet_NaN();
	}
	return numeric_value;
}

void Column::set_order(int order) {
	this->order = order;
}

int Column::get_order() {
	return order;
}

void Column::out(ofstream& output_stream) throw (ofstream::failure) {
	output_stream << char_value;
}
