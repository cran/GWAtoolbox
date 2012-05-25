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

#include "include/AnnotatorException.h"

const int AnnotatorException::MESSAGE_TEMPLATES_NUMBER = 23;
const char* AnnotatorException::MESSAGE_TEMPLATES[] = {
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
/*11*/	"'%s' column contains negative (%d) value on line %d.",
/*12*/	"'%s' column contains value (%d) greater than value (%d) in column '%s' on line %d.",
/*13*/	"Line %d is empty in '%s' file.",
/*14*/	"Error while reading '%s' file.",
/*15*/	"Error while indexing regions.",
/*16*/	"Error while initializing an output file name.",
/*17*/	"Error while opening '%s' output file.",
/*18*/	"Error while closing '%s' output file.",
/*19*/	"Error while writing '%s' output file.",
/*20*/	"Error while indexing map.",
/*21*/	"Error while annotating.",
/*22*/	"Memory re-allocation error."
};

AnnotatorException::AnnotatorException() : Exception() {

}

AnnotatorException::AnnotatorException(int message_template_index, ... ) : Exception()  {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

AnnotatorException::AnnotatorException(const char* class_name, const char* method_name, int source_line) : Exception(class_name, method_name, source_line)  {

}

AnnotatorException::AnnotatorException(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ) : Exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

AnnotatorException::AnnotatorException(const Exception& exception) : Exception(exception) {

}

AnnotatorException::~AnnotatorException() throw() {

}

const char* AnnotatorException::get_message_template(int message_template_index) {
	if ((message_template_index >= 0) && (message_template_index < MESSAGE_TEMPLATES_NUMBER)) {
		return  MESSAGE_TEMPLATES[message_template_index];
	}

	return NULL;
}
