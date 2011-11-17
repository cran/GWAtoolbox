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

#include "../../include/plots/Qqplot.h"

Qqplot::Qqplot():
	x(NULL), y(NULL), size(NULL), points(NULL), names(NULL),
	colors(NULL), lambdas(NULL), all_size(0), count(0) {

}

Qqplot::~Qqplot() {
	free(x);
	free(y);
	free(size);
	free(points);
	free(lambdas);
	for (int i = 0; i < count; i++) {
		free(names[i]);
		free(colors[i]);
	}
	free(names);
	free(colors);

	x = NULL;
	y = NULL;
	size = NULL;
	points = NULL;
	lambdas = NULL;
	names = NULL;
	colors = NULL;
}

Qqplot* Qqplot::create(const char* name, const char* color, double* sorted_data, double lambda, int data_size) throw (PlotException) {
	if (name == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 1, "name");
	}

	if (color == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 0, "color");
	}

	if (strlen(color) <= 0) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 1, "color");
	}

	if (data_size < 0) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 1, "data_size");
	}

	Qqplot* qqplot = new Qqplot();
	int new_data_size = 0;

	if (data_size > 0) {
		if (sorted_data == NULL) {
			throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 0, "sorted_data");
		}

		int i = 0, j = 0;

		double x_delta = (-log10(1 / (double)data_size)) / 500;
		double y_delta = (-log10(sorted_data[0]) + log10(sorted_data[data_size - 1])) / 500;

		double x_previous_value = numeric_limits<double>::infinity();
		double y_previous_value = numeric_limits<double>::infinity();

		double x_value = 0.0, y_value = 0.0;

		while (i < data_size) {
			x_value = -log10((i + 1) / (double)data_size);
			y_value = -log10(sorted_data[i]);

			if ((x_value < x_previous_value - x_delta) || (y_value < y_previous_value - y_delta)) {
				x_previous_value = x_value;
				y_previous_value = y_value;
				new_data_size += 1;
			}

			i += 1;
		}

		if ((qqplot->x = (double*)malloc(new_data_size * sizeof(double))) == NULL) {
			throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, new_data_size * sizeof(double));
		}

		if ((qqplot->y = (double*)malloc(new_data_size * sizeof(double))) == NULL) {
			throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, new_data_size * sizeof(double));
		}

		x_previous_value = numeric_limits<double>::infinity();
		y_previous_value = numeric_limits<double>::infinity();
		x_value = 0.0;
		y_value = 0.0;
		i = 0;

		while (i < data_size) {
			x_value = -log10((i + 1) / (double)data_size);
			y_value = -log10(sorted_data[i]);

			if ((x_value < x_previous_value - x_delta) || (y_value < y_previous_value - y_delta)) {
				qqplot->x[j] = x_value;
				qqplot->y[j] = y_value;
				x_previous_value = x_value;
				y_previous_value = y_value;
				j += 1;
			}

			i += 1;
		}
	}

	if ((qqplot->size = (int*)malloc(sizeof(int))) == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, sizeof(int));
	}

	if ((qqplot->points = (int*)malloc(sizeof(int))) == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, sizeof(int));
	}

	if ((qqplot->names = (char**)malloc(sizeof(char*))) == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, sizeof(char*));
	}

	if ((qqplot->colors = (char**)malloc(sizeof(char*))) == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, sizeof(char*));
	}

	if ((qqplot->lambdas = (double*)malloc(sizeof(double))) == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, sizeof(double));
	}

	if ((qqplot->names[0] = (char*)malloc((strlen(name) + 1) * sizeof(char))) == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, (strlen(name) + 1) * sizeof(char));
	}
	strcpy(qqplot->names[0], name);

	if ((qqplot->colors[0] = (char*)malloc((strlen(color) + 1) * sizeof(char))) == NULL) {
		throw PlotException("Qqplot", "Qqplot* create( const char*, const char*, double*, double, int )", __LINE__, 2, (strlen(color) + 1) * sizeof(char));
	}
	strcpy(qqplot->colors[0], color);

	qqplot->size[0] = new_data_size;
	qqplot->points[0] = data_size;
	qqplot->lambdas[0] = lambda;
	qqplot->all_size = new_data_size;
	qqplot->count = 1;

	return qqplot;
}

