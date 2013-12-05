/*
 * Copyright � 2012 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
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

#ifndef SELECTOR_H_
#define SELECTOR_H_

#include <map>
#include <vector>
#include <algorithm>

#include "SelectorException.h"
#include "../../gwafile/include/GwaFile.h"
#include "../../writer/include/WriterFactory.h"

#include <R.h>

using namespace std;

class Selector {
private:
	GwaFile* gwafile;

	Reader* reader;

	Reader* ld_reader;
	const char* ld_file_path;

//	char* header_backup;

	int total_columns;
	int marker_column_pos;
	int chr_column_pos;
	int pvalue_column_pos;

	int ld_total_columns;
	int ld_marker1_column_pos;
	int ld_marker2_column_pos;
	int ld_value_column_pos;

	vector<char*> all_marker_names;
	vector<char*>::iterator all_marker_names_it;

	map<char*, map<const char*, double, bool(*)(const char*, const char*)>*, bool(*)(const char*, const char*)> markers_by_chr;
	map<char*, map<const char*, double, bool(*)(const char*, const char*)>*, bool(*)(const char*, const char*)>::iterator markers_by_chr_it;
	map<const char*, double, bool(*)(const char*, const char*)>* markers;
	map<const char*, double, bool(*)(const char*, const char*)>::iterator markers_it;

	map<const char*, vector<const char*>*> markers_ld;
	map<const char*, vector<const char*>*>::iterator markers_ld_it;

	void open_ld_file(const char* file_path) throw (SelectorException);
	void close_ld_file() throw (SelectorException);
	void process_ld_header() throw (SelectorException);
	void process_ld_data() throw (SelectorException);
	void index_ld(const char* file_path) throw (SelectorException);
	void drop_correlated_markers() throw (SelectorException);
	void write_remaining_markers() throw (SelectorException);

public:
	static const double EPSILON;

	Selector();
	virtual ~Selector();

	void open_gwafile(GwaFile* gwafile) throw (SelectorException);
	void close_gwafile() throw (SelectorException);

	void process_header() throw (SelectorException);
	void process_data() throw (SelectorException);
	void independize() throw (SelectorException);
};

#endif
