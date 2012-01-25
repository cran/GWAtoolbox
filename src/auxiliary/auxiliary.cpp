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

#include "include/auxiliary.h"

char* auxiliary::strtok(char** start, char separator) {
	if ((*start != NULL) && (**start != '\0')) {
		char* token = *start;

		do {
			if (**start == separator) {
				**start = '\0';
				(*start)++;
				return token;
			}
			(*start)++;
		} while (**start != '\0');

		return token;
	}

	return NULL;
}

int auxiliary::strcmp_ignore_case(const char* first, const char* second) {
	int i = 0;

	while ((first[i] != '\0') && (second[i] != '\0')) {
		if (tolower(first[i]) < tolower(second[i])) {
			return -1;
		}
		else if (tolower(first[i]) > tolower(second[i])) {
			return 1;
		}
		i++;
	}

	if (first[i] == '\0') {
		if (second[i] == '\0') {
			return 0;
		}
		return -1;
	}
	return 1;
}

char* auxiliary::transform_file_name(char** new_file_name, const char* prefix, const char* old_file_name, const char* extension, bool preserve_old_extension) {
	int old_extension_start = -1;

	if ((new_file_name == NULL) || (old_file_name == NULL)) {
		return NULL;
	}

	if (!preserve_old_extension) {
		for (int i = strlen(old_file_name) - 1; i >= 0; i--) {
			if (old_file_name[i] == '.') {
				old_extension_start = i;
				break;
			}
		}

		if (old_extension_start < 0) {
			preserve_old_extension = true;
		}
	}

	*new_file_name = (char*)malloc(((prefix != NULL ? strlen(prefix) : 0) +
			(preserve_old_extension ? strlen(old_file_name) : old_extension_start) +
			(extension != NULL ? strlen(extension) : 0) + 1) * sizeof(char));

	if (*new_file_name == NULL) {
		return NULL;
	}

	(*new_file_name)[0] = '\0';

	if (prefix != NULL) {
		strcat(*new_file_name, prefix);
	}

	if (preserve_old_extension) {
		strcat(*new_file_name, old_file_name);
	} else {
		strncat(*new_file_name, old_file_name, old_extension_start);
	}

	if (extension != NULL) {
		strcat(*new_file_name, extension);
	}

	return *new_file_name;
}

double auxiliary::stats_mean(double* data, unsigned int size) {
	double sum = 0.0;

	for (unsigned int i = 0; i < size; i++) {
		sum += data[i];
	}

	return sum / size;
}

double auxiliary::stats_sd(double* data, unsigned int size, double mean) {
	double variance = 0.0;

	for (unsigned int i = 0; i < size; i++) {
		variance += pow(data[i] - mean, 2.0);
	}

	variance = variance / (size - 1);

	return sqrt(variance);
}

double auxiliary::stats_skewness(double* data, unsigned int size, double mean, double sd) {
	double skewness = 0.0;

	for (unsigned int i = 0; i < size; i++) {
		skewness += pow((data[i] - mean) / sd, 3.0);
	}

	return skewness / size;
}

double auxiliary::stats_kurtosis(double* data, unsigned int size, double mean, double sd) {
	double kurtosis = 0.0;

	for (unsigned int i = 0; i < size; i++) {
		kurtosis += pow((data[i] - mean) / sd, 4.0);
	}

	return kurtosis / size - 3.0;
}

double auxiliary::stats_median_from_sorted_data(double* data, unsigned int size) {
	if (size % 2 == 0) {
		return (data[size / 2 - 1] + data[size / 2]) / 2.0;
	} else {
		return data[(size - 1) / 2];
	}
}

double auxiliary::stats_quantile_from_sorted_data(double* data, unsigned int size, double fraction) {
	unsigned int i = (unsigned int)floor((size - 1) * fraction);
	double delta = (size - 1) * fraction - i;

	if (delta == 0) {
		return data[i];
	} else if (delta == 1) {
		return data[i + 1];
	}

	return (1 - delta) * data[i] + delta * data[i + 1];
}
