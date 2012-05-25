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

#ifndef DESCRIPTOR_H_
#define DESCRIPTOR_H_

#ifdef LINUX
#define DEFAULT_PATH_SEPARATOR	'/'
#else
#define DEFAULT_PATH_SEPARATOR	'\\'
#endif

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cctype>
#include <string.h>

#include "../../auxiliary/include/auxiliary.h"
#include "../../reader/include/TextReader.h"
#include "DescriptorException.h"

#include "R.h"
#include "Rinternals.h"

using namespace std;
using namespace auxiliary;

extern "C" {
	SEXP Robj2Descriptor(SEXP descriptor_Robj);
	SEXP Descriptor2Robj(SEXP descriptor);
}

class Descriptor {
private:
	char path_separator;
	char* name;
	char* path;
	char* full_path;
	char* abbreviation;

	map<const char* const, char*, bool(*)(const char*, const char*)> columns;
	map<const char* const, char*, bool(*)(const char*, const char*)> properties;
	map<const char* const, vector<double>*, bool(*)(const char*, const char*)> thresholds;
	map<const char* const, char*, bool(*)(const char*, const char*)> renamed_columns;
	vector<const char*> reordered_columns;

	map<const char* const, char*, bool(*)(const char*, const char*)>::iterator map_char_it;
	map<const char* const, vector<double>*, bool(*)(const char*, const char*)>::iterator map_vect_it;
	vector<double>::iterator vector_double_it;
	vector<const char*>::iterator vector_char_it;

	char* extract_name(const char* full_path, char path_separator) throw (DescriptorException);
	char* extract_path(const char* full_path, char path_separator) throw (DescriptorException);

	vector<double>* get(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException);
	const char* get(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException);

	void add(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key, double value) throw (DescriptorException);
	void add(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key, const char *value) throw (DescriptorException);

	bool remove(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException);
	bool remove(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException);

	void copy(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_to, map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_from) throw (DescriptorException);
	void copy(map<const char* const, char*, bool(*)(const char*, const char*)>& map_to, map<const char* const, char*, bool(*)(const char*, const char*)>& map_from) throw (DescriptorException);
	void copy(vector<const char*>& vector_to, vector<const char*> vector_from) throw (DescriptorException);

	Descriptor();
	Descriptor(const char* full_path, char path_separator = DEFAULT_PATH_SEPARATOR) throw (DescriptorException);
	Descriptor(const char* full_path, Descriptor& file, char path_separator = DEFAULT_PATH_SEPARATOR) throw (DescriptorException);

public:
	static const char* SCRIPT_COMMAND_DELIMITERS;
	static const char SCRIPT_COMMENT_SYMBOL;

	static const char* MARKER;
	static const char* ALLELE;
	static const char* ALLELE1;
	static const char* ALLELE2;
	static const char* CHR;
	static const char* POSITION;
	static const char* STRAND;
	static const char* PVALUE;
	static const char* EFFECT;
	static const char* STDERR;
	static const char* FREQLABEL;
	static const char* HWE_PVAL;
	static const char* CALLRATE;
	static const char* N_TOTAL;
	static const char* IMPUTED;
	static const char* USED_FOR_IMP;
	static const char* OEVAR_IMP;
	static const char* AVPOSTPROB;

	static const char* MAF;
	static const char* IMP;
	static const char* SNP_HQ;

	static const char* ANALYZE;
	static const char* BOXPLOTWIDTH;
	static const char* CASESENSITIVE;
	static const char* GC;
	static const char* GENOMICCONTROL;
	static const char* MISSING;
	static const char* ORDER;
	static const char* OUTFILE;
	static const char* PREFIX;
	static const char* PROCESS;
	static const char* SEPARATOR;
	static const char* VERBOSITY;
	static const char* RENAME;

	static const char* REGIONS_FILE;
	static const char* REGIONS_FILE_SEPARATOR;
	static const char* REGIONS_DEVIATION;
	static const char* REGIONS_APPEND;
	static const char* REGION_NAME;
	static const char* REGION_CHR;
	static const char* REGION_START;
	static const char* REGION_END;

	static const char* MAP_FILE;
	static const char* MAP_FILE_SEPARATOR;
	static const char* MAP_MARKER;
	static const char* MAP_CHR;
	static const char* MAP_POSITION;

	static const char* COMMA;
	static const char* COMMAS;
	static const char* SEMICOLON;
	static const char* TAB;
	static const char* TABULATION;
	static const char* WHITESPACE;

	static const char* ON_MODES[];
	static const char* OFF_MODES[];
	static const char* LOW_MODES[];
	static const char* HIGH_MODES[];

	virtual ~Descriptor();

	char get_path_separator();
	const char* get_name();
	const char* get_path();
	const char* get_full_path();
	const char* get_abbreviation();
	const char* get_extension();
	const char* get_column(const char* common_name) throw (DescriptorException);
	const char* get_default_column(const char* actual_name, bool case_sensitive);
	vector<const char*>* get_default_columns();
	const char* get_property(const char* value) throw (DescriptorException);
	vector<const char*>* get_properties();
	vector<double>* get_threshold(const char* name) throw (DescriptorException);
	vector<const char*>* get_thresholds();
	const char* get_renamed_column(const char* name) throw (DescriptorException);
	int get_column_order(const char* name, bool case_sensitive) throw (DescriptorException);
	vector<const char*>* get_reordered_columns();
	int get_columns_number();
	int get_properties_number();
	int get_thresholds_number();
	int get_reordered_columns_number();

	void set_full_path(const char* full_path, char path_separator = DEFAULT_PATH_SEPARATOR) throw (DescriptorException);

	void set_abbreviation(const char* abbreviation) throw (DescriptorException);
	void set_abbreviation(const char* abbreviation, unsigned int length) throw (DescriptorException);

	void add_column(const char* common_name, const char* actual_name) throw (DescriptorException);
	void add_property(const char* name, const char* value) throw (DescriptorException);
	void add_threshold(const char* name, double value) throw (DescriptorException);
	void add_threshold(const char* name, const char* numeric_value) throw (DescriptorException);
	void add_renamed_column(const char* name, const char* new_name) throw (DescriptorException);
	void add_reordered_column(const char* name) throw (DescriptorException);

	bool remove_column(const char* common_name) throw (DescriptorException);
	bool remove_property(const char* name) throw (DescriptorException);
	bool remove_threshold(const char* name) throw (DescriptorException);
	bool remove_renamed_column(const char* name) throw (DescriptorException);

	void remove_reordered_columns();

	static vector<Descriptor*>* process_instructions(const char* script_name, char path_separator = DEFAULT_PATH_SEPARATOR) throw (DescriptorException);

	friend SEXP Robj2Descriptor(SEXP descriptor_Robj);
	friend SEXP Descriptor2Robj(SEXP descriptor);
};

#endif
