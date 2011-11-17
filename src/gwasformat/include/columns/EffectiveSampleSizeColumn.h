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

#ifndef EFFECTIVESAMPLESIZECOLUMN_H_
#define EFFECTIVESAMPLESIZECOLUMN_H_

#include "Column.h"

#include <R.h>

class EffectiveSampleSizeColumn: public Column {
private:
	Column* n_total_column;
	Column* oevar_imp_column;
	double n_total;
	double oevar_imp;
	char* end_ptr;

public:
	EffectiveSampleSizeColumn(Column* n_total_column, Column* oevar_imp_column);
	virtual ~EffectiveSampleSizeColumn();

	void out(ofstream& output_stream) throw (ofstream::failure);
};

#endif
