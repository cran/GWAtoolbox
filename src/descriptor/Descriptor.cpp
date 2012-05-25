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

#include "include/Descriptor.h"

const char* Descriptor::SCRIPT_COMMAND_DELIMITERS = " \t";
const char Descriptor::SCRIPT_COMMENT_SYMBOL = '#';

const char* Descriptor::MARKER = "MARKER";
const char* Descriptor::ALLELE = "ALLELE";
const char* Descriptor::ALLELE1 = "ALLELE1";
const char* Descriptor::ALLELE2 = "ALLELE2";
const char* Descriptor::CHR = "CHR";
const char* Descriptor::POSITION = "POSITION";
const char* Descriptor::STRAND = "STRAND";
const char* Descriptor::PVALUE = "PVALUE";
const char* Descriptor::EFFECT = "EFFECT";
const char* Descriptor::STDERR = "STDERR";
const char* Descriptor::FREQLABEL = "FREQLABEL";
const char* Descriptor::HWE_PVAL = "HWE_PVAL";
const char* Descriptor::CALLRATE = "CALLRATE";
const char* Descriptor::N_TOTAL = "N";
const char* Descriptor::IMPUTED = "IMPUTED";
const char* Descriptor::USED_FOR_IMP = "USED_FOR_IMP";
const char* Descriptor::OEVAR_IMP = "IMP_QUALITY";
const char* Descriptor::AVPOSTPROB = "AVPOSTPROB";

const char* Descriptor::MAF = "MAF";
const char* Descriptor::IMP = "IMP";
const char* Descriptor::SNP_HQ = "HQ_SNP";

const char* Descriptor::ANALYZE = "ANALYZE";
const char* Descriptor::BOXPLOTWIDTH = "BOXPLOTWIDTH";
const char* Descriptor::CASESENSITIVE = "CASESENSITIVE";
const char* Descriptor::GC = "GC";
const char* Descriptor::GENOMICCONTROL = "GENOMICCONTROL";
const char* Descriptor::MISSING = "MISSING";
const char* Descriptor::ORDER = "ORDER";
const char* Descriptor::OUTFILE = "OUTFILE";
const char* Descriptor::PREFIX = "PREFIX";
const char* Descriptor::PROCESS = "PROCESS";
const char* Descriptor::SEPARATOR = "SEPARATOR";
const char* Descriptor::VERBOSITY = "VERBOSITY";
const char* Descriptor::RENAME = "RENAME";

const char* Descriptor::REGIONS_FILE = "REGIONS_FILE";
const char* Descriptor::REGIONS_FILE_SEPARATOR = "REGIONS_FILE_SEPARATOR";
const char* Descriptor::REGIONS_DEVIATION = "REGIONS_DEVIATION";
const char* Descriptor::REGIONS_APPEND = "REGIONS_APPEND";
const char* Descriptor::REGION_NAME = "REGION_NAME";
const char* Descriptor::REGION_CHR = "REGION_CHR";
const char* Descriptor::REGION_START = "REGION_START";
const char* Descriptor::REGION_END = "REGION_END";

const char* Descriptor::MAP_FILE = "MAP_FILE";
const char* Descriptor::MAP_FILE_SEPARATOR = "MAP_FILE_SEPARATOR";
const char* Descriptor::MAP_MARKER = "MAP_MARKER";
const char* Descriptor::MAP_CHR = "MAP_CHR";
const char* Descriptor::MAP_POSITION = "MAP_POSITION";

const char* Descriptor::COMMA = "COMMA";
const char* Descriptor::COMMAS = "COMMAS";
const char* Descriptor::SEMICOLON = "SEMICOLON";
const char* Descriptor::TAB = "TAB";
const char* Descriptor::TABULATION = "TABULATION";
const char* Descriptor::WHITESPACE = "WHITESPACE";

const char* Descriptor::ON_MODES[] = {"ON", "1"};
const char* Descriptor::OFF_MODES[] = {"OFF", "0"};
const char* Descriptor::LOW_MODES[] = {"LOW", "1"};
const char* Descriptor::HIGH_MODES[] = {"HIGH", "2"};

Descriptor::Descriptor():
	path_separator('\0'), name(NULL), path(NULL), full_path(NULL),
	abbreviation(NULL), columns(bool_strcmp), properties(bool_strcmp), thresholds(bool_strcmp), renamed_columns(bool_strcmp) {

}

Descriptor::Descriptor(const char* full_path, char path_separator) throw (DescriptorException):
	path_separator(path_separator), name(NULL), path(NULL), full_path(NULL),
	abbreviation(NULL),	columns(bool_strcmp), properties(bool_strcmp), thresholds(bool_strcmp), renamed_columns(bool_strcmp)  {

	if (full_path == NULL) {
		throw DescriptorException("Descriptor", "Descriptor( const char*, char )", __LINE__, 0, "full_path");
	}

	if (strlen(full_path) <= 0) {
		throw DescriptorException("Descriptor", "Descriptor( const char*, char )", __LINE__, 1, "full_path");
	}

	try {
		set_full_path(full_path, path_separator);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "Descriptor( const char*, char )", __LINE__, 3);
		throw;
	}
}

