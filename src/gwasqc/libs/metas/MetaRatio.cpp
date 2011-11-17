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

#include "../../include/metas/MetaRatio.h"

const unsigned int MetaRatio::SLICES_CNT = 5;
const double MetaRatio::slices[SLICES_CNT] = {
		0.5, 0.75, 0.95, 0.99, 1.0
};

MetaRatio::MetaRatio(MetaNumeric* source_a, MetaNumeric* source_b, unsigned int heap_size) throw (MetaException) : Meta(),
	source_a(source_a), source_b(source_b), n(0), data(NULL), new_data(NULL), new_value(NULL),
	current_heap_size(heap_size), skewness(NULL), kurtosis(NULL) {

	if (heap_size == 0) {
		throw MetaException("MetaRatio", "MetaRatio( MetaNumeric*, MetaNumeric*, unsigned int )", __LINE__, 1, "heap_size");
	}

	data = (double**)malloc(heap_size * sizeof(double*));
	if (data == NULL) {
		throw MetaException("MetaRatio", "MetaRatio( MetaNumeric*, MetaNumeric*, unsigned int )", __LINE__, 2, heap_size * sizeof(double*));
	}

	skewness = (double*)malloc(MetaRatio::SLICES_CNT * sizeof(double));
	if (skewness == NULL) {
		throw MetaException("MetaRatio", "MetaRatio( MetaNumeric*, MetaNumeric*, unsigned int )", __LINE__, 2, MetaRatio::SLICES_CNT * sizeof(double));
	}

	kurtosis = (double*)malloc(MetaRatio::SLICES_CNT * sizeof(double));
	if (kurtosis == NULL) {
		throw MetaException("MetaRatio", "MetaRatio( MetaNumeric*, MetaNumeric*, unsigned int )", __LINE__, 2, MetaRatio::SLICES_CNT * sizeof(double));
	}
}

MetaRatio::~MetaRatio() {
	if (data != NULL) {
		for (int i = 0; i < n; i++) {
			free(data[i]);
			data[i] = NULL;
		}
		free(data);
		data = NULL;
	}

	new_data = NULL;
	new_value = NULL;

	free(skewness);
	free(kurtosis);
	skewness = NULL;
	kurtosis = NULL;
}

MetaNumeric* MetaRatio::get_source_a() {
	return source_a;
}

MetaNumeric* MetaRatio::get_source_b() {
	return source_b;
}

double* MetaRatio::get_skewness() {
	return skewness;
}

double* MetaRatio::get_kurtosis() {
	return kurtosis;
}

void MetaRatio::put(char* value) throw (MetaException) {
	if (!source_a->is_numeric() || !source_b->is_numeric()) {
		return;
	}

	if (source_a->is_na() || source_b->is_na()) {
		return;
	}

	double ratio = source_a->get_value() / source_b->get_value();
	double p_wald = Rf_pchisq(ratio * ratio, 1.0, 0, 0);

	new_value = (double*)malloc(2 * sizeof(double));
	if (new_value == NULL) {
		throw MetaException("MetaRatio", "put( char* )", __LINE__, 2, (2 * sizeof(double)));
	}

	new_value[0] = p_wald;
	new_value[1] = source_a->get_value();

	n += 1;
	if (n > current_heap_size) {
		current_heap_size += Meta::HEAP_INCREMENT;

		new_data = (double**)realloc(data, current_heap_size * sizeof(double*));
		if (new_data == NULL) {
			for (int i = 0; i < n; i++) {
				free(data[i]);
			}
			free(data);
			free(new_value);
			data = NULL;
			throw MetaException("MetaRatio", "put( char* )", __LINE__, 3, current_heap_size * sizeof(double*));
		}

		data = new_data;
	}

	data[n - 1] = new_value;
}

void MetaRatio::finalize() throw (MetaException) {
	if ((data != NULL) && (n > 0)) {
		double* values = NULL;
		int slice = 0;
		double mean = 0.0;
		double sd = 0.0;

		qsort(data, n, sizeof(double*), auxiliary::qsort_dblcmp_array);

		values = (double*)malloc(n * sizeof(double));
		if (values == NULL) {
			throw MetaException("MetaRatio", "finalize()", __LINE__, 2, n * sizeof(double));
		}

		for (int i = n - 1; i >= 0; i--) {
			values[n - 1 - i] = data[i][1];
			free(data[i]);
			data[i] = NULL;
		}

		for (unsigned int i = 0; i < MetaRatio::SLICES_CNT; i++) {
			slice = (int)(n * MetaRatio::slices[i]);

			mean = auxiliary::stats_mean(values, slice);
			sd = auxiliary::stats_sd(values, slice, mean);
			skewness[i] = auxiliary::stats_skewness(values, slice, mean, sd);
			kurtosis[i] = auxiliary::stats_kurtosis(values, slice, mean, sd);
		}

		free(values);
		values = NULL;

		free(data);
		data = NULL;
	} else {
		for (unsigned int i = 0; i < MetaRatio::SLICES_CNT; i++) {
			skewness[i] = numeric_limits<double>::quiet_NaN();
			kurtosis[i] = numeric_limits<double>::quiet_NaN();
		}
	}
}

bool MetaRatio::is_na() {
	return false;
}

void MetaRatio::print(ostream& stream) {

}

void MetaRatio::print_html(ostream& stream, char path_separator) {

}

double MetaRatio::get_memory_usage() {
	unsigned long int memory = 0;

	memory += current_heap_size * sizeof(double*);
	memory += n * sizeof(double) * 2;
	memory += MetaRatio::SLICES_CNT * sizeof(double) * 2;

	return memory / 1048576.0;
}
