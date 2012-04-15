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

#include "../../include/plots/Boxplot.h"

const int Boxplot::TUKEY_SIZE = 5;
const int Boxplot::NOTCHES_SIZE = 2;

Boxplot::Boxplot():
	tukey(NULL), observations(NULL), notches(NULL), outliers(NULL),
	groups(NULL), names(NULL), widths(NULL), outliers_cnt(0), groups_cnt(0), common_name(NULL), title(NULL) {

}

Boxplot::~Boxplot() {
	free(tukey);
	free(observations);
	free(notches);
	free(outliers);
	free(groups);
	free(widths);
	free(common_name);
	free(title);
	for (int i = 0; i < groups_cnt; i++) {
		free(names[i]);
		names[i] = NULL;
	}
	free(names);

	tukey = NULL;
	observations = NULL;
	notches = NULL;
	outliers = NULL;
	groups = NULL;
	widths = NULL;
	common_name = NULL;
	title = NULL;
	names = NULL;
}

Boxplot* Boxplot::create(const char* name, double* sorted_data, int data_size, double median) throw (PlotException) {
	Boxplot* boxplot = NULL;
	double lower_q_position = 0.0, upper_q_position = 0.0;
	double integer = 0.0, fraction = 0.0;
	double iqr = 0.0;
	double lower_outliers = 0.0, upper_outliers = 0.0;
	int lower_outliers_cnt = 0, upper_outliers_cnt = 0;

	if (name == NULL) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 1, "name");
	}

	if (sorted_data == NULL) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 0, "sorted_data");
	}

	if (data_size <= 0) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 1, "data_size");
	}

	if (isnan(median) || isinf(median)) {
		return NULL;
	}

	/* Create a boxplot */
	boxplot = new Boxplot();

	if ((boxplot->tukey = (double*)malloc(TUKEY_SIZE * sizeof(double))) == NULL) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 2, TUKEY_SIZE * sizeof(double));
	}

	if ((boxplot->observations = (int*)malloc(sizeof(int))) == NULL) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 2, sizeof(int));
	}

	if ((boxplot->names = (char**)malloc(sizeof(char*))) == NULL) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 2, sizeof(char*));
	}

	if ((boxplot->names[0] = (char*)malloc((strlen(name) + 1) * sizeof(char))) == NULL) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 2, (strlen(name) + 1) * sizeof(char));
	}

	if ((boxplot->widths = (double*)malloc(sizeof(double))) == NULL) {
		throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 2, sizeof(double));
	}

	strcpy(boxplot->names[0], name);
	boxplot->tukey[2] = median;
	boxplot->widths[0] = numeric_limits<double>::quiet_NaN();
	boxplot->groups_cnt = 1;

	/* Calculate and set hinges */
	if (data_size % 2 == 0) {
		lower_q_position = (double)(data_size + 2) / 4;
		upper_q_position = (double)(3 * data_size + 2) / 4;
	} else {
		lower_q_position = (double)(data_size + 3) / 4;
		upper_q_position = (double)(3 * data_size + 1) / 4;
	}

	fraction = modf(lower_q_position, &integer);

	if ((int)integer == data_size) {
		boxplot->tukey[1] = sorted_data[(int)integer - 1];
	} else {
		boxplot->tukey[1]  = (1 - fraction) * sorted_data[(int)integer - 1] + fraction * sorted_data[(int)integer];
		if (isnan(boxplot->tukey[1])) {
			boxplot->tukey[1] = sorted_data[(int)integer - 1];
		}
	}

	fraction = modf(upper_q_position, &integer);

	if ((int)integer == data_size) {
		boxplot->tukey[3] = sorted_data[(int)integer - 1];
	} else {
		boxplot->tukey[3] = (1 - fraction) * sorted_data[(int)integer - 1] + fraction * sorted_data[(int)integer];
		if (isnan(boxplot->tukey[3])) {
			boxplot->tukey[3] = sorted_data[(int)integer - 1];
		}
	}

	/* Calculate IQR (inter-quartile range) */
	iqr = boxplot->tukey[3] - boxplot->tukey[1];

	/* Deterirmine outliers' boundaries */
	lower_outliers = boxplot->tukey[1] - 1.5 * iqr;
	upper_outliers = boxplot->tukey[3] + 1.5 * iqr;

	/* Determine lower outliers' count */
	while ((lower_outliers_cnt < data_size) && (sorted_data[lower_outliers_cnt] < lower_outliers)) {
		lower_outliers_cnt += 1;
	}

	/* Set lower whisker */
	if (lower_outliers_cnt < data_size) {
		boxplot->tukey[0] = sorted_data[lower_outliers_cnt];
	} else {
		boxplot->tukey[0] = sorted_data[data_size - 1];
	}

	/* Determine upper outliers' count */
	upper_outliers_cnt = data_size - 1;

	while ((upper_outliers_cnt >= 0) && (sorted_data[upper_outliers_cnt] > upper_outliers)) {
		upper_outliers_cnt -= 1;
	}

	/* Set upper whisker */
	if (upper_outliers_cnt >= 0) {
		boxplot->tukey[4] = sorted_data[upper_outliers_cnt];
	} else {
		boxplot->tukey[4] = sorted_data[0];
	}

	upper_outliers_cnt = data_size - upper_outliers_cnt - 1;

	/* Set total outliers' count */
	boxplot->outliers_cnt = lower_outliers_cnt + upper_outliers_cnt;

	/* Set observations' number */
	boxplot->observations[0] = data_size;

	/* If there exist outliers, then copy them */
	if (boxplot->outliers_cnt > 0) {
		int new_outliers_cnt = 0;
		double previous_value = -numeric_limits<double>::infinity();
		double delta = (sorted_data[data_size - 1] - sorted_data[0]) / 1000;
		int i = 0, j = 0;

		/* BUG FIX: handle infinities */
		while ((i < data_size) && (isinf(sorted_data[i]))) {
			i += 1;
		}

		j = data_size - 1;
		while ((j >= 0) && (isinf(sorted_data[j]))) {
			j -= 1;
		}

		delta = (sorted_data[j] - sorted_data[i]) / 1000;
		/* END BUG FIX. */

		i = 0;
		j = 0;

		/* Determine non-overlapping outliers count */
		while (i < lower_outliers_cnt) {
			if (sorted_data[i] >= previous_value + delta) {
				new_outliers_cnt += 1;
				previous_value = sorted_data[i];
			}
			i += 1;
		}

		i = 0;
		while (i < upper_outliers_cnt) {
			if (sorted_data[data_size - upper_outliers_cnt + i] >= previous_value + delta) {
				new_outliers_cnt += 1;
				previous_value = sorted_data[data_size - upper_outliers_cnt + i];
			}
			i += 1;
		}

		/* Copy non-overlapping outliers. Intuitively the check for > 0 is not necessary, however we keep it. */
		if (new_outliers_cnt > 0) {
			if ((boxplot->outliers = (double*)malloc(new_outliers_cnt * sizeof(double))) == NULL) {
				throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 2, new_outliers_cnt * sizeof(double));
			}

			previous_value = -numeric_limits<double>::infinity();
			i = 0;
			while (i < lower_outliers_cnt) {
				if (sorted_data[i] >= previous_value + delta) {
					boxplot->outliers[j] = sorted_data[i];
					j += 1;
					previous_value = sorted_data[i];
				}
				i += 1;
			}

			i = 0;
			while (i < upper_outliers_cnt) {
				if (sorted_data[data_size - upper_outliers_cnt + i] >= previous_value + delta) {
					boxplot->outliers[j] = sorted_data[data_size - upper_outliers_cnt + i];
					j += 1;
					previous_value = sorted_data[data_size - upper_outliers_cnt + i];
				}
				i += 1;
			}

			boxplot->outliers_cnt = new_outliers_cnt;

			/* Create and fill the groups array (Only for compatibility with R) */
			if ((boxplot->groups = (int*)malloc(boxplot->outliers_cnt * sizeof(int))) == NULL) {
				throw PlotException("Boxplot", "Boxplot* create( const char*, double*, int, double )", __LINE__, 2, boxplot->outliers_cnt * sizeof(int));
			}

			for (int i = 0; i < boxplot->outliers_cnt; i++) {
				boxplot->groups[i] = 1;
			}
		}
	}

	return boxplot;
}

