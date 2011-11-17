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

/*
 * Author: Daniel Taliun
 * Date: 03-12-2009
 *
 * Description:
 * Class Meta represents a single data column in an input file.
 * It holds column's real name (as it appears in the file) and common name (as it is usually referenced).
 * For the internal use a Meta entity might have an identification number and description.
 * This class provides an interface to process data values from the file with
 * respect to the nature of data and type of quality check.
 *
 */

#ifndef META_H_
#define META_H_

#include <cstdlib>
#include <cstring>
#include <ostream>
#include <map>
#include "MetaException.h"
#include "../plots/PlotException.h"

using namespace std;

class Meta {
public:
	static const unsigned int HEAP_SIZE;
	static const unsigned int HEAP_INCREMENT;

protected:
	int id;
	char* common_name;
	char* actual_name;
	char* full_name;
	char* description;
	char* na_marker;

public:
	Meta();
	virtual ~Meta();

	virtual void set_id(int id);
	virtual void set_common_name(const char* common_name) throw (MetaException);
	virtual void set_actual_name(const char* actual_name) throw (MetaException);
	virtual void set_full_name(const char* full_name) throw (MetaException);
	virtual void set_description(const char* description) throw (MetaException);
	virtual void set_na_marker(const char* na_marker) throw (MetaException);

	virtual int get_id();
	virtual const char* get_common_name();
	virtual const char* get_actual_name();
	virtual const char* get_full_name();
	virtual const char* get_description();
	virtual const char* get_na_marker();
	virtual double get_memory_usage();

	virtual void put(char* value) throw (MetaException) = 0;
	virtual void finalize() throw (MetaException) = 0;
	virtual bool is_na() = 0;
	virtual void print(ostream& stream) = 0;
	virtual void print_html(ostream& stream, char path_separator) = 0;

};

#endif
