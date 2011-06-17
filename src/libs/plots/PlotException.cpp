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

#include "../../include/plots/PlotException.h"

const char* PlotException::MESSAGES[] = {
/*00*/	"Error in memory allocation.",
/*01*/	"The NULL argument was passed to the method.",
/*02*/	"The empty argument was passed to the method.",
/*03*/	"Index out of bounds.",
/*04*/	"Error in memory reallocation."
};

PlotException::PlotException(int message):
	exception(), class_name(NULL), method(NULL), argument(NULL), value(NULL), message(message) {

}

PlotException::PlotException(int message, const char* class_name, const char* method, const char* argument, const char* value) :
	exception(), class_name(class_name), method(method), argument(argument), value(NULL), message(message) {

	if ((value != NULL) && (strlen(value) > 0)) {
		this->value = (char*)malloc((strlen(value) + 1) * sizeof(char));
		if (this->value != NULL) {
			strcpy(this->value, value);
		}
	}
}

PlotException::PlotException(int message, const char* class_name, const char* method, const char* argument, int value) :
	exception(), class_name(class_name), method(method), argument(argument), value(NULL), message(message) {

	stringstream string_stream;
	const char* value_copy = NULL;

	string_stream << value;

	value_copy = string_stream.str().c_str();

	this->value = (char*)malloc((strlen(value_copy) + 1) * sizeof(char));
	if (this->value != NULL) {
		strcpy(this->value, value_copy);
	}
}

PlotException::~PlotException() throw() {
	free(value);

	class_name = NULL;
	method = NULL;
	argument = NULL;
	value = NULL;
}

const char* PlotException::what() const throw() {
	stringstream string_stream;

	string_stream << setfill(' ');
	string_stream << MESSAGES[message] << endl;

	if ((class_name != NULL) && (strlen(class_name) > 0)) {
		string_stream << setw(12) << left << "Class name" << " : " << class_name << endl;
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

	return string_stream.str().c_str();
}
