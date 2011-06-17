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

#include "../../include/analyzer/AnalyzerException.h"

const char* AnalyzerException::MESSAGES[] = {
/*00*/	"Error in memory allocation.",
/*01*/	"Error while opening the file: ",
/*02*/	"Error while closing the file: ",
/*03*/	"Error while reading the file: ",
/*04*/	"Error while writing the file: ",
/*05*/	"Undefined type of quality check for the column: ",
/*06*/	"Failed to automatically detect the column separator.",
/*07*/	"The column separator of the file header is missing.",
/*08*/	"The column separator of the file header has the undefined value: ",
/*09*/	"The column separator of the file data rows is missing.",
/*10*/	"The column separator of the file data rows has the undefined value: ",
/*11*/	"The marker for the missing values (NA) is missing.",
/*12*/	"The marker for the missing values (NA) contains the column separator.",
/*13*/	"The verbosity level is undefined.",
/*14*/	"The MAF levels are not specified.",
/*15*/	"Too small number of the MAF levels is specified. Strictly two levels are required.",
/*16*/	"Too big number of the MAF levels is specified. Strictly two levels are required.",
/*17*/	"The IMP levels are not specified.",
/*18*/	"Too small number of the IMP levels is specified. Strictly two levels are required.",
/*19*/	"Too big number of the IMP levels is specified. Strictly two levels are required.",
/*20*/	"The high quality imputation filter (HQ_SNP) is not specified.",
/*21*/	"Too small number of thresholds is specified in the high quality imputation filter (HQ_SNP). Strictly two thresholds are required.",
/*22*/	"Too big number of thresholds is specified in the high quality imputation filter (HQ_SNP). Strictly two thresholds are required.",
/*23*/	"The implausible values filter is missing for the column: ",
/*24*/	"Too small number (strictly two are required) of thresholds is specified in the implausible values filter for the column: ",
/*25*/	"Too big number (strictly two are required) of thresholds is specified in the implausible values filter for the column: ",
/*26*/	"The output prefix (PREFIX) is not specified for the file: ",
/*27*/	"Failed to convert the string into a numeric value: ",
/*28*/	"Missing column(s) in the file row: ",
/*29*/	"Too many columns in the file row: ",
/*30*/	"The empty file row has been detected: "
};

AnalyzerException::AnalyzerException(int message):
	exception(), method(NULL), argument(NULL), value(NULL), message(message) {

}

AnalyzerException::AnalyzerException(int message, const char* value):
	exception(), method(NULL), argument(NULL), value(NULL), message(message) {

	if ((value != NULL) && (strlen(value) > 0)) {
		this->value = (char*)malloc((strlen(value) + 1) * sizeof(char));
		if (this->value != NULL) {
			strcpy(this->value, value);
		}
	}
}

AnalyzerException::AnalyzerException(int message, int value):
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

AnalyzerException::AnalyzerException(int message, const char* method, const char* argument, const char* value):
	exception(), method(method), argument(argument), value(NULL), message(message) {

	if ((value != NULL) && (strlen(value) > 0)) {
		this->value = (char*)malloc((strlen(value) + 1) * sizeof(char));
		if (this->value != NULL) {
			strcpy(this->value, value);
		}
	}
}

AnalyzerException::AnalyzerException(int message, const char* method, const char* argument, int value):
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

AnalyzerException::~AnalyzerException() throw() {
	free(value);

	method = NULL;
	argument = NULL;
	value = NULL;
}

const char* AnalyzerException::what() const throw() {
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


