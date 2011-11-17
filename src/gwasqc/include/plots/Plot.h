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

#ifndef PLOT_H_
#define PLOT_H_

#include <vector>
#include "Histogram.h"
#include "Boxplot.h"
#include "Qqplot.h"

#include "R.h"
#include "Rinternals.h"

using namespace std;

extern "C" {
	SEXP Plot2Robj(SEXP boxplot);
}

class Plot {
private:
	static const unsigned int STR_BUFFER_SIZE;

	vector<Histogram*> histograms;
	vector<Boxplot*> boxplots;
	vector<Qqplot*> qqplots;

	char* name;
	char* title;

public:
	Plot();
	virtual ~Plot();

	void add_histogram(Histogram* histogram) throw (PlotException);
	void add_boxplot(Boxplot* boxplot) throw (PlotException);
	void add_qqplot(Qqplot* qqplot) throw (PlotException);

	void set_name(const char* name) throw (PlotException);
	void set_formatted_name(const char* name, ... ) throw (PlotException);
	const char* get_name();

	void set_title(const char* title) throw (PlotException);
	void set_formatted_title(const char* title, ... ) throw (PlotException);
	const char* get_title();

	int histograms_count();
	int boxplots_count();
	int qqplots_count();

	vector<Histogram*>* get_histograms();
	vector<Boxplot*>* get_boxplots();
	vector<Qqplot*>* get_qqplots();

	friend SEXP Plot2Robj(SEXP boxplot);
};

#endif
