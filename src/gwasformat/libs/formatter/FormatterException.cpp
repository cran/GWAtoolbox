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

#include "../../include/formatter/FormatterException.h"

const int FormatterException::MESSAGE_TEMPLATES_NUMBER = 15;
const char* FormatterException::MESSAGE_TEMPLATES[] = {
/*00*/	"The '%s' argument has NULL value.",
/*01*/	"The '%s' argument has an invalid value.",
/*02*/	"Memory allocation error (%d bytes).",
/*03*/	"Error while opening '%s' GWAS file.",
/*04*/	"Error while closing '%s' GWAS file.",
/*05*/	"The header is missing in '%s' GWAS file.",
/*06*/	"Error while processing header in '%s' GWAS file.",
/*07*/	"Line %d is empty in '%s' GWAS file.",
/*08*/	"Column '%s' is missing in %d line in '%s' GWAS file.",
/*09*/	"Error while calculating inflation factor for '%s' GWAS file.",
/*10*/	"Error while initializing an output file name.",
/*11*/	"Error while opening '%s' output file.",
/*12*/	"Error while closing '%s' output file.",
/*13*/	"Error while writing '%s' output file.",
/*14*/	"Error while changing header and column separators in '%s' GWAS file."
};

FormatterException::FormatterException() : Exception() {

}

FormatterException::FormatterException(int message_template_index, ... ) : Exception()  {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

FormatterException::FormatterException(const char* class_name, const char* method_name, int source_line) : Exception(class_name, method_name, source_line)  {

}

FormatterException::FormatterException(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ) : Exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

FormatterException::FormatterException(const Exception& exception) : Exception(exception) {

}

FormatterException::~FormatterException() throw() {

}

const char* FormatterException::get_message_template(int message_template_index) {
	if ((message_template_index >= 0) && (message_template_index < MESSAGE_TEMPLATES_NUMBER)) {
		return  MESSAGE_TEMPLATES[message_template_index];
	}

	return NULL;
}
