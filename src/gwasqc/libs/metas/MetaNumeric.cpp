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

#include "../../include/metas/MetaNumeric.h"

MetaNumeric::MetaNumeric(unsigned int heap_size) throw (MetaException) : Meta(),
	end_ptr(NULL), d_value(0.0),
	n(0), na(0), mean(0.0), sd(0.0),
	min(numeric_limits<double>::infinity()),
	max(-numeric_limits<double>::infinity()),
	median(numeric_limits<double>::quiet_NaN()),
	skew(0.0), kurtosis(0.0),
	numeric(true), na_value(false), value_saved(false), value(numeric_limits<double>::quiet_NaN()),
	data(NULL), new_data(NULL), current_heap_size(heap_size),
	color(NULL), create_histogram(true), create_boxplot(true), create_qqplot(true),
	histogram(NULL), boxplot(NULL), qqplot(NULL), plot(NULL), print_min(true),
	print_max(true), print_median(true), print_skewness(true), print_kurtosis(true),
	print_quantiles(true) {

	quantiles[0][0] = 0.00;
	quantiles[1][0] = 0.01;
	quantiles[2][0] = 0.05;
	quantiles[3][0] = 0.25;
	quantiles[4][0] = 0.50;
	quantiles[5][0] = 0.75;
	quantiles[6][0] = 0.95;
	quantiles[7][0] = 0.99;
	quantiles[8][0] = 1.00;

	if (heap_size > 0) {
		data = (double*)malloc(heap_size * sizeof(double));
		if (data == NULL) {
			throw MetaException("MetaNumeric", "MetaNumeric( unsigned int )", __LINE__, 2, heap_size * sizeof(double));
		}
	}
}

MetaNumeric::~MetaNumeric() {
	delete boxplot;
	delete histogram;
	delete qqplot;

	free(color);
	free(data);

	boxplot = NULL;
	histogram = NULL;
	qqplot = NULL;
	color = NULL;
	data = NULL;
}

void MetaNumeric::put(char* value) throw (MetaException) {
	if (numeric) {
		if (strcmp(na_marker, value) == 0) {
			na_value = true;
			na += 1;
			value_saved = false;
			this->value = numeric_limits<double>::quiet_NaN();
			return;
		}
		else {
			na_value = false;
		}

		d_value = R_strtod(value, &end_ptr);

		if (*end_ptr != '\0') {
			numeric = false;
			value_saved = false;
			this->value = numeric_limits<double>::quiet_NaN();
			free(data);
			data = NULL;
			return;
		}

		if (isnan(d_value)) {
			na_value = true;
			na += 1;
			value_saved = false;
			this->value = numeric_limits<double>::quiet_NaN();
			return;
		}

		n += 1;

		if (n > current_heap_size) {
			current_heap_size += Meta::HEAP_INCREMENT;

			new_data = (double*)realloc(data, current_heap_size * sizeof(double));
			if (new_data == NULL) {
				free(data);
				data = NULL;
				throw MetaException("MetaNumeric", "put( char* )", __LINE__, 3, current_heap_size * sizeof(double));
			}

			data = new_data;
		}

		value_saved = true;
		this->value = d_value;
		data[n - 1] = d_value;
	}
}
void MetaNumeric::finalize() throw (MetaException) {
	if (n <= 0) {
		numeric = false;
		free(data);
		data = NULL;
		return;
	}

	if (numeric) {
		mean = auxiliary::stats_mean(data, n);
		sd = auxiliary::stats_sd(data, n, mean);
		skew = auxiliary::stats_skewness(data, n, mean, sd);
		kurtosis = auxiliary::stats_kurtosis(data, n, mean, sd);

		qsort(data, n, sizeof(double), dblcmp);

		median = auxiliary::stats_median_from_sorted_data(data, n);

		for (unsigned int j = 0; j < 9; j++) {
			quantiles[j][1] = auxiliary::stats_quantile_from_sorted_data(data, n, quantiles[j][0]);
		}

		min = data[0];
		max = data[n - 1];

		try {
			if (create_histogram) {
				histogram = Histogram::create(actual_name, data, n, 1000);
				if (histogram != NULL) {
					histogram->set_title(get_description());
				}
			}

			if (create_boxplot) {
				boxplot = Boxplot::create(actual_name, data, n, median);
				if (boxplot != NULL) {
					boxplot->set_quantiles(quantiles[0][1], quantiles[3][1], quantiles[4][1], quantiles[5][1], quantiles[8][1]);
					boxplot->set_title(get_description());
				}
			}
		} catch (PlotException &e) {
			MetaException new_e(e);
			new_e.add_message("MetaNumeric", "finalize()", __LINE__, 4, actual_name != NULL ? actual_name : "NULL");
			throw new_e;
		}
	}

	free(data);
	data = NULL;
}

