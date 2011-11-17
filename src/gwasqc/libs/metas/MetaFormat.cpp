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

#include "../../include/metas/MetaFormat.h"

MetaFormat::MetaFormat(): Meta(), numeric(true), na_value(false), n(0) {

}

MetaFormat::~MetaFormat() {

}

void MetaFormat::put(char* value) throw (MetaException) {
	if (numeric) {
		if (strcmp(na_marker, value) == 0) {
			na_value = true;
			return;
		}
		else {
			na_value = false;
		}

		char* end_ptr = NULL;
		double d_value = 0.0;

		d_value = R_strtod(value, &end_ptr);

		if (*end_ptr != '\0') {
			numeric = false;
		}

		n += 1;
	}
}

void MetaFormat::finalize() throw (MetaException) {
	if (n == 0) {
		/* In R if a column contains all NA values, then it is not numeric. */
		numeric = false;
	}
}

bool MetaFormat::is_na() {
	return na_value;
}

void MetaFormat::print(ostream& stream) {
	stream << resetiosflags(stream.flags());
	stream << setfill(' ');

	if (!numeric) {
		stream << " '" << actual_name << "' : non-numeric values are present or all values are missing" << endl;
	}
	else {
		stream << " O.K." << endl;
	}
}

void MetaFormat::print_html(ostream& stream, char path_separator) {
	stream << resetiosflags(stream.flags());

	stream << "<div class = \"result\">";
	if (!numeric) {
		stream << "'" << actual_name << "' : non-numeric values are present or all values are missing";
	}
	else {
		stream << "O.K.";
	}
	stream << "</div>";
}

double MetaFormat::get_memory_usage() {
	return 0.0;
}