Boxplot* Boxplot::merge(Boxplot* to, Boxplot* from) throw (PlotException) {
	Boxplot* boxplot = to;

	if ((from != NULL) && (from->get_groups_cnt() == 1)) {
		void* new_memory = NULL;
		int new_group = 0;
		int i = 0, j = 0;

		if (boxplot == NULL) {
			boxplot = new Boxplot();
			boxplot->set_title(from->get_title());
		}

		new_group = boxplot->groups_cnt + 1;

		new_memory = realloc(boxplot->tukey, new_group * TUKEY_SIZE * sizeof(double));
		if (new_memory == NULL) {
			throw PlotException("Boxplot", "Boxplot* merge( Boxplot*, Boxplot* )", __LINE__, 3, new_group * TUKEY_SIZE * sizeof(double));
		}
		boxplot->tukey = (double*)new_memory;

		i = 0;
		j = boxplot->groups_cnt * TUKEY_SIZE;
		while (i < TUKEY_SIZE) {
			boxplot->tukey[j++] = from->tukey[i++];
		}

		new_memory = realloc(boxplot->observations, new_group * sizeof(int));
		if (new_memory == NULL) {
			throw PlotException("Boxplot", "Boxplot* merge( Boxplot*, Boxplot* )", __LINE__, 3, new_group * sizeof(int));
		}
		boxplot->observations = (int*)new_memory;
		boxplot->observations[boxplot->groups_cnt] = from->observations[0];

		new_memory = realloc(boxplot->outliers, (boxplot->outliers_cnt + from->outliers_cnt) * sizeof(double));
		if (new_memory == NULL) {
			throw PlotException("Boxplot", "Boxplot* merge( Boxplot*, Boxplot* )", __LINE__, 3, (boxplot->outliers_cnt + from->outliers_cnt) * sizeof(double));
		}
		boxplot->outliers = (double*)new_memory;

		new_memory = realloc(boxplot->groups, (boxplot->outliers_cnt + from->outliers_cnt) * sizeof(int));
		if (new_memory == NULL) {
			throw PlotException("Boxplot", "Boxplot* merge( Boxplot*, Boxplot* )", __LINE__, 3, (boxplot->outliers_cnt + from->outliers_cnt) * sizeof(int));
		}
		boxplot->groups = (int*)new_memory;

		i = 0;
		j = boxplot->outliers_cnt;
		while (i < from->outliers_cnt) {
			boxplot->outliers[j] = from->outliers[i++];
			boxplot->groups[j++] = new_group;
		}

		new_memory = realloc(boxplot->widths, new_group * sizeof(double));
		if (new_memory == NULL) {
			throw PlotException("Boxplot", "Boxplot* merge( Boxplot*, Boxplot* )", __LINE__, 3, new_group * sizeof(double));
		}
		boxplot->widths = (double*)new_memory;
		boxplot->widths[boxplot->groups_cnt] = from->widths[0];

		new_memory = realloc(boxplot->names, new_group * sizeof(char*));
		if (new_memory == NULL) {
			throw PlotException("Boxplot", "Boxplot* merge( Boxplot*, Boxplot* )", __LINE__, 3, new_group * sizeof(char*));
		}
		boxplot->names = (char**)new_memory;

		boxplot->names[boxplot->groups_cnt] = (char*)malloc((strlen(from->names[0]) + 1) * sizeof(char));
		if (boxplot->names[boxplot->groups_cnt] == NULL) {
			throw PlotException("Boxplot", "Boxplot* merge( Boxplot*, Boxplot* )", __LINE__, 2, (strlen(from->names[0]) + 1) * sizeof(char));
		}
		strcpy(boxplot->names[boxplot->groups_cnt], from->names[0]);

		boxplot->outliers_cnt = boxplot->outliers_cnt + from->outliers_cnt;
		boxplot->groups_cnt = new_group;
	}

	return boxplot;
}

