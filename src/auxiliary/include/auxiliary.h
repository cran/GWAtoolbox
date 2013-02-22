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

#ifndef AUXILIARY_H_
#define AUXILIARY_H_

#include <iostream>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <limits>

using namespace std;

namespace auxiliary {

	char* strtok(char** start, char separator);

	int strcmp_ignore_case(const char* first, const char* second);

	int strcmp_ignore_case(const char* first, const char* second, int n);

	inline bool is_absolute_path(const char* path, char path_separator) {
		if (path == NULL) {
			return false;
		}

		if ((strlen(path) >= 1) && (path[0] == path_separator)) {
			return true;
		}

		if ((strlen(path) >= 3) && (isalpha(path[0])) && (path[1] == ':') && (path[2] == path_separator)) {
			return true;
		}

		return false;
	}

	inline bool is_numeric(const char* value) {
		if ((value == NULL) || (strlen(value) <= 0)) {
			return false;
		}

		char* end_ptr = NULL;

		strtod(value, &end_ptr);

		return (*end_ptr == '\0');
	}

	inline int qsort_strcmp(const void* first, const void* second) {
		return strcmp(*(char**)first, *(char**)second);
	}

	inline int qsort_dblcmp_array(const void* first, const void* second) {
		double d_first = **((double**)first);
		double d_second = **((double**)second);

		if (d_first < d_second) {
			return -1;
		}
		else if (d_first > d_second) {
			return 1;
		}

		return 0;
	}

	inline bool bool_strcmp(const char* first, const char* second) {
		return strcmp(first, second) < 0;
	}

	inline bool bool_strcmp_ignore_case(const char* first, const char* second) {
		return strcmp_ignore_case(first, second) < 0;
	}

	inline int dblcmp(const void* first, const void* second) {
		double d_first = *(double*)first;
		double d_second = *(double*)second;

		if (d_first < d_second) {
			return -1;
		}
		else if (d_first > d_second) {
			return 1;
		}

		return 0;
	}

	char* transform_file_name(char** new_file_name, const char* prefix, const char* file_name, const char* suffix, bool preserve_old_suffix);

	double stats_mean(double* data, unsigned int size);

	double stats_sd(double* data, unsigned int size, double mean);

	double stats_skewness(double* data, unsigned int size, double mean, double sd);

	double stats_kurtosis(double* data, unsigned int size, double mean, double sd);

	double stats_median_from_sorted_data(double* data, unsigned int size);

	double stats_quantile_from_sorted_data(double* data, unsigned int size, double fraction);

	inline int fcmp(double x, double y, double epsilon) {
		int max_exponent = 0;
		double delta = 0.0;
		double diff = 0.0;

		frexp(fabs(x) > fabs(y) ? x : y, &max_exponent);
		delta = ldexp(epsilon, max_exponent);

		diff = x - y;

		if (diff > delta) {
			return 1;
		} else if (diff < -delta) {
			return -1;
		} else {
			return 0;
		}
	}

	inline double to_double(const char* value) {
		if ((value != NULL) && (strlen(value) > 0)) {
			char* end_ptr = NULL;
			double d_value = strtod(value, &end_ptr);
			if (*end_ptr == '\0') {
				return d_value;
			}
		}

		return numeric_limits<double>::quiet_NaN();
	}

	inline void trim_end(char* string) {
		int j = strlen(string) - 1;

		while ((j >= 0) && ((string[j] == ' ') || (string[j] == '\t'))) {
			j--;
		}

		string[j + 1] = '\0';
	}

	inline void trim_start(char* string) {
		int i = 0;
		int j = strlen(string);

		while ((string[i] == ' ') || (string[i] == '\t')) {
			i++;
		}

		memmove(string, string + i, j - i);
		string[j - i] = '\0';
	}

	inline unsigned long int to_ulong_int(const char* value, unsigned long int* ulong_int) {
		if ((value != NULL) && (strlen(value) <= 0)) {
			return false;
		}

		char* end_ptr = NULL;

		*ulong_int = strtol(value, &end_ptr, 10);

		return (*end_ptr == '\0');
	}
}

#endif
