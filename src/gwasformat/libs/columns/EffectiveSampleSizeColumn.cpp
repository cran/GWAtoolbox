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

#include "../../include/columns/EffectiveSampleSizeColumn.h"

EffectiveSampleSizeColumn::EffectiveSampleSizeColumn(Column* n_total_column, Column* oevar_imp_column) : Column(),
	n_total_column(n_total_column), oevar_imp_column(oevar_imp_column),
	n_total(numeric_limits<double>::quiet_NaN()), oevar_imp(numeric_limits<double>::quiet_NaN()), end_ptr(NULL) {

}

EffectiveSampleSizeColumn::~EffectiveSampleSizeColumn() {
	n_total_column = NULL;
	oevar_imp_column = NULL;
}

void EffectiveSampleSizeColumn::out(ofstream& output_stream) throw (ofstream::failure) {
	n_total = R_strtod(n_total_column->char_value, &end_ptr);
	if ((*end_ptr != '\0') || (isnan(n_total))) {
		output_stream << "NA";
	} else {
		oevar_imp = R_strtod(oevar_imp_column->char_value, &end_ptr);
		if ((*end_ptr != '\0') || (isnan(oevar_imp))) {
			output_stream << "NA";
		} else {
			n_total = n_total * oevar_imp;
			output_stream << n_total;
		}
	}
}