Descriptor::Descriptor(const char* full_path, Descriptor& file, char path_separator) throw (DescriptorException):
	path_separator(path_separator), name(NULL), path(NULL), full_path(NULL),
	abbreviation(NULL), columns(bool_strcmp), properties(bool_strcmp), thresholds(bool_strcmp), renamed_columns(bool_strcmp)  {

	if (full_path == NULL) {
		throw DescriptorException("Descriptor", "Descriptor( const char*, Descriptor&, char )", __LINE__, 0, "full_path");
	}

	if (strlen(full_path) <= 0) {
		throw DescriptorException("Descriptor", "Descriptor( const char*, Descriptor&, char )", __LINE__, 1, "full_path");
	}

	try {
		set_full_path(full_path, path_separator);
		copy(columns, file.columns);
		copy(properties, file.properties);
		copy(thresholds, file.thresholds);
		copy(renamed_columns, file.renamed_columns);
		copy(reordered_columns, file.reordered_columns);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "Descriptor( const char*, Descriptor&, char )", __LINE__, 3);
		throw;
	}
}

Descriptor::~Descriptor() {
	free(name);
	name = NULL;

	free(path);
	path = NULL;

	free(full_path);
	full_path = NULL;

	free(abbreviation);
	abbreviation = NULL;

	for (map_char_it = columns.begin(); map_char_it != columns.end(); map_char_it++) {
		free((char*)map_char_it->first);
		free(map_char_it->second);
	}

	for (map_char_it = properties.begin(); map_char_it != properties.end(); map_char_it++) {
		free((char*)map_char_it->first);
		free(map_char_it->second);
	}

	for (map_vect_it = thresholds.begin(); map_vect_it != thresholds.end(); map_vect_it++) {
		free((char*)map_vect_it->first);
		delete map_vect_it->second;
	}

	for (map_char_it = renamed_columns.begin(); map_char_it != renamed_columns.end(); map_char_it++) {
		free((char*)map_char_it->first);
		free(map_char_it->second);
	}

	for (vector_char_it = reordered_columns.begin(); vector_char_it != reordered_columns.end(); vector_char_it++) {
		free((char*)*vector_char_it);
	}

	columns.clear();
	properties.clear();
	thresholds.clear();
	renamed_columns.clear();
	reordered_columns.clear();
}

char* Descriptor::extract_name(const char* full_path, char path_separator) throw (DescriptorException) {
	int file_name_start = 0, file_name_length = 0, current_position = 0;
	char* file_name = NULL;

	if (full_path == NULL) {
		throw DescriptorException("Descriptor", "char* extract_name( const char*, char )", __LINE__, 0, "full_path");
	}

	if (strlen(full_path) <= 0) {
		throw DescriptorException("Descriptor", "char* extract_name( const char*, char )", __LINE__, 1, "full_path");
	}

	for (int i = strlen(full_path) - 1; i >= 0; i--) {
		if (full_path[i] == path_separator) {
			file_name_start = i + 1;
			break;
		}

		file_name_length += 1;
	}

	if (file_name_length > 0) {
		file_name = (char*)malloc((file_name_length + 1) * sizeof(char));
		if (file_name == NULL) {
			throw DescriptorException("Descriptor", "char* extract_name( const char*, char )", __LINE__, 2, (file_name_length + 1) * sizeof(char));
		}

		for (int i = file_name_start; i < file_name_start + file_name_length; i++) {
			file_name[current_position++] = full_path[i];
		}

		file_name[current_position] = '\0';
	}

	return file_name;
}

char* Descriptor::extract_path(const char* full_path, char path_separator) throw (DescriptorException) {
	int file_path_length = 0;
	char* file_path = NULL;

	if (full_path == NULL) {
		throw DescriptorException("Descriptor", "char* extract_path( const char*, char )", __LINE__, 0, "full_path");
	}

	if (strlen(full_path) <= 0) {
		throw DescriptorException("Descriptor", "char* extract_path( const char*, char )", __LINE__, 1, "full_path");
	}

	for (int i = strlen(full_path) - 1; i >= 0; i--) {
		if (full_path[i] == path_separator) {
			file_path_length = i + 1;
			break;
		}
	}

	if (file_path_length > 0) {
		file_path = (char*)malloc((file_path_length + 1) * sizeof(char));
		if (file_path == NULL) {
			throw DescriptorException("Descriptor", "char* extract_path( const char*, char )", __LINE__, 2, (file_path_length + 1) * sizeof(char));
		}
		strncpy(file_path, full_path, file_path_length);
		file_path[file_path_length] = '\0';
	}

	return file_path;
}

void Descriptor::set_full_path(const char* full_path, char path_separator) throw (DescriptorException) {
	if (full_path == NULL) {
		throw DescriptorException("Descriptor", "set_name_and_paths( const char*, char )", __LINE__, 0, "full_path");
	}

	if (strlen(full_path) <= 0) {
		throw DescriptorException("Descriptor", "set_name_and_paths( const char*, char )", __LINE__, 1, "full_path");
	}

	free(this->full_path);
	this->full_path = NULL;
	this->full_path = (char*)malloc((strlen(full_path) + 1) * sizeof(char));
	if (this->full_path == NULL) {
		throw DescriptorException("Descriptor", "set_name_and_paths( const char*, char )", __LINE__, 2, (strlen(full_path) + 1) * sizeof(char));
	}
	strcpy(this->full_path, full_path);

	this->path_separator = path_separator;

	free(name);
	name = NULL;
	name = extract_name(this->full_path, path_separator);

	free(path);
	path = NULL;
	path = extract_path(this->full_path, path_separator);
}

vector<double>* Descriptor::get(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException) {
	if (key == NULL) {
		throw DescriptorException("Descriptor", "vector<double>* get( map<const char* const, vector<double>*>&, const char* )", __LINE__, 0, "key");
	}

	if (strlen(key) <= 0) {
		throw DescriptorException("Descriptor", "vector<double>* get( map<const char* const, vector<double>*>&, const char* )", __LINE__, 1, "key");
	}

	map_vect_it = map.find(key);
	if (map_vect_it != map.end()) {
		return map_vect_it->second;
	}

	return NULL;
}

