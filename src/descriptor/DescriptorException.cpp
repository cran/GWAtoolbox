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

#include "include/DescriptorException.h"

const int DescriptorException::MESSAGE_TEMPLATES_NUMBER = 20;
const char* DescriptorException::MESSAGE_TEMPLATES[] = {
/*00*/	"The '%s' argument has NULL value.",
/*01*/	"The '%s' argument has an invalid value.",
/*02*/	"Memory allocation error (%d bytes).",
/*03*/	"Error while creating an instance of the Descriptor class.",
/*04*/	"Error while converting '%s' string into a floating point number.",
/*05*/	"Error while adding a column to an instance of the Descriptor class.",
/*06*/	"Error while adding a property to an instance of the Descriptor class.",
/*07*/	"Error while adding a threshold to an instance of the Descriptor class.",
/*08*/	"Error while removing a column from an instance of the Descriptor class.",
/*09*/	"Error while removing a property from an instance of the Descriptor class.",
/*10*/	"Error while removing thresholds from an instance of the Descriptor class.",
/*11*/	"Error while retrieving a column from an instance of the Descriptor class.",
/*12*/	"Error while retrieving a property from an instance of the Descriptor class.",
/*13*/	"Error while retrieving a property from an instance of the Descriptor class.",
/*14*/	"Error while processing '%s' script with the instructions.",
/*15*/	"The specified '%s' value for the %s command is not supported.",
/*16*/	"Error while adding a new column name to an instance of the Descriptor class.",
/*17*/	"Error while removing a new column name from an instance of the Descriptor class.",
/*18*/	"Error while retrieving a new column name from an instance of the Descriptor class.",
/*19*/	"Error while adding a column to the ordered list of columns in the Descriptor class."
};

DescriptorException::DescriptorException() : Exception() {

}

DescriptorException::DescriptorException(int message_template_index, ... ) : Exception()  {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

DescriptorException::DescriptorException(const char* class_name, const char* method_name, int source_line) : Exception(class_name, method_name, source_line)  {

}

DescriptorException::DescriptorException(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ) : Exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

DescriptorException::DescriptorException(const Exception& exception) : Exception(exception) {

}

DescriptorException::~DescriptorException() throw() {

}

const char* DescriptorException::get_message_template(int message_template_index) {
	if ((message_template_index >= 0) && (message_template_index < MESSAGE_TEMPLATES_NUMBER)) {
		return  MESSAGE_TEMPLATES[message_template_index];
	}

	return NULL;
}
