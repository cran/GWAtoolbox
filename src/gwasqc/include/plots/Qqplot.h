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

#ifndef QQPLOT_H_
#define QQPLOT_H_

#include <limits>
#include <math.h>
#include <iostream>

#include "PlotException.h"

#include "R.h"
#include "Rinternals.h"

extern "C" {
	SEXP Qqplot2Robj(SEXP qqplot);
}

class Qqplot {
private:
	double* x;
	double* y;
	int* size;
	int* points;
	char** names;
	char** colors;
	double* lambdas;

	int all_size;
	int count;

	Qqplot();

public:
	virtual ~Qqplot();

	void set_lambda(int group, double lambda) throw (PlotException);

	const double* get_x();
	const double* get_y();
	const int* get_size();
	const int* get_points();
	int get_count();
	const char* const* get_names();
	const char* const* get_colors();
	const double* get_lambdas();

	static Qqplot* create(const char* name, const char* color, double* sorted_data, double lambda, int data_size) throw (PlotException);
	static Qqplot* merge(Qqplot* to, Qqplot* from) throw (PlotException);

	friend SEXP Qqplot2Robj(SEXP qqplot);
};

#endif
