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

#include "../../include/metas/MetaImplausibleStrict.h"

MetaImplausibleStrict::MetaImplausibleStrict(int heap_size) throw (MetaException): MetaNumeric(heap_size), n_less(0), n_greater(0) {

}

MetaImplausibleStrict::MetaImplausibleStrict(double less, double greater, int heap_size) throw (MetaException): MetaNumeric(heap_size) {
	this->less = less;
	this->greater = greater;

	n_less = 0;
	n_greater = 0;
}

MetaImplausibleStrict::~MetaImplausibleStrict() {

}

void MetaImplausibleStrict::put(char* value) throw (MetaException) {
	if (numeric) {
		if (strcmp(na_marker, value) == 0) {
			na_value = true;
			na += 1;
			this->value = numeric_limits<double>::quiet_NaN();
			return;
		}
		else {
			na_value = false;
		}

		d_value = R_strtod(value, &end_ptr);

		if (*end_ptr != '\0') {
			numeric = false;
			this->value = numeric_limits<double>::quiet_NaN();
			free(data);
			data = NULL;
			return;
		}

		if (isnan(d_value)) {
			na_value = true;
			na += 1;
			this->value = numeric_limits<double>::quiet_NaN();
			return;
		}

		this->value = d_value;

		if (d_value < less) {
			n_less += 1;
			return;
		}

		if (d_value > greater) {
			n_greater += 1;
			return;
		}

		n += 1;

		if (n > current_heap_size) {
			current_heap_size += Meta::HEAP_INCREMENT;

			new_data = (double*)realloc(data, current_heap_size * sizeof(double));
			if (new_data == NULL) {
				free(data);
				data = NULL;
				throw MetaException(0, "MetaImplausibleStrict", "put( ... )", "data", current_heap_size * sizeof(double));
			}
			data = new_data;
		}

		data[n - 1] = d_value;
	}
}


void MetaImplausibleStrict::print(ostream& stream) {
	stream << resetiosflags(stream.flags());
	stream << setfill(' ');

	if (!numeric) {
		stream << " '" << actual_name << "' : non-numeric values are present or all values are missing" << endl;
	}
	else {
		stream << " Implausible values:" << endl;
		stream << " < " << setw(12) << left << less << " : " << n_less << endl;
		stream << " > " << setw(12) << left << greater << " : " << n_greater << endl;
		stream << endl;

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

void MetaImplausibleStrict::print_html(ostream& stream, const char* prefix) {
	if (!numeric) {
		stream << "<div class = \"result\">";
		stream << "'" << actual_name << "' : non-numeric values are present or all values are missing";
		stream << "</div>";
	}
	else {
		stream << resetiosflags(stream.flags());
		stream << setprecision(7);

		stream << "<div class = \"container\">";

		stream << "<div class = \"spacer\">&nbsp;</div>";

		stream << "<div class = \"float\">";
		stream << "<table class = \"groups\">";
		stream << "<tr><th colspan = \"2\">Implausible values</th></tr>";
		stream << "<tr><td>&lt; " << less << "</td><td>" << n_less << "</tr></td>";
		stream << "<tr><td>&gt; " << greater << "</td><td>" << n_greater << "</tr></td>";
		stream << "</table>";
		stream << "</div>";

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
			stream << "<a href = \"" << get_plot()->get_name() << ".png\">";
			stream << "<img src = \"" << get_plot()->get_name() << ".png\" title = \"Click to enlarge the figure\" width = \"400\" height = \"300\" />";
			stream << "</a>";
			stream << "</div>";
		}

		stream << "<div class = \"spacer\">&nbsp;</div>";
		stream << "</div>";

		stream << resetiosflags(stream.flags());
	}
}

void MetaImplausibleStrict::set_implausible(double less, double greater) {
	this->less = less;
	this->greater = greater;
}
