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

#ifndef PLOTTABLE_H_
#define PLOTTABLE_H_

#include "Plot.h"
#include "Histogram.h"
#include "Boxplot.h"
#include "Qqplot.h"

class Plottable {
public:
	virtual const char* get_plot_name() = 0;
	virtual bool is_plottable() = 0;
	virtual void set_color(const char* color) throw (PlotException) = 0;
	virtual const char* get_color() = 0;
	virtual void set_plots(bool histogram, bool boxplot, bool qqplot) = 0;
	virtual void set_plot(Plot* plot) = 0;
	virtual bool plots_histogram() = 0;
	virtual bool plots_boxplot() = 0;
	virtual bool plots_qqplot() = 0;
	virtual Histogram* get_histogram() = 0;
	virtual Boxplot* get_boxplot() = 0;
	virtual Qqplot* get_qqplot() = 0;
	virtual Plot* get_plot() = 0;
};

#endif