const char* Descriptor::get(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException) {
	if (key == NULL) {
		throw DescriptorException("Descriptor", "const char* get( map<const char* const, char*> )", __LINE__, 0, "key");
	}

	if (strlen(key) <= 0) {
		throw DescriptorException("Descriptor", "const char* get( map<const char* const, char*> )", __LINE__, 1, "key");
	}

	map_char_it = map.find(key);
	if (map_char_it != map.end()) {
		return map_char_it->second;
	}

	return NULL;
}

void Descriptor::add(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key, double value) throw (DescriptorException) {
	if (key == NULL) {
		throw DescriptorException("Descriptor", "add( map<const char* const, vector<double>*>&, const char*, double )", __LINE__, 0, "key");
	}

	if (strlen(key) <= 0) {
		throw DescriptorException("Descriptor", "add( map<const char* const, vector<double>*>&, const char*, double )", __LINE__, 1, "key");
	}

	map_vect_it = map.find(key);
	if (map_vect_it != map.end()) {
		map_vect_it->second->push_back(value);
	}
	else {
		vector<double>* values = NULL;
		char* key_copy = NULL;

		key_copy = (char*)malloc((strlen(key) + 1) * sizeof(char));
		if (key_copy == NULL) {
			throw DescriptorException("Descriptor", "add( map<const char* const, vector<double>*>&, const char*, double )", __LINE__, 2, (strlen(key) + 1) * sizeof(char));
		}
		strcpy(key_copy, key);

		values = new vector<double>();

		values->push_back(value);
		map.insert(pair<const char* const, vector<double>*>(key_copy, values));
	}
}

void Descriptor::add(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key, const char* value) throw (DescriptorException) {
	if (key == NULL) {
		throw DescriptorException("Descriptor", "add( map<const char* const, char*>&, const char* key, const char* value )", __LINE__, 0, "key");
	}

	if (strlen(key) <= 0) {
		throw DescriptorException("Descriptor", "add( map<const char* const, char*>&, const char* key, const char* value )", __LINE__, 1, "key");
	}

	if (value == NULL) {
		throw DescriptorException("Descriptor", "add( map<const char* const, char*>&, const char* key, const char* value )", __LINE__, 0, "value");
	}

	if (strlen(value) <= 0) {
		throw DescriptorException("Descriptor", "add( map<const char* const, char*>&, const char* key, const char* value )", __LINE__, 1, "value");
	}

	char* value_copy = NULL;

	value_copy = (char*)malloc((strlen(value) + 1) * sizeof(char));
	if (value_copy == NULL) {
		throw DescriptorException("Descriptor", "add( map<const char* const, char*>&, const char* key, const char* value )", __LINE__, 2, (strlen(value) + 1) * sizeof(char));
	}
	strcpy(value_copy, value);

	map_char_it = map.find(key);
	if (map_char_it != map.end()) {
		free(map_char_it->second);
		map_char_it->second = value_copy;
	}
	else {
		char* key_copy = NULL;

		key_copy = (char*)malloc((strlen(key) + 1) * sizeof(char));
		if (key_copy == NULL) {
			free(value_copy);
			throw DescriptorException("Descriptor", "add( map<const char* const, char*>&, const char* key, const char* value )", __LINE__, 2, (strlen(key) + 1) * sizeof(char));
		}
		strcpy(key_copy, key);

		map.insert(pair<const char* const, char*>(key_copy, value_copy));
	}
}

bool Descriptor::remove(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException) {
	if (key == NULL) {
		throw DescriptorException("Descriptor", "bool remove( map<const char* const, vector<double>*>&, const char* )", __LINE__, 0, "key");
	}

	if (strlen(key) <= 0) {
		throw DescriptorException("Descriptor", "bool remove( map<const char* const, vector<double>*>&, const char* )", __LINE__, 1, "key");
	}

	map_vect_it = map.find(key);
	if (map_vect_it != map.end()) {
		free((char*)map_vect_it->first);
		delete map_vect_it->second;
		map.erase(map_vect_it);
		return true;
	}

	return false;
}