Qqplot* Qqplot::merge(Qqplot* to, Qqplot* from) throw (PlotException) {
	Qqplot* qqplot = to;

	if ((from != NULL) && (from->get_count() == 1)) {
		void* new_memory = NULL;
		int new_all_size = 0;
		int new_count = 0;
		int i = 0, j = 0;

		if (qqplot == NULL) {
			qqplot = new Qqplot();
		}

		new_all_size = qqplot->all_size + from->size[0];
		new_count = qqplot->count + 1;

		if (from->size[0] > 0) {
			new_memory = realloc(qqplot->x, new_all_size * sizeof(double));
			if (new_memory == NULL) {
				throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 3, new_all_size * sizeof(double));
			}
			qqplot->x = (double*)new_memory;

			new_memory = realloc(qqplot->y, new_all_size * sizeof(double));
			if (new_memory == NULL) {
				throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 3, new_all_size * sizeof(double));
			}
			qqplot->y = (double*)new_memory;

			i = 0;
			j = qqplot->all_size;
			while (i < from->size[0]) {
				qqplot->x[j] =  from->x[i];
				qqplot->y[j] =  from->y[i];
				i += 1;
				j += 1;
			}
		}

		new_memory = realloc(qqplot->size, new_count * sizeof(int));
		if (new_memory == NULL) {
			throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 3, new_count * sizeof(int));
		}
		qqplot->size = (int*)new_memory;

		new_memory = realloc(qqplot->points, new_count * sizeof(int));
		if (new_memory == NULL) {
			throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 3, new_count * sizeof(int));
		}
		qqplot->points = (int*)new_memory;

		new_memory = realloc(qqplot->names, new_count * sizeof(char*));
		if (new_memory == NULL) {
			throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 3, new_count * sizeof(char*));
		}
		qqplot->names = (char**)new_memory;

		if ((qqplot->names[qqplot->count] = (char*)malloc((strlen(from->names[0]) + 1) * sizeof(char))) == NULL) {
			throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 2, (strlen(from->names[0]) + 1) * sizeof(char));
		}
		strcpy(qqplot->names[qqplot->count], from->names[0]);

		new_memory = realloc(qqplot->colors, new_count * sizeof(char*));
		if (new_memory == NULL) {
			throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 3, new_count * sizeof(char*));
		}
		qqplot->colors = (char**)new_memory;

		if ((qqplot->colors[qqplot->count] = (char*)malloc((strlen(from->colors[0]) + 1) * sizeof(char))) == NULL) {
			throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 2, (strlen(from->colors[0]) + 1) * sizeof(char));
		}
		strcpy(qqplot->colors[qqplot->count], from->colors[0]);

		new_memory = realloc(qqplot->lambdas, new_count * sizeof(double));
		if (new_memory == NULL) {
			throw PlotException("Qqplot", "Qqplot* merge( Qqplot*, Qqplot* )", __LINE__, 3, new_count * sizeof(double));
		}
		qqplot->lambdas = (double*)new_memory;

		qqplot->size[qqplot->count] = from->size[0];
		qqplot->points[qqplot->count] = from->points[0];
		qqplot->lambdas[qqplot->count] = from->lambdas[0];
		qqplot->all_size = new_all_size;
		qqplot->count = new_count;
	}

	return qqplot;
}

void Qqplot::set_lambda(int group, double lambda) throw (PlotException) {
	if ((group <= 0) || (group > count)) {
		throw PlotException("Qqplot", "set_lambda( int, double )", __LINE__, 1, "group");
	}

	group -= 1;
	lambdas[group] = lambda;
}

const double* Qqplot::get_x() {
	return x;
}

const double* Qqplot::get_y() {
	return y;
}

const int* Qqplot::get_size() {
	return size;
}

const int* Qqplot::get_points() {
	return points;
}

int Qqplot::get_count() {
	return count;
}

const char* const* Qqplot::get_names() {
	return names;
}

const char* const* Qqplot::get_colors() {
	return colors;
}

const double* Qqplot::get_lambdas() {
	return lambdas;
}