bool MetaNumeric::is_na() {
	return na_value;
}

const char* MetaNumeric::get_plot_name() {
	return common_name;
}

bool MetaNumeric::is_plottable() {
	return numeric;
}

void MetaNumeric::set_plots(bool histogram, bool boxplot, bool qqplot) {
	create_histogram = histogram;
	create_boxplot = boxplot;
	create_qqplot = qqplot;
}

void MetaNumeric::set_ouptut(bool min, bool max, bool median, bool skewness, bool kurtosis, bool quantiles) {
	this->print_min = min;
	this->print_max = max;
	this->print_median = median;
	this->print_skewness = skewness;
	this->print_kurtosis = kurtosis;
	this->print_quantiles = quantiles;
}

void MetaNumeric::set_plot(Plot* plot) {
	this->plot = plot;
}

bool MetaNumeric::plots_histogram() {
	return create_histogram;
}

bool MetaNumeric::plots_boxplot() {
	return create_boxplot;
}

bool MetaNumeric::plots_qqplot() {
	return create_qqplot;
}

Histogram* MetaNumeric::get_histogram() {
	return histogram;
}

Boxplot* MetaNumeric::get_boxplot() {
	return boxplot;
}

Qqplot* MetaNumeric::get_qqplot() {
	return qqplot;
}

Plot* MetaNumeric::get_plot() {
	return plot;
}

void MetaNumeric::print(ostream& stream) {
	stream << resetiosflags(stream.flags());
	stream << setfill(' ');

	if (!numeric) {
		stream << " '" << actual_name << "' : non-numeric values are present or all values are missing" << endl;
	}
	else {
		stream << resetiosflags(stream.flags());
		stream << setprecision(7);
		stream << setw(12) << left << " N" << " = " << n << endl;
		stream << setw(12) << left << " Mean" << " = " << mean << endl;
		stream << setw(12) << left << " StdDev" << " = " << sd << endl;
		if (print_min == true) {
			stream << setw(12) << left << " Min" << " = " << min << endl;
		}
		if (print_max == true) {
			stream << setw(12) << left << " Max" << " = " << max << endl;
		}
		if (print_median == true) {
			stream << setw(12) << left << " Median" << " = " << median << endl;
		}
		if (print_skewness == true) {
			stream << setw(12) << left << " Skewness" << " = " << skew << endl;
		}
		if (print_kurtosis == true) {
			stream << setw(12) << left << " Kurtosis" << " = " << kurtosis << endl;
		}
		stream << setw(12) << left << " No. of NAs" << " = " << na << endl;

		if (print_quantiles == true) {
			stream << endl;
			stream << " Quantiles:" << endl;
			stream << fixed;
			stream << setprecision(0);
			stream << " Min (" << quantiles[0][0] * 100 << "%)";
			stream << "\t = " << setprecision(3) << quantiles[0][1] << endl;
			for (unsigned int j = 1; j < 4; j++) {
				stream << setprecision(0);
				stream << " " << quantiles[j][0] * 100 << "%";
				stream << "\t\t = " << setprecision(3) << quantiles[j][1] << endl;
			}
			stream << setprecision(0);
			stream << " Median (" << quantiles[4][0] * 100 << "%)";
			stream << "\t = " << setprecision(3) << quantiles[4][1] << endl;
			for (unsigned int j = 5; j < 8; j++) {
				stream << setprecision(0);
				stream << " " << quantiles[j][0] * 100 << "%";
				stream << "\t\t = " << setprecision(3) << quantiles[j][1] << endl;
			}
			stream << setprecision(0);
			stream << " Max (" << quantiles[8][0] * 100 << "%)";
			stream << "\t = " << setprecision(3) << quantiles[8][1] << endl;
		}

		stream << resetiosflags(stream.flags());
		stream << setfill(' ');
	}
}

