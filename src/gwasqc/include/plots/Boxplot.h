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

#ifndef BOXPLOT_H_
#define BOXPLOT_H_

#include <limits>
#include <math.h>
#include <iostream>
#include <iomanip>

#include "PlotException.h"

#include "R.h"
#include "Rinternals.h"

extern "C" {
	SEXP Boxplot2Robj(SEXP boxplot);
}

class Boxplot {
private:
	/* (Tukey's summary) lower whisker, lower hinge, median, upper hinge, upper whisker */
	double* tukey;
	int* observations;

	/* Lower notch, upper notch */
	double* notches;
	double* outliers;
	int* groups;
	char**	names;
	double* widths;

	/* Quantiles */
	double quantiles[5];

	int outliers_cnt;
	int groups_cnt;

	char* common_name;
	char* title;

	Boxplot();

public:
	static const int TUKEY_SIZE;
	static const int NOTCHES_SIZE;

	virtual ~Boxplot();

	void set_name(int group, const char* name) throw (PlotException);
	void set_name(int group, const char* name, unsigned int) throw (PlotException);
	void set_common_name(const char* name) throw (PlotException);
	void set_width(int group, double) throw (PlotException);
	void set_quantiles(double q0, double q25, double q5, double q75, double q1);
	const double* get_tukey();
	const int* get_observations();
	const double* get_notches();
	const double* get_outliers();
	const int* get_groups();
	const char* const* get_names();
	const double* get_widths();
	double get_quantile(int q);
	int get_outliers_cnt();
	int get_groups_cnt();
	const char* get_common_name();
	void set_title(const char* title) throw (PlotException);
	const char* get_title();

	static Boxplot* create(const char* name, double* sorted_data, int data_size, double median) throw (PlotException);
	static Boxplot* merge(Boxplot* to, Boxplot* from) throw (PlotException);

	friend SEXP Boxplot2Robj(SEXP boxplot);
};

#endif