void Boxplot::set_name(int group, const char* name) throw (PlotException) {
	if ((group <= 0) || (group > groups_cnt)) {
		throw PlotException("Boxplot", "set_name( int, const char* )", __LINE__, 1, "group");
	}

	if (name == NULL) {
		throw PlotException("Boxplot", "set_name( int, const char* )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Boxplot", "set_name( int, const char* )", __LINE__, 1, "name");
	}

	group -= 1;
	free(names[group]);
	names[group] = NULL;
	names[group] = (char*)malloc((strlen(name) + 1) * sizeof(char));
	if (names[group] == NULL) {
		throw PlotException("Boxplot", "set_name( int, const char* )", __LINE__, 2, (strlen(name) + 1) * sizeof(char));
	}
	strcpy(names[group], name);
}

void Boxplot::set_name(int group, const char* name, unsigned int length) throw (PlotException) {
	if ((group <= 0) || (group > groups_cnt)) {
		throw PlotException("Boxplot", "set_name( int, const char* , unsigned int )", __LINE__, 1, "group");
	}

	if (name == NULL) {
		throw PlotException("Boxplot", "set_name( int, const char* , unsigned int )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Boxplot", "set_name( int, const char* , unsigned int )", __LINE__, 1, "name");
	}

	if ((length <= 0) || (length > strlen(name))) {
		length = strlen(name);
	}

	group -= 1;
	free(names[group]);
	names[group] = NULL;
	names[group] = (char*)malloc((length + 1) * sizeof(char));
	if (names[group] == NULL) {
		throw PlotException("Boxplot", "set_name( int, const char* , unsigned int )", __LINE__, 2, (length + 1) * sizeof(char));
	}
	strncpy(names[group], name, length);
	names[group][length] = '\0';
}

