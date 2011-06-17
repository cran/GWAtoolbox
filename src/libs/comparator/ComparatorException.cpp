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

#include "../../include/comparator/ComparatorException.h"

const char* ComparatorException::MESSAGES[] = {
/*00*/	"Error in memory allocation.",
/*01*/	"Error in memory reallocation.",
/*02*/	"Error while opening the file: ",
/*03*/	"Error while closing the file: ",
/*04*/	"Error while reading the file: ",
/*05*/	"Error while writing the file: ",
/*06*/	"Failed to automatically detect the column separator for the file: ",
/*07*/	"The column separator of the header is missing for the file: ",
/*08*/	"The column separator of the header has the undefined value for the file: ",
/*09*/	"The column separator of the data rows is missing for the file: ",
/*10*/	"The column separator of the data rows has the undefined value for the file: ",
/*11*/	"The column was not found: ",
/*12*/	"Missing column(s) in the file row: ",
/*13*/	"Bad direction for the SNP: "
};

ComparatorException::ComparatorException(int message):
	exception(), method(NULL), argument(NULL), value(NULL), message(message) {

}

ComparatorException::ComparatorException(int message, const char* value):
	exception(), method(NULL), argument(NULL), value(NULL), message(message) {

	if ((value != NULL) && (strlen(value) > 0)) {
		this->value = (char*)malloc((strlen(value) + 1) * sizeof(char));
		if (this->value != NULL) {
			strcpy(this->value, value);
		}
	}
}

ComparatorException::ComparatorException(int message, int value):
	exception(), method(NULL), argument(NULL), value(NULL), message(message) {

	stringstream string_stream;
	const char* value_copy = NULL;

	string_stream << value;

	value_copy = string_stream.str().c_str();

	this->value = (char*)malloc((strlen(value_copy) + 1) * sizeof(char));
	if (this->value != NULL) {
		strcpy(this->value, value_copy);
	}
}


ComparatorException::ComparatorException(int message, const char* method, const char* argument, const char* value):
	exception(), method(method), argument(argument), value(NULL), message(message) {

	if ((value != NULL) && (strlen(value) > 0)) {
		this->value = (char*)malloc((strlen(value) + 1) * sizeof(char));
		if (this->value != NULL) {
			strcpy(this->value, value);
		}
	}
}

ComparatorException::ComparatorException(int message, const char* method, const char* argument, int value):
	exception(), method(method), argument(argument), value(NULL), message(message) {

	stringstream string_stream;
	const char* value_copy = NULL;

	string_stream << value;

	value_copy = string_stream.str().c_str();

	this->value = (char*)malloc((strlen(value_copy) + 1) * sizeof(char));
	if (this->value != NULL) {
		strcpy(this->value, value_copy);
	}
}

const char* ComparatorException::what() const throw() {
	stringstream string_stream;

	string_stream << setfill(' ');

	if (((method == NULL) || (strlen(method) <= 0)) && ((argument == NULL) || (strlen(argument) <= 0))) {
		string_stream << MESSAGES[message];
		if ((value != NULL) && (strlen(value) > 0)) {
			string_stream << value << endl;
		}
		else {
			string_stream << endl;
		}
	}
	else {
		string_stream << MESSAGES[message] << endl;

		if ((method != NULL) && (strlen(method) > 0)) {
			string_stream << setw(12) << left << "Method name" << " : " << method << endl;
		}

		if ((argument != NULL) && (strlen(argument) > 0)) {
			string_stream << setw(12) << left << "Argument" << " : " << argument << endl;
		}

		if ((value != NULL) && (strlen(value) > 0)) {
			string_stream << setw(12) << left << "Value" << " : " << value << endl;
		}
	}

	return string_stream.str().c_str();
}

ComparatorException::~ComparatorException() throw() {
	free(value);

	method = NULL;
	argument = NULL;
	value = NULL;
}
