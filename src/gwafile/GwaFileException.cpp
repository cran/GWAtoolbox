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

#include "include/GwaFileException.h"

const int GwaFileException::MESSAGE_TEMPLATES_NUMBER = 16;
const char* GwaFileException::MESSAGE_TEMPLATES[] = {
/*00*/	"The '%s' argument has NULL value.",
/*01*/	"The '%s' argument has an invalid value.",
/*02*/	"Memory allocation error (%d bytes).",
/*03*/	"Too few levels are specified in the %s command for the '%s' file. Strictly two levels are required.",
/*04*/	"Too many levels are specified in the %s command for the '%s' file. Strictly two levels are required.",
/*05*/	"Too few thresholds are specified in the high quality imputation filter %s for the '%s' file. Strictly two thresholds are required.",
/*06*/	"Too many thresholds are specified in the high quality imputation filter %s for the '%s' file. Strictly two thresholds are required.",
/*07*/	"Too few thresholds are specified in the %s column for the '%s' file. Strictly %d threshold(s) are required.",
/*08*/	"Too many thresholds are specified in the %s column for the '%s' file. Strictly %d threshold(s) are required.",
/*09*/	"The mandatory output prefix is not specified in the %s command for the '%s' file.",
/*10*/	"Error while checking processing instructions for the '%s' file.",
/*11*/	"Error while estimating size of '%s' file.",
/*12*/	"The name of the file with regions for annotation is not specified in the %s command for the '%s' file.",
/*13*/	"Negative value is specified in the %s command for the '%s' file. Strictly positive values or 0 are required.",
/*14*/	"Names of the files with LD values for independent SNPs selection are not specified in the %s commands for the '%s' file.",
/*15*/	"Field separators in %s file don't match field separators in previously declared files in the %s commands for the '%s' file."
};

GwaFileException::GwaFileException() : Exception() {

}

GwaFileException::GwaFileException(int message_template_index, ... ) : Exception()  {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

GwaFileException::GwaFileException(const char* class_name, const char* method_name, int source_line) : Exception(class_name, method_name, source_line)  {

}

GwaFileException::GwaFileException(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ) : Exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

GwaFileException::GwaFileException(const Exception& exception) : Exception(exception) {

}

GwaFileException::~GwaFileException() throw() {

}

const char* GwaFileException::get_message_template(int message_template_index) {
	if ((message_template_index >= 0) && (message_template_index < MESSAGE_TEMPLATES_NUMBER)) {
		return  MESSAGE_TEMPLATES[message_template_index];
	}

	return NULL;
}
