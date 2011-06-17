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

#include "../../include/annotation/AnnotationException.h"

const char* AnnotationException::MESSAGES[] = {
/*00*/	"Error while allocating memory block for a text line buffer.",
/*01*/	"Error while allocating memory block for a copy of text line.",
/*02*/	"Error while opening the file: ",
/*03*/	"Error while closing the file: ",
/*04*/	"Error while reading the file: ",
/*05*/	"Error while writing the file: ",
/*06*/	"Incorrect row in the input file: ",
/*07*/	"Column with a chromosome name was not found: ",
/*08*/	"Column with a gene name was not found: ",
/*09*/	"Column with a gene start position was not found: ",
/*10*/	"Column with a gene end position was not found: ",
/*11*/	"Column with a marker name was not found: ",
/*12*/	"Column with a marker position was not found: ",
/*13*/	"A chromosome name is missing: ",
/*14*/	"A gene name is missing: ",
/*15*/	"A gene start position value is missing: ",
/*16*/	"A gene end position value is missing: ",
/*17*/	"A marker name is missing: ",
/*18*/	"A marker position value is missing: ",
/*19*/	"A column value is missing: ",
/*20*/	"A gene start position value is not a valid number: ",
/*21*/	"A gene end position value is not a valid number: ",
/*22*/	"A marker position value is not a valid number: ",
/*23*/	"An r^2 value is not a valid number: ",
/*24*/	"A hotspot start position value is not a valid number: ",
/*25*/	"A hotspot end position value is not a valid number: ",
/*26*/	"A hotspot start and end position values are incorrect: ",
/*27*/	"Error while allocating memory block for a gene name: ",
/*28*/	"Error while allocating memory block for a chromosome name: ",
/*29*/	"Error while allocating memory block for a marker name: ",
/*30*/	"Unable to detect a column seperator: "
};

AnnotationException::AnnotationException(int message) :
		exception(), value(NULL), message(message) {

}

AnnotationException::AnnotationException(int message, const char* value) :
		exception(), value(value), message(message) {

}

AnnotationException::~AnnotationException() throw() {
	value = NULL;
}

const char* AnnotationException::what() const throw() {
	stringstream string_stream;

	string_stream << setfill(' ');
	string_stream << MESSAGES[message];

	if ((value != NULL) && (strlen(value) > 0)) {
		string_stream << value;
	}

	string_stream << endl;

	return string_stream.str().c_str();
}
