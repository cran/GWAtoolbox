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

#ifndef METANUMERIC_H_
#define METANUMERIC_H_

#include <string>
#include <limits>
#include <iostream>

#include "../../../auxiliary/include/auxiliary.h"
#include "Meta.h"
#include "../plots/Plottable.h"

using namespace auxiliary;

class MetaNumeric : public Meta, public Plottable {
protected:
	char* end_ptr;
	double d_value;

	int n;
	int na;
	double mean;
	double sd;
	double min;
	double max;
	double median;
	double skew;
	double kurtosis;
	double quantiles[9][2];

	bool numeric;
	bool na_value;
	bool value_saved;
	double value;

	double* data;
	double* new_data;
	int current_heap_size;

	char* color;

	bool create_histogram;
	bool create_boxplot;
	bool create_qqplot;

	Histogram* histogram;
	Boxplot* boxplot;
	Qqplot* qqplot;
	Plot* plot;

public:
	bool print_min;
	bool print_max;
	bool print_median;
	bool print_skewness;
	bool print_kurtosis;
	bool print_quantiles;

	MetaNumeric(unsigned int heap_size = HEAP_SIZE) throw (MetaException);
	virtual ~MetaNumeric();

	void put(char* value) throw (MetaException);
	void finalize() throw (MetaException);
	bool is_na();
	void print(ostream& stream);
	void print_html(ostream& stream, char path_separator);
	double get_memory_usage();

	const char* get_plot_name();
	bool is_plottable();
	void set_color(const char* color) throw (PlotException);
	const char* get_color();
	void set_plots(bool histogram, bool boxplot, bool qqplot);
	void set_plot(Plot* plot);
	bool plots_histogram();
	bool plots_boxplot();
	bool plots_qqplot();
	Histogram* get_histogram();
	Boxplot* get_boxplot();
	Qqplot* get_qqplot();
	Plot* get_plot();

	bool is_numeric();
	bool is_value_saved();
	double get_value();
	const double* get_data();
	void set_ouptut(bool min, bool max, bool median, bool skewness, bool kurtosis, bool quantiles);
	int get_n();
	int get_na();
	double get_mean();
	double get_sd();
	double get_min();
	double get_max();
	double get_median();
	double get_skew();
	double get_kurtosis();
	double get_quantile(double quantile);
};

#endif
