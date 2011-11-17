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

#ifndef METAFILTERED_H_
#define METAFILTERED_H_

#include <vector>
#include <map>

#include "MetaNumeric.h"
#include "../expressions/Evaluable.h"
#include "../../../bitarray/include/BitArray.h"

#include "R.h"
#include "Rmath.h"

class MetaFiltered: public MetaNumeric {
private:
	MetaNumeric* source;

	vector<MetaNumeric*> affiliates;
	vector<MetaNumeric*>::iterator affiliate_it;
	vector<MetaNumeric*>::iterator affiliate_begin;
	vector<MetaNumeric*>::iterator affiliate_end;

	map<MetaNumeric*, Evaluable*> conditions;
	map<MetaNumeric*, Evaluable*>::iterator condition_it;
	map<MetaNumeric*, Evaluable*>::iterator condition_begin;
	map<MetaNumeric*, Evaluable*>::iterator condition_end;

	BitArray* bitarray;

public:
	MetaFiltered(MetaNumeric* source, unsigned int heap_size = Meta::HEAP_SIZE) throw (MetaException);
	virtual ~MetaFiltered();

	void add_dependency(MetaNumeric& meta);
	void add_dependency(MetaNumeric& meta, Evaluable& condition);

	MetaNumeric* get_source();

	void put(char* value) throw (MetaException);
	void finalize() throw (MetaException);
	void print(ostream& stream);
	void print_html(ostream& stream, char path_separator);

	double get_memory_usage();
};

#endif
