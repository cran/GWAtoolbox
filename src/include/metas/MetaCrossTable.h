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

#ifndef METACROSSTABLE_H_
#define METACROSSTABLE_H_

#include "Meta.h"
#include "MetaNumeric.h"

#include <map>

class MetaCrossTable: public Meta {
private:
	MetaNumeric* x_meta;
	MetaNumeric* y_meta;

	map<double, map<double, int>* > table;

	map<double, int> x_na;
	map<double, int> y_na;
	int xy_na;

	map<double, int> x_total;
	map<double, int> y_total;
	int x_na_total;
	int y_na_total;
	int xy_total;

	map<double, map<double, int>* >::iterator column_it;
	map<double, int>::iterator row_it;
	map<double, int>* row;

	double new_x_value;
	double new_y_value;

public:
	MetaCrossTable(MetaNumeric* x_meta, MetaNumeric* y_meta);
	virtual ~MetaCrossTable();

	void put(char* value) throw (MetaException);
	void finalize() throw (MetaException);
	bool is_na();
	void print(ostream& stream);
	void print_html(ostream& stream, const char* image_name);
	int get_memory_usage();
};

#endif