bool Descriptor::remove(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (DescriptorException) {
	if (key == NULL) {
		throw DescriptorException("Descriptor", "bool remove( map<const char* const, char*>& , const char* )", __LINE__, 0, "key");
	}

	if (strlen(key) <= 0) {
		throw DescriptorException("Descriptor", "bool remove( map<const char* const, char*>& , const char* )", __LINE__, 1, "key");
	}

	map_char_it = map.find(key);
	if (map_char_it != map.end()) {
		free((char*)map_char_it->first);
		free(map_char_it->second);
		map.erase(map_char_it);
		return true;
	}

	return false;
}

void Descriptor::copy(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_to, map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_from) throw (DescriptorException) {
	char* key_copy = NULL;
	vector<double>* values = NULL;

	for (map_vect_it = map_from.begin(); map_vect_it != map_from.end(); map_vect_it++) {
		key_copy = (char*)malloc((strlen(map_vect_it->first) + 1) * sizeof(char));
		if (key_copy == NULL) {
			throw DescriptorException("Descriptor", "copy( map<const char* const, vector<double>*>&, map<const char* const, vector<double>*>& )", __LINE__, 2, (strlen(map_vect_it->first) + 1) * sizeof(char));
		}
		strcpy(key_copy, map_vect_it->first);

		values = new vector<double>();
		vector_double_it = map_vect_it->second->begin();
		while (vector_double_it != map_vect_it->second->end()) {
			values->push_back(*vector_double_it);
			vector_double_it++;
		}

		map_to.insert(pair<const char* const, vector<double>*>(key_copy, values));
	}
}

void Descriptor::copy(map<const char* const, char*, bool(*)(const char*, const char*)>& map_to, map<const char* const, char*, bool(*)(const char*, const char*)>& map_from) throw (DescriptorException) {
	char* key_copy = NULL;
	char* value_copy = NULL;

	for (map_char_it = map_from.begin(); map_char_it != map_from.end(); map_char_it++) {
		key_copy = (char*)malloc((strlen(map_char_it->first) + 1) * sizeof(char));
		if (key_copy == NULL) {
			throw DescriptorException("Descriptor", "copy( map<const char* const, char*>&, map<const char* const, char*>& )", __LINE__, 2, (strlen(map_char_it->first) + 1) * sizeof(char));
		}

		value_copy = (char*)malloc((strlen(map_char_it->second) + 1) * sizeof(char));
		if (value_copy == NULL) {
			free(key_copy);
			throw DescriptorException("Descriptor", "copy( map<const char* const, char*>&, map<const char* const, char*>& )", __LINE__, 2, (strlen(map_char_it->second) + 1) * sizeof(char));
		}

		strcpy(key_copy, map_char_it->first);
		strcpy(value_copy, map_char_it->second);

		map_to.insert(pair<const char* const, char*>(key_copy, value_copy));
	}
}

void Descriptor::copy(vector<const char*>& vector_to, vector<const char*> vector_from) throw (DescriptorException) {
	char* value_copy = NULL;

	for (vector_char_it = vector_from.begin(); vector_char_it != vector_from.end(); vector_char_it++) {
		value_copy = (char*)malloc((strlen(*vector_char_it) + 1) * sizeof(char));
		if (value_copy == NULL) {
			throw DescriptorException("Descriptor", "copy( vector<const char*>& , vector<const char*>& )", __LINE__, 2, (strlen(*vector_char_it) + 1) * sizeof(char));
		}
		strcpy(value_copy, *vector_char_it);
		vector_to.push_back(value_copy);
	}
}

char Descriptor::get_path_separator() {
	return path_separator;
}

const char* Descriptor::get_name() {
	return name;
}

const char* Descriptor::get_path() {
	return path;
}

const char* Descriptor::get_full_path() {
	return full_path;
}

const char* Descriptor::get_abbreviation() {
	return abbreviation;
}

const char* Descriptor::get_extension() {
	if (name != NULL) {
		return strrchr(name, '.');
	}

	return NULL;
}

const char* Descriptor::get_column(const char* common_name) throw (DescriptorException) {
	try {
		return get(columns, common_name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "const char* get_column( const char* )", __LINE__, 11);
		throw;
	}
}

const char* Descriptor::get_default_column(const char* actual_name, bool case_sensitive) {
	if ((actual_name != NULL) && (strlen(actual_name) > 0)) {
		if (case_sensitive) {
			for (map_char_it = columns.begin(); map_char_it != columns.end(); map_char_it++) {
				if (strcmp(map_char_it->second, actual_name) == 0) {
					return map_char_it->first;
				}
			}
		} else {
			for (map_char_it = columns.begin(); map_char_it != columns.end(); map_char_it++) {
				if (strcmp_ignore_case(map_char_it->second, actual_name) == 0) {
					return map_char_it->first;
				}
			}
		}
	}

	return NULL;
}

vector<const char*>* Descriptor::get_default_columns() {
	vector<const char*>* default_columns = new vector<const char*>();

	map_char_it = columns.begin();
	while (map_char_it != columns.end()) {
		default_columns->push_back(map_char_it->first);
		map_char_it++;
	}

	return default_columns;
}

int Descriptor::get_columns_number() {
	return columns.size();
}

const char* Descriptor::get_property(const char* value) throw (DescriptorException) {
	try {
		return get(properties, value);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "const char* get_property( const char* )", __LINE__, 12);
		throw;
	}
}

vector<const char*>* Descriptor::get_properties() {
	vector<const char*>* properties = new vector<const char*>();

	map_char_it = this->properties.begin();
	while (map_char_it != this->properties.end()) {
		properties->push_back(map_char_it->first);
		map_char_it++;
	}

	return properties;
}

int Descriptor::get_properties_number() {
	return properties.size();
}

vector<double>* Descriptor::get_threshold(const char* name) throw (DescriptorException) {
	try {
		return get(thresholds, name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "vector<double>* get_threshold( const char* )", __LINE__, 13);
		throw;
	}
}

vector<const char*>* Descriptor::get_thresholds() {
	vector<const char*>* thresholds = new vector<const char*>();

	map_vect_it = this->thresholds.begin();
	while (map_vect_it != this->thresholds.end()) {
		thresholds->push_back(map_vect_it->first);
		map_vect_it++;
	}

	return thresholds;
}

int Descriptor::get_thresholds_number() {
	return thresholds.size();
}

const char* Descriptor::get_renamed_column(const char* name) throw (DescriptorException) {
	try {
		return get(renamed_columns, name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "const char* get_new_column_name( const char* )", __LINE__, 18);
		throw;
	}
}

