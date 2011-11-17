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

#include "../../include/columns/CorrectedPvalueColumn.h"

CorrectedPvalueColumn::CorrectedPvalueColumn(Column* pvalue_column, double lambda) : Column(),
	pvalue_column(pvalue_column), lambda(lambda) {

}

CorrectedPvalueColumn::~CorrectedPvalueColumn() {
	pvalue_column = NULL;
}

void CorrectedPvalueColumn::out(ofstream& output_stream) throw (ofstream::failure) {
	numeric_value = R_strtod(pvalue_column->char_value, &end_ptr);
	if ((*end_ptr != '\0') || (isnan(numeric_value))) {
		output_stream << "NA";
	} else {
		if (lambda > 1.0) {
			numeric_value = Rf_pchisq((Rf_qchisq(numeric_value, 1, 0, 0) / lambda), 1, 0, 0);
		}
		output_stream << numeric_value;
	}
}
