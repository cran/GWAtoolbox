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
 * Date: 14-12-2009
 *
 * Description:
 * Class File contains all methods, that are neccesary to work with input files.
 * It contains all the information about a file and its data.
 *
 */

#ifndef FILE_H_
#define FILE_H_

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <ctime>
#include <cctype>
#include <string.h>

#include "FileException.h"
#include "../metas/Meta.h"
#include "../metas/MetaNumeric.h"
#include "../metas/MetaFiltered.h"
#include "../metas/MetaRatio.h"
#include "../metas/MetaCrossTable.h"
#include "../plots/Plottable.h"

#define DEFAULT_FILE_SEPARATOR	'\\'

using namespace std;

class File {
private:
	char* file_name;
	char* file_path;
	char* full_path;
	char* abbreviation;
	char* output_prefix;
	char* output_name;
	char file_separator;
	bool case_sensitive;

	map<const char* const, char*, bool(*)(const char*, const char*)> columns;
	map<const char* const, char*, bool(*)(const char*, const char*)> properties;
	map<const char* const, vector<double>*, bool(*)(const char*, const char*)> thresholds;
	map<const char* const, char*, bool(*)(const char*, const char*)>::iterator map_char_it;
	map<const char* const, vector<double>*, bool(*)(const char*, const char*)>::iterator map_vect_it;
	vector<double>::iterator vector_it;

	vector<char*> missed_columns;
	vector<char*> not_required_columns;

	char header_separator;
	char data_separator;
	char* missing_value;
	int estimated_lines_count;

	double lambda;

	vector<Meta*> metas;
	vector<MetaNumeric*> numeric_metas;
	vector<MetaFiltered*> filtered_metas;
	vector<MetaRatio*> ratio_metas;
	vector<MetaCrossTable*> cross_table_metas;
	vector<Plottable*> plots;
	vector< pair<pair<Meta*, Meta*>, int> > dependencies;

	static bool compare(const char* first, const char* second);
	void add(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key, double value) throw (FileException);
	void add(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key, const char *value) throw (FileException);
	vector<double>* get(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException);
	const char* get(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException);
	bool remove(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException);
	bool remove(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException);
	void copy(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_f, map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_s) throw (FileException);
	void copy(map<const char* const, char*, bool(*)(const char*, const char*)>& map_f, map<const char* const, char*, bool(*)(const char*, const char*)>& map_s) throw (FileException);
	int sstrcmp(const char* first, const char* second);
	char* get_file_name(const char* path, char file_sep) throw (FileException);
	char* get_file_path(const char* path, char file_sep) throw (FileException);

public:
	File();
	File(const char* path, char file_sep = DEFAULT_FILE_SEPARATOR) throw (FileException);
	File(const char* path, File& file, char file_sep = DEFAULT_FILE_SEPARATOR) throw (FileException);
	virtual ~File();

	void set_full_path(const char* path, char file_sep = DEFAULT_FILE_SEPARATOR) throw (FileException);
	void set_abbreviation(const char* abbreviation) throw (FileException);
	void set_abbreviation(const char* abbreviation, unsigned int length) throw (FileException);

	const char* get_file_name();
	const char* get_file_path();
	const char* get_file_extension();
	const char* get_full_path();
	const char* get_abbreviation();
	char* transform_file_abbreviation(const char* prefix, const char* extension) throw (FileException);
	char* transform_output_name(const char* prefix, const char* extension) throw (FileException);
	char* transform_file_name(const char* prefix, const char* extension, bool preserve_old_extension = true) throw (FileException);

	void add_column(const char* common_name, const char* actual_name) throw (FileException);
	const char* get_column(const char* common_name) throw (FileException);
	const char* get_common_column(const char* actual_name);
	bool remove_column(const char* common_name) throw (FileException);

	void add_property(const char* name, const char* value) throw (FileException);
	const char* get_property(const char* value) throw (FileException);
	bool remove_property(const char* name) throw (FileException);

	void add_threshold(const char* name, double value) throw (FileException);
	vector<double>* get_threshold(const char* name) throw (FileException);
	bool remove_threshold(const char* name) throw (FileException);

	int get_columns_number();
	int get_properties_number();
	int get_thresholds_number();

	vector<char*>* get_missed_columns();
	vector<char*>* get_not_required_columns();

	void set_header_separator(char separator);
	char get_header_separator();
	void set_data_separator(char separator);
	char get_data_separator();

	void set_output_prefix(const char* prefix) throw (FileException);
	void set_output_name(const char* file_name) throw (FileException);
	const char* get_output_prefix();
	const char* get_output_name();

	void set_case_sensitive(bool sensitive);
	bool is_case_sensitive();

	void set_estimated_lines_count(int estimated_line_count);
	int get_estimated_lines_count();

	void set_lambda(double lambda);
	double get_lambda();

	vector<Meta*>* get_metas();
	vector<MetaNumeric*>* get_numeric_metas();
	vector<MetaFiltered*>* get_filtered_metas();
	vector<MetaRatio*>* get_ratio_metas();
	vector<MetaCrossTable*>* get_cross_table_metas();
	vector<Plottable*>* get_plots();
	vector< pair<pair<Meta*, Meta*>, int> >* get_dependencies();

	int get_memory_usage();
};

#endif
