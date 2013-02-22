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

#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include <math.h>
/*#include <R_ext/Applic.h>*/
#include <iostream>

#include "PlotException.h"

#include "R.h"
#include "Rmath.h"
#include "Rinternals.h"

extern "C" {
	SEXP Histogram2Robj(SEXP histogram);
}

class Histogram {
private:
	double* breaks;
	int* counts;
	double* density;
	double* intensities;
	double* mids;
	char* xname;

	int nclass;

	char* title;

	Histogram();

	static double pretty(double *lo, double *up, int *ndiv, int min_n, double shrink_sml, double high_u_fact[], int eps_correction, int return_bounds);

public:
	virtual ~Histogram();

	const double* get_breaks();
	const int* get_counts();
	const double* get_density();
	const double* get_intensities();
	const double* get_mids();
	const char* get_xname();
	int get_nclass();
	void set_title(const char* title) throw (PlotException);
	const char* get_title();

	static Histogram* create(const char* name, double* sorted_data, int data_size, int nclass) throw (PlotException);

	friend SEXP Histogram2Robj(SEXP histogram);
};

#endif
