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

#include "../../include/plots/Histogram.h"

Histogram::Histogram():
	breaks(NULL), counts(NULL), density(NULL), intensities(NULL),
	mids(NULL), xname(NULL), nclass(0), title(NULL) {

}

Histogram::~Histogram() {
	free(breaks);
	free(counts);
	free(density);
	free(mids);
	free(xname);
	free(title);

	breaks = NULL;
	counts = NULL;
	density = NULL;
	intensities = NULL;
	mids = NULL;
	xname = NULL;
	title = NULL;
}

Histogram* Histogram::create(const char* name, double* sorted_data, int data_size, int nclass) throw (PlotException) {
	Histogram* histogram = NULL;

	/* Finite data boundaries */
	double* lower_finite = NULL;
	double*	upper_finite = NULL;
	int n_finite = 0;

	/* R_pretty arguments */
	double range[2];
	double unit = 0.0;
	double tolerance = 1e-07;

	int i = 0, j = 0;
	double start = 0.0, end = 0.0;
	double denominator = 0.0;

	if (name == NULL) {
		throw PlotException(1, "Histogram", "create( ... )", "name", "NULL");
	}

	if (strlen(name) <= 0) {
		throw PlotException(2, "Histogram", "create( ... )", "name", (const char*)NULL);
	}

	if (sorted_data == NULL) {
		throw PlotException(1, "Histogram", "create( ... )", "sorted_data", "NULL");
	}

	if (data_size <= 0) {
		throw PlotException(3, "Histogram", "create( ... )", "data_size", data_size);
	}

	/* Create new histogram */
	histogram = new Histogram();

	/* Determine finite data boundaries */
	while ((i < data_size) && (isinf(sorted_data[i]))) {
		i += 1;
	}

	if (i >= data_size) {
		return NULL;
	}

	lower_finite = sorted_data + i;
	n_finite = data_size - i;

	i = data_size - 1;
	while ((i >= 0) && (isinf(sorted_data[i]))) {
		i -= 1;
	}

	if (i < 0) {
		return NULL;
	}

	upper_finite = sorted_data + i;
	n_finite = n_finite - data_size + i + 1;

	range[0] = *lower_finite;
	range[1] = *upper_finite;

	/* Calculate breaks, counts, densities and etc. */
	/* See R help(pretty) */
	double bias[] = {1.5, 0.5 + 1.5 * 1.5};
	unit = R_pretty0(range, range + 1, &nclass, 1, 0.75, bias, 0, 1);

	/* Allocate memory for breaks */
	if ((histogram->breaks = (double*)malloc((nclass + 1) * sizeof(double))) == NULL) {
		throw PlotException(0, "Histogram", "create( ... )", "breaks", (nclass + 1) * sizeof(double));
	}

	/* Allocate memory for counts */
	if ((histogram->counts = (int*)malloc(nclass * sizeof(int))) == NULL) {
		throw PlotException(0, "Histogram", "create( ... )", "counts", nclass * sizeof(int));
	}

	/* Allocate memory for density and intensities */
	if ((histogram->density = (double*)malloc(nclass * sizeof(double))) == NULL) {
		throw PlotException(0, "Histogram", "create( ... )", "density", nclass * sizeof(double));
	}

	histogram->intensities = histogram->density;

	/* Allocate memory for mids */
	if ((histogram->mids = (double*)malloc(nclass * sizeof(double))) == NULL) {
		throw PlotException(0, "Histogram", "create( ... )", "mids", nclass * sizeof(double));
	}

	/* Set number of bars */
	histogram->nclass = nclass;

	/* Calculate breaks */
	for (i = 0; i <= nclass; i++) {
		histogram->breaks[i] = range[0] + unit * i;
	}

	/* Calculate counts, densities and middle points */
	i = j = 0;
	start = histogram->breaks[j] - tolerance;
	do {
		end = histogram->breaks[j + 1] + tolerance;
		denominator = n_finite * (end - start);

		histogram->counts[j] = 0;
		while (i < n_finite) {
			if ((start < lower_finite[i]) && (lower_finite[i] <= end)) {
				histogram->counts[j] += 1;
				i += 1;
			}
			else {
				break;
			}
		}

		histogram->density[j] = histogram->counts[j] / denominator;
		histogram->mids[j] = 0.5 * (histogram->breaks[j] + histogram->breaks[j + 1]);

		start = end;
		j += 1;
	} while (j < nclass);

	/* Allocate memory for xname and copy name */
	if ((histogram->xname = (char*)malloc((strlen(name) + 1) * sizeof(char))) == NULL) {
		throw PlotException(0, "Histogram", "create( ... )", "name", name);
	}

	strcpy(histogram->xname, name);

	return histogram;
}

const double* Histogram::get_breaks() {
	return breaks;
}

const int* Histogram::get_counts() {
	return counts;
}

const double* Histogram::get_density() {
	return density;
}

const double* Histogram::get_intensities() {
	return intensities;
}

const double* Histogram::get_mids() {
	return mids;
}

const char* Histogram::get_xname() {
	return xname;
}

int Histogram::get_nclass() {
	return nclass;
}

void Histogram::set_title(const char* title) throw (PlotException) {
	if (title == NULL) {
		throw PlotException(1, "Histogram", "set_title( ... )", "title", "NULL");
	}

	if (strlen(title) <= 0) {
		throw PlotException(2, "Histogram", "set_title( ... )", "title", (const char*)NULL);
	}

	free(this->title);
	this->title = NULL;
	this->title = (char*)malloc((strlen(title) + 1) * sizeof(char));
	if (this->title == NULL) {
		throw PlotException(0, "Histogram", "set_title( ... )", "title", title);
	}

	strcpy(this->title, title);
}

const char* Histogram::get_title() {
	return title;
}
