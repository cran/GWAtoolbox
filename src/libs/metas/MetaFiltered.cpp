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

#include "../../include/metas/MetaFiltered.h"

MetaFiltered::MetaFiltered(MetaNumeric* source, int heap_size) throw (MetaException) : MetaNumeric(heap_size), source(source)  {
	affiliate_begin = affiliates.begin();
	affiliate_end = affiliates.end();
	condition_begin = conditions.begin();
	condition_end = conditions.end();
}

MetaFiltered::~MetaFiltered() {
	affiliates.clear();

	for (condition_it = condition_begin; condition_it != condition_end; condition_it++) {
		delete condition_it->second;
	}
	conditions.clear();
}

void MetaFiltered::add_dependency(MetaNumeric& meta) {
	affiliates.push_back(&meta);
	affiliate_begin = affiliates.begin();
	affiliate_end = affiliates.end();
}

void MetaFiltered::add_dependency(MetaNumeric& meta, Evaluable& condition) {
	conditions.insert(pair<MetaNumeric*, Evaluable*>(&meta, &condition));
	condition_begin = conditions.begin();
	condition_end = conditions.end();
}

MetaNumeric* MetaFiltered::get_source() {
	return source;
}

void MetaFiltered::put(char* value) throw (MetaException) {
	if (numeric) {
		if (!source->is_numeric()) {
			numeric = false;
			this->value = numeric_limits<double>::quiet_NaN();
			free(data);
			data = NULL;
			return;
		}

		na_value = false;

		if (source->is_na()) {
			na += 1;
			na_value = true;
			this->value = numeric_limits<double>::quiet_NaN();
			return;
		}

		for (affiliate_it = affiliate_begin; affiliate_it != affiliate_end; affiliate_it++) {
			if (!(*affiliate_it)->is_numeric()) {
				numeric = false;
				this->value = numeric_limits<double>::quiet_NaN();
				free(data);
				data = NULL;
				return;
			}

			if ((*affiliate_it)->is_na()) {
				na_value = true;
				this->value = numeric_limits<double>::quiet_NaN();
				return;
			}
		}

		for (condition_it = condition_begin; condition_it != condition_end; condition_it++) {
			if (!condition_it->first->is_numeric()) {
				numeric = false;
				this->value = numeric_limits<double>::quiet_NaN();
				free(data);
				data = NULL;
				return;
			}

			if ((condition_it->first->is_na()) || (!condition_it->second->evaluate(condition_it->first->get_value()))) {
				na_value = true;
				this->value = numeric_limits<double>::quiet_NaN();
				return;
			}
		}

		n += 1;

		if (n > current_heap_size) {
			current_heap_size += Meta::HEAP_INCREMENT;

			new_data = (double*)realloc(data, current_heap_size * sizeof(double));
			if (new_data == NULL) {
				free(data);
				data = NULL;
				throw MetaException(3, "MetaFiltered", "put( ... )", "data", current_heap_size * sizeof(double));
			}
			data = new_data;
		}

		data[n - 1] = source->get_value();
		this->value = source->get_value();
	}
}

void MetaFiltered::finalize() throw (MetaException, PlotException) {
	if (source->get_n() <= 0) {
		numeric = false;
		free(data);
		data = NULL;
		return;
	}

	if (numeric) {
		if (n > 0) {
			mean = gsl_stats_mean(data, 1, n);
			sd = gsl_stats_sd_m(data, 1, n, mean);
			skew = gsl_stats_skew_m_sd(data, 1, n, mean, sd);
			kurtosis = gsl_stats_kurtosis_m_sd(data, 1, n, mean, sd);

			qsort(data, n, sizeof(double), MetaNumeric::compare);

			median = gsl_stats_median_from_sorted_data(data, 1, n);

			for (unsigned int j = 0; j < 9; j++) {
				quantiles[j][1] = gsl_stats_quantile_from_sorted_data(data, 1, n, quantiles[j][0]);
			}

			min = data[0];
			max = data[n - 1];

			if (create_qqplot) {
				double* chi_data = NULL;
				double lambda = 0;

				if ((chi_data = (double*)malloc(n * sizeof(double))) == NULL) {
					throw MetaException(0, "MetaFiltered", "finalize()", "chi_data", n * sizeof(double));
				}

				for (int i = 0; i < n; i++) {
					chi_data[i] = Rf_qchisq(data[i], 1, 0, 0);
				}

				qsort(chi_data, n, sizeof(double), MetaNumeric::compare);
				lambda = gsl_stats_median_from_sorted_data(chi_data, 1, n) / Rf_qchisq(0.5, 1, 0, 0);
				free(chi_data);
				chi_data = NULL;

				qqplot = Qqplot::create(get_description(), get_color(), data, lambda, n);
			}

			if (create_histogram) {
				histogram = Histogram::create(actual_name, data, n, 1000);
				if (histogram != NULL) {
					histogram->set_title(get_description());
				}
			}

			if (create_boxplot) {
				boxplot = Boxplot::create(actual_name, data, n, median);
				if (boxplot != NULL) {
					boxplot->set_title(get_description());
					boxplot->set_quantiles(quantiles[0][1], quantiles[3][1], quantiles[4][1], quantiles[5][1], quantiles[8][1]);
				}
			}

			/* TODO: remove this */
			free(data);
			data = NULL;
		}
		else if (create_qqplot) {
			qqplot = Qqplot::create(get_description(), get_color(), data, numeric_limits<double>::quiet_NaN(), 0);
		}
	}

	free(data);
	data = NULL;
}

void MetaFiltered::print(ostream& stream) {

}

void MetaFiltered::print_html(ostream& stream, const char* image_name) {

}

