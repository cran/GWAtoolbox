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

	double* lower_finite = NULL;
	double*	upper_finite = NULL;
	int n_finite = 0;

	double range[2];
	double bias[] = {1.5, 0.5 + 1.5 * 1.5};
	double tolerance = 1e-07;
	double unit = 0.0;

	int i = 0, j = 0;
	double start = 0.0, end = 0.0;
	double denominator = 0.0;

	if (name == NULL) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 1, "name");
	}

	if (sorted_data == NULL) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 0, "sorted_data");
	}

	if (data_size <= 0) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 1, "data_size");
	}

	if (nclass <= 0) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 1, "nclass");
	}

	/* BEGIN: Determine finite data boundaries, ranges, breaks and etc. See R help(pretty). */
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

	unit = R_pretty0(range, range + 1, &nclass, 1, 0.75, bias, 0, 1);
	/* END: Determine finite data boundaries, ranges, breaks and etc. See R help(pretty). */

	/* BEGIN: Initialize new histogram object */
	histogram = new Histogram();

	if ((histogram->breaks = (double*)malloc((nclass + 1) * sizeof(double))) == NULL) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 2, (nclass + 1) * sizeof(double));
	}

	if ((histogram->counts = (int*)malloc(nclass * sizeof(int))) == NULL) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 2, nclass * sizeof(int));
	}

	if ((histogram->density = (double*)malloc(nclass * sizeof(double))) == NULL) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 2, nclass * sizeof(double));
	}

	if ((histogram->mids = (double*)malloc(nclass * sizeof(double))) == NULL) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 2, nclass * sizeof(double));
	}

	if ((histogram->xname = (char*)malloc((strlen(name) + 1) * sizeof(char))) == NULL) {
		throw PlotException("Histogram", "Histogram* create( ... )", __LINE__, 2, (strlen(name) + 1) * sizeof(char));
	}

	histogram->intensities = histogram->density;
	histogram->nclass = nclass;
	strcpy(histogram->xname, name);
	/* END: Initialize new histogram object */

	/* BEGIN: Calculate counts, densities and etc. */
	for (i = 0; i <= nclass; i++) {
		histogram->breaks[i] = range[0] + unit * i;
	}

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
	/* END: Calculate breaks, counts, densities and etc. See R help(pretty). */

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
		throw PlotException("Histogram", "set_title( const char* )", __LINE__, 0, "title");
	}

	if (strlen(title) <= 0) {
		throw PlotException("Histogram", "set_title( const char* )", __LINE__, 1, "title");
	}

	free(this->title);
	this->title = NULL;
	this->title = (char*)malloc((strlen(title) + 1) * sizeof(char));
	if (this->title == NULL) {
		throw PlotException("Histogram", "set_title( const char* )", __LINE__, 2, (strlen(title) + 1) * sizeof(char));
	}

	strcpy(this->title, title);
}

const char* Histogram::get_title() {
	return title;
}
