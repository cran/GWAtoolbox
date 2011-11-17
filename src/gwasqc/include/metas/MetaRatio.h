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

#ifndef METARATIO_H_
#define METARATIO_H_

#include "MetaNumeric.h"
#include <map>

#include <R.h>
#include <Rmath.h>

using namespace std;

class MetaRatio: public Meta {
private:
	MetaNumeric* source_a;
	MetaNumeric* source_b;

	int n;

	double** data;
	double** new_data;
	double* new_value;

	int current_heap_size;

	double* skewness;
	double* kurtosis;

public:
	static const unsigned int SLICES_CNT;
	static const double slices[];

	MetaRatio(MetaNumeric* source_a, MetaNumeric* source_b, unsigned int heap_size = Meta::HEAP_SIZE) throw (MetaException);
	virtual ~MetaRatio();

	MetaNumeric* get_source_a();
	MetaNumeric* get_source_b();

	double* get_skewness();
	double* get_kurtosis();

	void put(char* value) throw (MetaException);
	void finalize() throw (MetaException);
	bool is_na();
	void print(ostream& stream);
	void print_html(ostream& stream, char path_separator);
	double get_memory_usage();
};

#endif
