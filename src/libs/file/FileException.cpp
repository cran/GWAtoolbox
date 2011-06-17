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

#include "../../include/file/FileException.h"

const char* FileException::MESSAGES[] = {
/*00*/	"Error in memory allocation.",
/*01*/	"The NULL argument was passed to the method.",
/*02*/	"The empty argument was passed to the method.",
/*03*/	"Error while adding a new column to the file object.",
/*04*/	"Error while getting a column from the file object.",
/*05*/	"Error while deleting a column from the file object.",
/*06*/	"Error while adding a new property to the file object.",
/*07*/	"Error while getting a property from the file object.",
/*08*/	"Error while deleting a column from the file object.",
/*09*/	"Error while adding a new threshold to the file object.",
/*10*/	"Error while getting a threshold from the file object.",
/*11*/	"Error while deleting a threshold from the file object.",
/*12*/	"Error while copying columns from the file object.",
/*13*/	"Error while copying properties from the file object.",
/*14*/	"Error while copying thresholds from the file object."
};

FileException::FileException(int message): exception(),
	source(NULL), file(NULL), method(NULL), argument(NULL), value(NULL), message(message) {

}

FileException::FileException(int message, const char* file, const char* method, const char* argument, const char* value) :
	exception(), source(NULL), file(file), method(method), argument(argument), value(NULL), message(message) {

	if ((value != NULL) && (strlen(value) > 0)) {
		this->value = (char*)malloc((strlen(value) + 1) * sizeof(char));
		if (this->value != NULL) {
			strcpy(this->value, value);
		}
	}
}

FileException::~FileException() throw () {
	if (source != NULL) {
		delete source;
		source = NULL;
	}

	free(value);

	method = NULL;
	argument = NULL;
	value = NULL;
}

void FileException::set_source(FileException& source) {
	this->source = new FileException(source.message, source.file, source.method, source.argument, source.value);
}

FileException* FileException::get_source() {
	return source;
}

const char* FileException::what() const throw() {
	stringstream string_stream;

	string_stream << setfill(' ');
	string_stream << MESSAGES[message] << endl;

	if ((file != NULL) && (strlen(file) > 0)) {
		string_stream << setw(12) << left << "File name" << " : " << file << endl;
	}

	if ((method != NULL) && (strlen(method) > 0)) {
		string_stream << setw(12) << left << "Method name" << " : " << method << endl;
	}

	if ((argument != NULL) && (strlen(argument) > 0)) {
		string_stream << setw(12) << left << "Argument" << " : " << argument << endl;
	}

	if ((value != NULL) && (strlen(value) > 0)) {
		string_stream << setw(12) << left << "Value" << " : " << value << endl;
	}

	if (source != NULL) {
		string_stream << source->what();
	}

	return string_stream.str().c_str();
}
