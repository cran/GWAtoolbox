/*
 * Copyright � 2011 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
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

#include "../../include/columns/CorrectedStandardErrorColumn.h"

CorrectedStandardErrorColumn::CorrectedStandardErrorColumn(Column* stderr_column, double lambda) : Column(),
	stderr_column(stderr_column), lambda(lambda), sqrt_lambda(sqrt(lambda)) {

}

CorrectedStandardErrorColumn::~CorrectedStandardErrorColumn() {
	stderr_column = NULL;
}

void CorrectedStandardErrorColumn::out(ofstream& output_stream) throw (ofstream::failure) {
	numeric_value = R_strtod(stderr_column->char_value, &end_ptr);
	if ((*end_ptr != '\0') || (ISNAN(numeric_value))) {
		output_stream << "NA";
	} else {
		if (lambda > 1.0) {
			numeric_value = numeric_value * sqrt_lambda;
		}
		output_stream << numeric_value;
	}
}