void Boxplot::set_common_name(const char* name) throw (PlotException) {
	if (name == NULL) {
		throw PlotException("Boxplot", "set_common_name( const char* )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Boxplot", "set_common_name( const char* )", __LINE__, 1, "name");
	}

	free(common_name);
	common_name = NULL;
	common_name = (char*)malloc((strlen(name) + 1) * sizeof(char));
	if (common_name == NULL) {
		throw PlotException("Boxplot", "set_common_name( const char* )", __LINE__, 2, (strlen(name) + 1) * sizeof(char));
	}
	strcpy(common_name, name);
}

void Boxplot::set_width(int group, double width) throw (PlotException) {
	if ((group <= 0) || (group > groups_cnt)) {
		throw PlotException("Boxplot", "set_width( int, double )", __LINE__, 1, "group");
	}

	group -= 1;
	widths[group] = width;
}

void Boxplot::set_quantiles(double q0, double q25, double q5, double q75, double q1) {
	quantiles[0] = q0;
	quantiles[1] = q25;
	quantiles[2] = q5;
	quantiles[3] = q75;
	quantiles[4] = q1;
}

const double* Boxplot::get_tukey() {
	return tukey;
}

const int* Boxplot::get_observations() {
	return observations;
}

const double* Boxplot::get_notches() {
	return notches;
}

const double* Boxplot::get_outliers() {
	return outliers;
}

const int* Boxplot::get_groups() {
	return groups;
}

const char* const* Boxplot::get_names() {
	return names;
}

const double* Boxplot::get_widths() {
	return widths;
}

double Boxplot::get_quantile(int q) {
	double value = numeric_limits<double>::quiet_NaN();

	switch (q) {
		case 0:
			value = quantiles[0];
			break;
		case 25:
			value = quantiles[1];
			break;
		case 50:
			value = quantiles[2];
			break;
		case 75:
			value = quantiles[3];
			break;
		case 100:
			value = quantiles[4];
			break;
		default:
			break;
	}

	return value;
}

int Boxplot::get_outliers_cnt() {
	return outliers_cnt;
}

int Boxplot::get_groups_cnt() {
	return groups_cnt;
}

const char* Boxplot::get_common_name() {
	return common_name;
}

void Boxplot::set_title(const char* title) throw (PlotException) {
	if (title == NULL) {
		throw PlotException("Boxplot", "set_title( const char* )", __LINE__, 0, "title");
	}

	if (strlen(title) <= 0) {
		throw PlotException("Boxplot", "set_title( const char* )", __LINE__, 1, "title");
	}

	free(this->title);
	this->title = NULL;
	this->title = (char*)malloc((strlen(title) + 1) * sizeof(char));
	if (this->title == NULL) {
		throw PlotException("Boxplot", "set_title( const char* )", __LINE__, 2, (strlen(title) + 1) * sizeof(char));
	}

	strcpy(this->title, title);
}

const char* Boxplot::get_title() {
	return title;
}
