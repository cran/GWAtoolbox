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

#include "include/ReaderException.h"

const int ReaderException::MESSAGE_TEMPLATES_NUMBER = 11;
const char* ReaderException::MESSAGE_TEMPLATES[] = {
/*00*/	"The '%s' argument has NULL value.",
/*01*/	"The '%s' argument has an invalid value.",
/*02*/	"Memory allocation error (%d bytes).",
/*03*/	"Error while opening '%s' file.",
/*04*/	"Error while reading '%s' file.",
/*05*/	"Error while closing '%s' file.",
/*06*/	"Error while setting the read position in '%s' file.",
/*07*/	"Error while inspecting lines in '%s' file.",
/*08*/	"Failed to automatically detect the field separator in '%s' file.",
/*09*/	"Error while getting the read position in '%s' file.",
/*10*/	"Error while initializing reading facilities for '%s' file."
};

ReaderException::ReaderException() : Exception() {

}

ReaderException::ReaderException(int message_template_index, ... ) : Exception()  {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

ReaderException::ReaderException(const char* class_name, const char* method_name, int source_line) : Exception(class_name, method_name, source_line)  {

}

ReaderException::ReaderException(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ) : Exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

ReaderException::ReaderException(const Exception& exception) : Exception(exception) {

}

ReaderException::~ReaderException() throw() {

}

const char* ReaderException::get_message_template(int message_template_index) {
	if ((message_template_index >= 0) && (message_template_index < MESSAGE_TEMPLATES_NUMBER)) {
		return  MESSAGE_TEMPLATES[message_template_index];
	}

	return NULL;
}