int Descriptor::get_column_order(const char* name, bool case_sensitive) throw (DescriptorException) {
	if ((name != NULL) && (strlen(name) > 0)) {
		int position = 0;
		if (case_sensitive) {
			vector_char_it = reordered_columns.begin();
			while (vector_char_it != reordered_columns.end()) {
				if (strcmp(*vector_char_it, name) == 0) {
					return position;
				}
				position += 1;
				vector_char_it++;
			}
		} else {
			vector_char_it = reordered_columns.begin();
			while (vector_char_it != reordered_columns.end()) {
				if (strcmp_ignore_case(*vector_char_it, name) == 0) {
					return position;
				}
				position += 1;
				vector_char_it++;
			}
		}
	}

	return numeric_limits<int>::max();
}

vector<const char*>* Descriptor::get_reordered_columns() {
	vector<const char*>* reordered_columns = new vector<const char*>();

	vector_char_it = this->reordered_columns.begin();
	while (vector_char_it != this->reordered_columns.end()) {
		reordered_columns->push_back(*vector_char_it);
		vector_char_it++;
	}

	return reordered_columns;
}

int Descriptor::get_reordered_columns_number() {
	return reordered_columns.size();
}

void Descriptor::set_abbreviation(const char* abbreviation) throw (DescriptorException) {
	if (abbreviation == NULL) {
		throw DescriptorException("Descriptor", "set_abbreviation( const char* )", __LINE__, 0, "abbreviation");
	}

	if (strlen(abbreviation) <= 0) {
		throw DescriptorException("Descriptor", "set_abbreviation( const char* )", __LINE__, 1, "abbreviation");
	}

	free(this->abbreviation);
	this->abbreviation = NULL;
	this->abbreviation = (char*)malloc((strlen(abbreviation) + 1) * sizeof(char));
	if (this->abbreviation == NULL) {
		throw DescriptorException("Descriptor", "set_abbreviation( const char* )", __LINE__, 2, (strlen(abbreviation) + 1) * sizeof(char));
	}
	strcpy(this->abbreviation, abbreviation);
}

void Descriptor::set_abbreviation(const char* abbreviation, unsigned int length) throw (DescriptorException) {
	if (abbreviation == NULL) {
		throw DescriptorException("Descriptor", "set_abbreviation( const char*, unsigned int )", __LINE__, 0, "abbreviation");
	}

	if (strlen(abbreviation) <= 0) {
		throw DescriptorException("Descriptor", "set_abbreviation( const char*, unsigned int )", __LINE__, 1, "abbreviation");
	}

	if ((length <= 0) || (length > strlen(abbreviation))) {
		length = strlen(abbreviation);
	}

	free(this->abbreviation);
	this->abbreviation = NULL;
	this->abbreviation = (char*)malloc((length + 1) * sizeof(char));
	if (this->abbreviation == NULL) {
		throw DescriptorException("Descriptor", "set_abbreviation( const char*, unsigned int )", __LINE__, 2, (length + 1) * sizeof(char));
	}
	strncpy(this->abbreviation, abbreviation, length);
	this->abbreviation[length] = '\0';
}

void Descriptor::add_column(const char* common_name, const char* actual_name) throw (DescriptorException) {
	try {
		add(columns, common_name, actual_name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "add_column( const char*, const char* )", __LINE__, 5);
		throw;
	}
}

void Descriptor::add_property(const char* name, const char* value) throw (DescriptorException) {
	try {
		add(properties, name, value);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "add_property( const char*, const char* )", __LINE__, 6);
		throw;
	}
}

void Descriptor::add_threshold(const char* name, double value) throw (DescriptorException) {
	try {
		add(thresholds, name, value);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "add_threshold( const char*, double )", __LINE__, 7);
		throw;
	}
}

void Descriptor::add_threshold(const char* name, const char* numeric_value) throw (DescriptorException) {
	double d_value = 0.0;
	char* end_ptr = NULL;

	if (numeric_value == NULL) {
		throw DescriptorException("Descriptor", "add_threshold( const char*, const char* )", __LINE__, 0, "numeric_value");
	}

	if (strlen(numeric_value) <= 0) {
		throw DescriptorException("Descriptor", "add_threshold( const char*, const char* )", __LINE__, 1, "numeric_value");
	}

	d_value = strtod(numeric_value, &end_ptr);
	if (*end_ptr != '\0') {
		throw DescriptorException("Descriptor", "add_threshold( const char*, const char* )", __LINE__, 4, numeric_value);
	}

	try {
		add(thresholds, name, d_value);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "add_threshold( const char*, const char* )", __LINE__, 7);
		throw;
	}
}

void Descriptor::add_renamed_column(const char* name, const char* new_name) throw (DescriptorException) {
	try {
		add(renamed_columns, name, new_name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "add_new_column_name( const char*, const char* )", __LINE__, 16);
		throw;
	}
}

void Descriptor::add_reordered_column(const char* name) throw (DescriptorException) {
	char* name_copy = NULL;

	try {
		if (name == NULL) {
			throw DescriptorException("Descriptor", "add_reordered_column( const char* )", __LINE__, 0, "name");
		}

		if (strlen(name) <= 0) {
			throw DescriptorException("Descriptor", "add_reordered_column( const char* )", __LINE__, 1, "name");
		}

		name_copy = (char*)malloc((strlen(name) + 1) * sizeof(char));
		if (name_copy == NULL) {
			throw DescriptorException("Descriptor", "add_reordered_column( const char* )", __LINE__, 2, (strlen(name) + 1) * sizeof(char));
		}
		strcpy(name_copy, name);

		reordered_columns.push_back(name_copy);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "add_reordered_column( const char* )", __LINE__, 19);
	}
}

