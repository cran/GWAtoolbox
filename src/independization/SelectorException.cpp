/*
 * Copyright © 2012 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
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

#include "include/SelectorException.h"

const int SelectorException::MESSAGE_TEMPLATES_NUMBER = 21;
const char* SelectorException::MESSAGE_TEMPLATES[] = {
/*00*/	"The '%s' argument has NULL value.",
/*01*/	"The '%s' argument has an invalid value.",
/*02*/	"Memory allocation error (%d bytes).",
/*03*/	"Error while opening '%s' file.",
/*04*/	"Error while closing '%s' file.",
/*05*/	"The header is missing on line %d in '%s' file.",
/*06*/	"Error while processing header in '%s' file.",
/*07*/	"Column '%s' is missing in the header of '%s' file.",
/*08*/	"Line %d in '%s' file contains less columns (%d) than expected (%d).",
/*09*/	"Line %d in '%s' file contains more columns (%d) than expected (%d).",
/*10*/	"Error while parsing '%s' value in '%s' column on line %d to an integer.",
/*11*/	"Line %d is empty in '%s' file.",
/*12*/	"Error while reading '%s' file.",



/*13*/	"VCF file line %d is empty.",
/*14*/	"Line %d in '%s' file contains less columns (%d) than expected (%d).",
/*15*/	"Line %d in '%s' file contains more columns (%d) than expected (%d).",
/*16*/	"The header is missing on line %d in '%s' file.",
/*17*/	"Column '%s' is missing in the header of '%s' file.",
/*18*/	"Error while initializing a log file name.",
/*19*/	"Memory re-allocation error (%d bytes).",
/*20*/	"The chromosomal position '%s' on line %d in file '%s' could not be parsed to unsigned integer."
};

SelectorException::SelectorException() : Exception() {

}

SelectorException::SelectorException(int message_template_index, ... ) : Exception()  {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

SelectorException::SelectorException(const char* class_name, const char* method_name, int source_line) : Exception(class_name, method_name, source_line)  {

}

SelectorException::SelectorException(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ) : Exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

SelectorException::SelectorException(const Exception& exception) : Exception(exception) {

}

SelectorException::~SelectorException() throw() {

}

const char* SelectorException::get_message_template(int message_template_index) {
	if ((message_template_index >= 0) && (message_template_index < MESSAGE_TEMPLATES_NUMBER)) {
		return  MESSAGE_TEMPLATES[message_template_index];
	}

	return NULL;
}
