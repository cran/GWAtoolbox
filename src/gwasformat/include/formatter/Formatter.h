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

#ifndef FORMATTER_H_
#define FORMATTER_H_

#include <stdexcept>
#include <algorithm>

#include "../../../auxiliary/include/auxiliary.h"
#include "../../../reader/include/TextReader.h"
#include "../../../gwafile/include/GwaFile.h"
#include "../columns/Column.h"
#include "../columns/CorrectedPvalueColumn.h"
#include "../columns/CorrectedStandardErrorColumn.h"
#include "../columns/EffectiveSampleSizeColumn.h"
#include "FormatterException.h"

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

class Formatter {
private:
	TextReader reader;
	GwaFile* gwafile;

	vector<Column*> input_columns;
	vector<Column*> output_columns;
	vector<Column*>::iterator columns_it;

	Column* maf_column;
	Column* pvalue_column;
	Column* stderr_column;
	Column* n_total_column;
	Column* oevar_imp_column;

	int pvalue_column_pos;
	int maf_column_pos;
	int oevar_imp_column_pos;

	static bool compare_columns(Column* first, Column* second);
	static bool compare_columns_by_name(Column* first, Column* second);

public:
	static const unsigned int HEAP_SIZE;
	static const unsigned int HEAP_INCREMENT;
	static const double EPSILON;

	Formatter();
	virtual ~Formatter();

	void open_gwafile(GwaFile* gwafile) throw (FormatterException);
	void close_gwafile() throw (FormatterException);

	void process_header() throw (FormatterException);
	double calculate_lambda(int& n_total, int& n_filtered) throw (FormatterException);
	void format(double lambda, char new_separator, int& n_total, int& n_filtered) throw (FormatterException);
};

#endif
