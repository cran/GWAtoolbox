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

#include "../../include/metas/MetaImplausibleStrictAdjusted.h"

MetaImplausibleStrictAdjusted::MetaImplausibleStrictAdjusted(double less, double greater, double threshold, int heap_size) throw (MetaException):
	MetaImplausibleStrict(less, greater, heap_size), threshold(threshold) {

}

MetaImplausibleStrictAdjusted::~MetaImplausibleStrictAdjusted() {

}

void MetaImplausibleStrictAdjusted::put(char* value) throw (MetaException) {
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

		this->value = d_value > threshold ? 1.0 - d_value : d_value;

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
				throw MetaException(3, "MetaImplausibleStrictAdjusted", "put( ... )", "data", current_heap_size * sizeof(double));
			}
			data = new_data;
		}

		data[n - 1] = this->value;
	}
}

void MetaImplausibleStrictAdjusted::set_threshold(double threshold) {
	this->threshold = threshold;
}