bool Descriptor::remove_column(const char* common_name) throw (DescriptorException) {
	try {
		return remove(columns, common_name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "bool remove_column( const char* )", __LINE__, 8);
		throw;
	}
}

bool Descriptor::remove_property(const char* name) throw (DescriptorException) {
	try {
		return remove(properties, name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "bool remove_property( const char* )", __LINE__, 9);
		throw;
	}
}

bool Descriptor::remove_threshold(const char* name) throw (DescriptorException) {
	try {
		return remove(thresholds, name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "bool remove_threshold( const char* )", __LINE__, 10);
		throw;
	}
}

bool Descriptor::remove_renamed_column(const char* name) throw (DescriptorException) {
	try {
		return remove(renamed_columns, name);
	} catch (DescriptorException &e) {
		e.add_message("Descriptor", "bool remove_new_column_name( const char* )", __LINE__, 17);
		throw;
	}
}

void Descriptor::remove_reordered_columns() {
	for (vector_char_it = reordered_columns.begin(); vector_char_it != reordered_columns.end(); vector_char_it++) {
		free((char*)*vector_char_it);
	}

	reordered_columns.clear();
}

vector<Descriptor*>* Descriptor::process_instructions(const char* script_name, char path_separator) throw (DescriptorException) {
	list<char*> tokens;
	char* token = NULL;

	vector<Descriptor*>* descriptors = new vector<Descriptor*>();
	Descriptor default_descriptor;
	Descriptor* new_descriptor = NULL;

	if (script_name == NULL) {
		throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 0, "script_name");
	}

	if (strlen(script_name) <= 0) {
		throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 1, "script_name");
	}

	try {
		default_descriptor.add_column(MARKER, MARKER);
		default_descriptor.add_column(ALLELE1, ALLELE1);
		default_descriptor.add_column(ALLELE2, ALLELE2);
		default_descriptor.add_column(CHR, CHR);
		default_descriptor.add_column(POSITION, POSITION);
		default_descriptor.add_column(STRAND, STRAND);
		default_descriptor.add_column(PVALUE, PVALUE);
		default_descriptor.add_column(EFFECT, EFFECT);
		default_descriptor.add_column(STDERR, STDERR);
		default_descriptor.add_column(FREQLABEL, FREQLABEL);
		default_descriptor.add_column(HWE_PVAL, HWE_PVAL);
		default_descriptor.add_column(CALLRATE, CALLRATE);
		default_descriptor.add_column(N_TOTAL, N_TOTAL);
		default_descriptor.add_column(IMPUTED, IMPUTED);
		default_descriptor.add_column(USED_FOR_IMP, USED_FOR_IMP);
		default_descriptor.add_column(OEVAR_IMP, OEVAR_IMP);
		default_descriptor.add_column(AVPOSTPROB, AVPOSTPROB);

		default_descriptor.add_property(REGION_NAME, REGION_NAME);
		default_descriptor.add_property(REGION_CHR, REGION_CHR);
		default_descriptor.add_property(REGION_START, REGION_START);
		default_descriptor.add_property(REGION_END, REGION_END);

		default_descriptor.add_property(MAP_MARKER, MAP_MARKER);
		default_descriptor.add_property(MAP_CHR, MAP_CHR);
		default_descriptor.add_property(MAP_POSITION, MAP_POSITION);

		TextReader reader;

		reader.set_file_name(script_name);
		reader.open();

		int length = 0;

		while ((length = reader.read_line()) >= 0) {
			token = strtok(*reader.line, SCRIPT_COMMAND_DELIMITERS);
			while (token != NULL) {
				tokens.push_back(token);
				token = strtok(NULL, SCRIPT_COMMAND_DELIMITERS);
			}

			if (tokens.size() > 0) {
				token = tokens.front();
				tokens.pop_front();

				if (strcmp(token, MARKER) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(MARKER, tokens.front());
					}
				} else if (strcmp(token, ALLELE) == 0) {
					if (tokens.size() >= 2) {
						default_descriptor.add_column(ALLELE1, tokens.front());
						tokens.pop_front();
						default_descriptor.add_column(ALLELE2, tokens.front());
					}
				} else if (strcmp(token, CHR) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(CHR, tokens.front());
					}
				} else if (strcmp(token, POSITION) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(POSITION, tokens.front());
					}
				} else if (strcmp(token, STRAND) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(STRAND, tokens.front());
					}
				} else if (strcmp(token, PVALUE) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(PVALUE, tokens.front());
						tokens.pop_front();

						if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
							default_descriptor.remove_threshold(PVALUE);

							do {
								default_descriptor.add_threshold(PVALUE, tokens.front());
								tokens.pop_front();
							} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
						}
					}
				} else if (strcmp(token, EFFECT) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(EFFECT, tokens.front());
					}
				} else if (strcmp(token, STDERR) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(STDERR, tokens.front());
						tokens.pop_front();

						if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
							default_descriptor.remove_threshold(STDERR);

							do {
								default_descriptor.add_threshold(STDERR, tokens.front());
								tokens.pop_front();
							} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
						}
					}
				} else if (strcmp(token, FREQLABEL) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(FREQLABEL, tokens.front());
						tokens.pop_front();

						if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
							default_descriptor.remove_threshold(FREQLABEL);

							do {
								default_descriptor.add_threshold(FREQLABEL, tokens.front());
								tokens.pop_front();
							} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
						}
					}
				} else if (strcmp(token, HWE_PVAL) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(HWE_PVAL, tokens.front());
						tokens.pop_front();

						if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
							default_descriptor.remove_threshold(HWE_PVAL);

							do {
								default_descriptor.add_threshold(HWE_PVAL, tokens.front());
								tokens.pop_front();
							} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
						}
					}
				} else if (strcmp(token, CALLRATE) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(CALLRATE, tokens.front());
						tokens.pop_front();

						if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
							default_descriptor.remove_threshold(CALLRATE);

							do {
								default_descriptor.add_threshold(CALLRATE, tokens.front());
								tokens.pop_front();
							} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
						}
					}
				} else if (strcmp(token, N_TOTAL) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(N_TOTAL, tokens.front());
					}
				} else if (strcmp(token, IMPUTED) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(IMPUTED, tokens.front());
					}
				} else if (strcmp(token, USED_FOR_IMP) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(USED_FOR_IMP, tokens.front());
					}
				} else if (strcmp(token, OEVAR_IMP) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(OEVAR_IMP, tokens.front());
						tokens.pop_front();

						if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
							default_descriptor.remove_threshold(OEVAR_IMP);

							do {
								default_descriptor.add_threshold(OEVAR_IMP, tokens.front());
								tokens.pop_front();
							} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
						}
					}
				} else if (strcmp(token, MAF) == 0) {
					if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
						default_descriptor.remove_threshold(MAF);

						do {
							default_descriptor.add_threshold(MAF, tokens.front());
							tokens.pop_front();
						} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
					}
				} else if (strcmp(token, IMP) == 0) {
					if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
						default_descriptor.remove_threshold(IMP);

						do {
							default_descriptor.add_threshold(IMP, tokens.front());
							tokens.pop_front();
						} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
					}
				} else if (strcmp(token, SNP_HQ) == 0) {
					if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
						default_descriptor.remove_threshold(SNP_HQ);

						do {
							default_descriptor.add_threshold(SNP_HQ, tokens.front());
							tokens.pop_front();
						} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
					}
				} else if (strcmp(token, ANALYZE) == 0) {

				} else if (strcmp(token, BOXPLOTWIDTH) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(BOXPLOTWIDTH, tokens.front());
					}
				} else if (strcmp(token, CASESENSITIVE) == 0) {
					if (!tokens.empty()) {
						if ((strcmp_ignore_case(tokens.front(), ON_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), ON_MODES[1]) == 0)) {
							default_descriptor.add_property(CASESENSITIVE, ON_MODES[0]);
						} else if ((strcmp_ignore_case(tokens.front(), OFF_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), OFF_MODES[1]) == 0)) {
							default_descriptor.add_property(CASESENSITIVE, OFF_MODES[0]);
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), CASESENSITIVE);
						}
					}
				} else if ((strcmp(token, GC) == 0) || (strcmp(token, GENOMICCONTROL) == 0)) {
					if (!tokens.empty()) {
						if (strcmp_ignore_case(tokens.front(), ON_MODES[0]) == 0) {
							default_descriptor.add_property(GENOMICCONTROL, ON_MODES[0]);
						} else if (strcmp_ignore_case(tokens.front(), OFF_MODES[0]) == 0) {
							default_descriptor.add_property(GENOMICCONTROL, OFF_MODES[0]);
						} else if (is_numeric(tokens.front())){
							default_descriptor.add_property(GENOMICCONTROL, tokens.front());
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), GENOMICCONTROL);
						}
					}
				} else if (strcmp(token, MISSING) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(MISSING, tokens.front());
					}
				} else if (strcmp(token, ORDER) == 0) {
					if (!tokens.empty()) {
						if ((strcmp_ignore_case(tokens.front(), ON_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), ON_MODES[1]) == 0)) {
							default_descriptor.add_property(ORDER, ON_MODES[0]);
							default_descriptor.remove_reordered_columns();

							tokens.pop_front();
							if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
								do {
									default_descriptor.add_reordered_column(tokens.front());
									tokens.pop_front();
								} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
							}
						} else if ((strcmp_ignore_case(tokens.front(), OFF_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), OFF_MODES[1]) == 0)) {
							default_descriptor.add_property(ORDER, OFF_MODES[0]);
							default_descriptor.remove_reordered_columns();
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), ORDER);
						}
					}
				} else if (strcmp(token, OUTFILE) == 0) {

				} else if (strcmp(token, PREFIX) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(PREFIX, tokens.front());
					}
				} else if (strcmp(token, PROCESS) == 0) {
					if (!tokens.empty()) {
						new_descriptor = new Descriptor(tokens.front(), default_descriptor, path_separator);
						tokens.pop_front();

						if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
							new_descriptor->set_abbreviation(tokens.front());
						}

						descriptors->push_back(new_descriptor);
					}
				} else if (strcmp(token, SEPARATOR) == 0) {
					if (!tokens.empty()) {
						if ((strcmp_ignore_case(tokens.front(), COMMA) == 0) ||
								(strcmp_ignore_case(tokens.front(), COMMAS) == 0)) {
							default_descriptor.add_property(SEPARATOR, COMMA);
						} else if (strcmp_ignore_case(tokens.front(), SEMICOLON) == 0) {
							default_descriptor.add_property(SEPARATOR, SEMICOLON);
						} else if ((strcmp_ignore_case(tokens.front(), TAB) == 0) ||
								(strcmp_ignore_case(tokens.front(), TABULATION) == 0)) {
							default_descriptor.add_property(SEPARATOR, TABULATION);
						} else if (strcmp_ignore_case(tokens.front(), WHITESPACE) == 0) {
							default_descriptor.add_property(SEPARATOR, WHITESPACE);
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), SEPARATOR);
						}
					}
				} else if (strcmp(token, VERBOSITY) == 0) {
					if (!tokens.empty()) {
						if ((strcmp_ignore_case(tokens.front(), LOW_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), LOW_MODES[1]) == 0)) {
							default_descriptor.add_property(VERBOSITY, LOW_MODES[0]);
						} else if ((strcmp_ignore_case(tokens.front(), HIGH_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), HIGH_MODES[1]) == 0)) {
							default_descriptor.add_property(VERBOSITY, HIGH_MODES[0]);
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), VERBOSITY);
						}
					}
				} else if (strcmp(token, RENAME) == 0) {
					if (tokens.size() >= 2) {
						token = tokens.front();
						tokens.pop_front();
						default_descriptor.add_renamed_column(token, tokens.front());
					}
				} else if (strcmp(token, AVPOSTPROB) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_column(AVPOSTPROB, tokens.front());
					}
				} else if (strcmp(token, REGIONS_FILE) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(REGIONS_FILE, tokens.front());
					}
				} else if (strcmp(token, REGIONS_FILE_SEPARATOR) == 0) {
					if (!tokens.empty()) {
						if ((strcmp_ignore_case(tokens.front(), COMMA) == 0) ||
								(strcmp_ignore_case(tokens.front(), COMMAS) == 0)) {
							default_descriptor.add_property(REGIONS_FILE_SEPARATOR, COMMA);
						} else if (strcmp_ignore_case(tokens.front(), SEMICOLON) == 0) {
							default_descriptor.add_property(REGIONS_FILE_SEPARATOR, SEMICOLON);
						} else if ((strcmp_ignore_case(tokens.front(), TAB) == 0) ||
								(strcmp_ignore_case(tokens.front(), TABULATION) == 0)) {
							default_descriptor.add_property(REGIONS_FILE_SEPARATOR, TABULATION);
						} else if (strcmp_ignore_case(tokens.front(), WHITESPACE) == 0) {
							default_descriptor.add_property(REGIONS_FILE_SEPARATOR, WHITESPACE);
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), REGIONS_FILE_SEPARATOR);
						}
					}
				} else if (strcmp(token, REGIONS_DEVIATION) == 0) {
					if ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL)) {
						default_descriptor.remove_threshold(REGIONS_DEVIATION);

						do {
							default_descriptor.add_threshold(REGIONS_DEVIATION, tokens.front());
							tokens.pop_front();
						} while ((!tokens.empty()) && (tokens.front()[0] != SCRIPT_COMMENT_SYMBOL));
					}
				} else if (strcmp(token, REGIONS_APPEND) == 0) {
					if (!tokens.empty()) {
						if ((strcmp_ignore_case(tokens.front(), ON_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), ON_MODES[1]) == 0)) {
							default_descriptor.add_property(REGIONS_APPEND, ON_MODES[0]);
						} else if ((strcmp_ignore_case(tokens.front(), OFF_MODES[0]) == 0) ||
								(strcmp_ignore_case(tokens.front(), OFF_MODES[1]) == 0)) {
							default_descriptor.add_property(REGIONS_APPEND, OFF_MODES[0]);
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), REGIONS_APPEND);
						}
					}
				} else if (strcmp(token, REGION_NAME) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(REGION_NAME, tokens.front());
					}
				} else if (strcmp(token, REGION_CHR) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(REGION_CHR, tokens.front());
					}
				} else if (strcmp(token, REGION_START) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(REGION_START, tokens.front());
					}
				} else if (strcmp(token, REGION_END) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(REGION_END, tokens.front());
					}
				} else if (strcmp(token, MAP_FILE) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(MAP_FILE, tokens.front());
					}
				} else if (strcmp(token, MAP_FILE_SEPARATOR) == 0) {
					if (!tokens.empty()) {
						if ((strcmp_ignore_case(tokens.front(), COMMA) == 0) ||
								(strcmp_ignore_case(tokens.front(), COMMAS) == 0)) {
							default_descriptor.add_property(MAP_FILE_SEPARATOR, COMMA);
						} else if (strcmp_ignore_case(tokens.front(), SEMICOLON) == 0) {
							default_descriptor.add_property(MAP_FILE_SEPARATOR, SEMICOLON);
						} else if ((strcmp_ignore_case(tokens.front(), TAB) == 0) ||
								(strcmp_ignore_case(tokens.front(), TABULATION) == 0)) {
							default_descriptor.add_property(MAP_FILE_SEPARATOR, TABULATION);
						} else if (strcmp_ignore_case(tokens.front(), WHITESPACE) == 0) {
							default_descriptor.add_property(MAP_FILE_SEPARATOR, WHITESPACE);
						} else {
							throw DescriptorException("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 15, tokens.front(), MAP_FILE_SEPARATOR);
						}
					}
			 	} else if (strcmp(token, MAP_MARKER) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(MAP_MARKER, tokens.front());
					}
				} else if (strcmp(token, MAP_CHR) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(MAP_CHR, tokens.front());
					}
				} else if (strcmp(token, MAP_POSITION) == 0) {
					if (!tokens.empty()) {
						default_descriptor.add_property(MAP_POSITION, tokens.front());
					}
				}
				tokens.clear();
			}
		}

		reader.close();
	} catch (Exception &e) {
		DescriptorException new_e(e);
		new_e.add_message("Descriptor", "vector<Descriptor*>* process_instructions( const char*, char )", __LINE__, 14, script_name);
		throw new_e;
	}

	return descriptors;
}