void MetaNumeric::print_html(ostream& stream, char path_separator) {
	if (!numeric) {
		stream << "<div class = \"result\">";
		stream << "'" << actual_name << "' : non-numeric values are present or all values are missing";
		stream << "</div>";
	}
	else {
		stream << "<div class = \"container\">";

		stream << "<div class = \"spacer\">&nbsp;</div>";

		stream << "<div class = \"float\">";
		stream << "<table class = \"groups\">";
		stream << "<tr><th colspan = \"2\">Statistics</th></tr>";
		stream << "<tr><td>N</td><td>" << n << "</td></tr>";
		stream << "<tr><td>Mean</td><td>" << mean << "</td></tr>";
		stream << "<tr><td>StdDev</td><td>" << sd << "</td></tr>";
		if (print_min == true) {
			stream << "<tr><td>Min</td><td>" << min << "</td></tr>";
		}
		if (print_max == true) {
			stream << "<tr><td>Max</td><td>" << max << "</td></tr>";
		}
		if (print_median == true) {
			stream << "<tr><td>Median</td><td>" << median << "</td></tr>";
		}
		if (print_skewness == true) {
			stream << "<tr><td>Skewness</td><td>" << skew << "</td></tr>";
		}
		if (print_kurtosis == true) {
			stream << "<tr><td>Kurtosis</td><td>" << kurtosis << "</td></tr>";
		}
		stream << "<tr><td>No. of NAs</td><td>" << na << "</td></tr>";
		stream << "</table>";
		stream << "</div>";

		if (print_quantiles == true) {
			stream << fixed;

			stream << "<div class = \"float\">";
			stream << "<table class = \"groups\">";
			stream << "<tr><th colspan = \"2\">Quantiles</th></tr>";
			stream << setprecision(0) << "<tr><td>Min (" << quantiles[0][0] * 100 << "%)</td>";
			stream << setprecision(3) << "<td>" << quantiles[0][1] << "</td></tr>";
			for (unsigned int j = 1; j < 4; j++) {
				stream << setprecision(0) << "<tr><td>" << quantiles[j][0] * 100 << "%</td>";
				stream << setprecision(3) << "<td>" << quantiles[j][1] << "</td></tr>";
			}
			stream << setprecision(0) << "<tr><td>Median (" << quantiles[4][0] * 100 << "%)</td>";
			stream << setprecision(3) << "<td>" << quantiles[4][1] << "</td></tr>";
			for (unsigned int j = 5; j < 8; j++) {
				stream << setprecision(0) << "<tr><td>" << quantiles[j][0] * 100 << "%</td>";
				stream << setprecision(3) << "<td>" << quantiles[j][1] << "</td></tr>";
			}
			stream << setprecision(0) << "<tr><td>Max (" << quantiles[8][0] * 100 << "%)</td>";
			stream << setprecision(3) << "<td>" << quantiles[8][1] << "</td></tr>";
			stream << "</table>";
			stream << "</div>";
		}

		if (get_plot() != NULL)  {
			stream << "<div class = \"float\">";
			stream << "<a href = \"" << (auxiliary::is_absolute_path(get_plot()->get_name(), path_separator) ? "file:///" : "") << get_plot()->get_name() << ".png\">";
			stream << "<img src = \"" << (auxiliary::is_absolute_path(get_plot()->get_name(), path_separator) ? "file:///" : "") << get_plot()->get_name() << ".png\" alt = \"" << get_plot()->get_name() << ".png\" title = \"Click to enlarge the figure\" width = \"400\" height = \"300\" />";
			stream << "</a>";
			stream << "</div>";
		}

		stream << "<div class = \"spacer\">&nbsp;</div>";
		stream << "</div>";

		stream << resetiosflags(stream.flags());
	}
}

void MetaNumeric::set_color(const char* color) throw (PlotException) {
	if (color == NULL) {
		throw PlotException("MetaNumeric", "set_color( const char* )", __LINE__, 0, "color");
	}

	if (strlen(color) <= 0) {
		throw PlotException("MetaNumeric", "set_color( const char* )", __LINE__, 1, "color");
	}

	free(this->color);
	this->color = NULL;
	this->color = (char*)malloc((strlen(color) + 1) * sizeof(char));
	if (this->color == NULL) {
		throw PlotException("MetaNumeric", "set_color( const char* )", __LINE__, 2, (strlen(color) + 1) * sizeof(char));
	}
	strcpy(this->color, color);
}

const char* MetaNumeric::get_color() {
	return color;
}

bool MetaNumeric::is_numeric() {
	return numeric;
}

bool MetaNumeric::is_value_saved() {
	return value_saved;
}

double MetaNumeric::get_value() {
	return value;
}

const double* MetaNumeric::get_data() {
	return data;
}

int MetaNumeric::get_n() {
	return n;
}

int MetaNumeric::get_na() {
	return na;
}

double MetaNumeric::get_mean() {
	return mean;
}

double MetaNumeric::get_sd() {
	return sd;
}

double MetaNumeric::get_min() {
	return min;
}

double MetaNumeric::get_max() {
	return max;
}

double MetaNumeric::get_median() {
	return median;
}

double MetaNumeric::get_skew() {
	return skew;
}

double MetaNumeric::get_kurtosis() {
	return kurtosis;
}

double MetaNumeric::get_quantile(double quantile) {
	for (int i = 0; i < 9; i++) {
		if (quantiles[i][0] == quantile) {
			return quantiles[i][1];
		}
	}

	return numeric_limits<double>::quiet_NaN();
}

double MetaNumeric::get_memory_usage() {
	unsigned long int memory = 0;

	memory += current_heap_size * sizeof(double);

	return memory / 1048576.0;
}

