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

#include "../../include/file/File.h"

File::File():
	file_name(NULL), file_path(NULL), full_path(NULL), abbreviation(NULL),
	output_prefix(NULL), output_name(NULL), file_separator('\0'), case_sensitive(false),
	columns(compare), properties(compare), thresholds(compare), estimated_lines_count(0) {

}

File::File(const char* path, char file_sep) throw (FileException):
	file_name(NULL), file_path(NULL), full_path(NULL), abbreviation(NULL),
	output_prefix(NULL), output_name(NULL), file_separator(file_sep), case_sensitive(false),
	columns(compare), properties(compare), thresholds(compare), estimated_lines_count(0) {

	if (path == NULL) {
		throw FileException(1, NULL, "File(const char* path)", "path", "NULL");
	}

	if (strlen(path) <= 0) {
		throw FileException(2, NULL, "File(const char* path)", "path", NULL);
	}

	set_full_path(path, file_separator);
}

File::File(const char* path, File& file, char file_sep) throw (FileException):
	file_name(NULL), file_path(NULL), full_path(NULL), abbreviation(NULL),
	output_prefix(NULL), output_name(NULL), file_separator(file_sep), case_sensitive(false),
	columns(compare), properties(compare), thresholds(compare), estimated_lines_count(0) {

	if (path == NULL) {
		throw FileException(1, NULL, "File(const char* path, File& file)", "path", "NULL");
	}

	if (strlen(path) <= 0) {
		throw FileException(2, NULL, "File(const char* path, File& file)", "path", NULL);
	}

	set_full_path(path, file_separator);

	try {
		copy(columns, file.columns);
	} catch (FileException &e) {
		FileException* new_e = new FileException(12, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}

	try {
		copy(properties, file.properties);
	} catch (FileException &e) {
		FileException* new_exception = new FileException(13, NULL, NULL, NULL, NULL);
		new_exception->set_source(e);
		throw *new_exception;
	}

	try {
		copy(thresholds, file.thresholds);
	} catch (FileException &e) {
		FileException* new_exception = new FileException(14, NULL, NULL, NULL, NULL);
		new_exception->set_source(e);
		throw *new_exception;
	}
}

File::~File() {
	vector<char*>::iterator char_it;
	vector<Meta*>::iterator meta_it;
	vector<MetaFiltered*>::iterator filtered_metas_it;
	vector<MetaRatio*>::iterator ratio_metas_it;
	vector<MetaCrossTable*>::iterator cross_table_metas_it;

	free(file_name);
	file_name = NULL;

	free(file_path);
	file_path = NULL;

	free(full_path);
	full_path = NULL;

	free(abbreviation);
	abbreviation = NULL;

	free(output_prefix);
	output_prefix = NULL;

	free(output_name);
	output_name = NULL;

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

	for (char_it = missed_columns.begin(); char_it != missed_columns.end(); char_it++) {
		free(*char_it);
	}

	for (char_it = not_required_columns.begin(); char_it != not_required_columns.end(); char_it++) {
		free(*char_it);
	}

	for (meta_it = metas.begin(); meta_it != metas.end(); meta_it++) {
		if ((*meta_it) != NULL) {
			delete *meta_it;
		}
	}

	for (filtered_metas_it = filtered_metas.begin(); filtered_metas_it != filtered_metas.end(); filtered_metas_it++) {
		delete *filtered_metas_it;
	}

	for (ratio_metas_it = ratio_metas.begin(); ratio_metas_it != ratio_metas.end(); ratio_metas_it++) {
		delete *ratio_metas_it;
	}

	for (cross_table_metas_it = cross_table_metas.begin(); cross_table_metas_it != cross_table_metas.end(); cross_table_metas_it++) {
		delete *cross_table_metas_it;
	}

	columns.clear();
	properties.clear();
	thresholds.clear();
	missed_columns.clear();
	not_required_columns.clear();
	metas.clear();
	numeric_metas.clear();
	filtered_metas.clear();
	ratio_metas.clear();
	cross_table_metas.clear();
	plots.clear();
	dependencies.clear();
}

void File::set_full_path(const char* path, char file_sep) throw (FileException) {
	if (path == NULL) {
		throw FileException(1, full_path, "set_full_path( ... )", "path", "NULL");
	}

	if (strlen(path) <= 0) {
		throw FileException(2, full_path, "set_full_path( ... )", "path", NULL);
	}

	free(full_path);
	full_path = NULL;
	full_path = (char*)malloc((strlen(path) + 1) * sizeof(char));
	if (full_path == NULL) {
		throw FileException(0, NULL, "set_full_path( ... )", "path", path);
	}
	strcpy(full_path, path);

	free(file_name);
	file_name = NULL;
	file_name = get_file_name(full_path, file_sep);

	free(file_path);
	file_path = NULL;
	file_path = get_file_path(full_path, file_sep);
}

void File::set_abbreviation(const char* abbreviation) throw (FileException) {
	if (abbreviation == NULL) {
		throw FileException(1, file_name, "set_abbreviation(const char* abbreviation)", "abbreviation", "NULL");
	}

	if (strlen(abbreviation) <= 0) {
		throw FileException(2, file_name, "set_abbreviation(const char* abbreviation)", "abbreviation", NULL);
	}

	free(this->abbreviation);
	this->abbreviation = NULL;
	this->abbreviation = (char*)malloc((strlen(abbreviation) + 1) * sizeof(char));
	if (this->abbreviation == NULL) {
		throw FileException(0, file_name, "set_abbreviation(const char* abbreviation)", "abbreviation", abbreviation);
	}
	strcpy(this->abbreviation, abbreviation);
}

void File::set_abbreviation(const char* abbreviation, unsigned int length) throw (FileException) {
	if (abbreviation == NULL) {
		throw FileException(1, file_name, "set_abbreviation(const char* abbreviation, int length)", "abbreviation", "NULL");
	}

	if (strlen(abbreviation) <= 0) {
		throw FileException(2, file_name, "set_abbreviation(const char* abbreviation, int length)", "abbreviation", NULL);
	}

	if ((length <= 0) || (length > strlen(abbreviation))) {
		length = strlen(abbreviation);
	}

	free(this->abbreviation);
	this->abbreviation = NULL;
	this->abbreviation = (char*)malloc((length + 1) * sizeof(char));
	if (this->abbreviation == NULL) {
		throw FileException(0, file_name, "set_abbreviation(const char* abbreviation, int length)", "abbreviation", abbreviation);
	}
	strncpy(this->abbreviation, abbreviation, length);
	this->abbreviation[length] = '\0';
}

const char* File::get_file_name() {
	return file_name;
}

const char* File::get_file_path() {
	return file_path;
}

const char* File::get_file_extension() {
	if (file_name != NULL) {
		return strrchr(file_name, '.');
	}

	return NULL;
}

const char* File::get_full_path() {
	return full_path;
}

const char* File::get_abbreviation() {
	return abbreviation;
}

char* File::transform_file_abbreviation(const char* prefix, const char* extension) throw (FileException) {
	char* transformed_file_abbreviation = NULL;

	if ((abbreviation != NULL) && (strlen(abbreviation) > 0)) {
		transformed_file_abbreviation = (char*)malloc((
				(prefix != NULL ? strlen(prefix) : 0) + strlen(abbreviation) +
				(extension != NULL ? strlen(extension) : 0) + 1) * sizeof(char));

		if (transformed_file_abbreviation == NULL) {
			throw FileException(0, NULL, "transform_file_abbreviation( ... )", "abbreviation", abbreviation);
		}

		transformed_file_abbreviation[0] = '\0';

		if (prefix != NULL) {
			strcat(transformed_file_abbreviation, prefix);
		}

		strcat(transformed_file_abbreviation, abbreviation);

		if (extension != NULL) {
			strcat(transformed_file_abbreviation, extension);
		}
	}

	return transformed_file_abbreviation;
}

char* File::transform_output_name(const char* prefix, const char* extension) throw (FileException) {
	char* transformed_output_name = NULL;

	if ((output_name != NULL) && (strlen(output_name) > 0)) {
		transformed_output_name = (char*)malloc((
				(prefix != NULL ? strlen(prefix) : 0) + strlen(output_name) +
				(extension != NULL ? strlen(extension) : 0) + 1) * sizeof(char));
		if (transformed_output_name == NULL) {
			throw FileException(0, NULL, "transform_file_exception( ... )", "output_name", output_name);
		}

		transformed_output_name[0] = '\0';

		if (prefix != NULL) {
			strcat(transformed_output_name, prefix);
		}

		strcat(transformed_output_name, output_name);

		if (extension != NULL) {
			strcat(transformed_output_name, extension);
		}
	}

	return transformed_output_name;
}

char* File::transform_file_name(const char* prefix, const char* extension, bool preserve_old_extension) throw (FileException) {
	char* transformed_file_name = NULL;
	int old_extension_start = -1;

	if (file_name != NULL) {
		if (!preserve_old_extension) {
			for (int i = strlen(file_name) - 1; i >= 0; i--) {
				if (file_name[i] == '.') {
					old_extension_start = i;
					break;
				}
			}

			if (old_extension_start < 0) {
				preserve_old_extension = true;
			}
		}

		transformed_file_name = (char*)malloc((
				(prefix != NULL ? strlen(prefix) : 0) +
				(preserve_old_extension ? strlen(file_name) : old_extension_start) +
				(extension != NULL ? strlen(extension) : 0) + 1) * sizeof(char));

		if (transformed_file_name == NULL) {
			throw FileException(0, NULL, "transform_file_name( ... )", "file_name", file_name);
		}

		transformed_file_name[0] = '\0';

		if (prefix != NULL) {
			strcat(transformed_file_name, prefix);
		}

		if (preserve_old_extension) {
			strcat(transformed_file_name, file_name);
		}
		else {
			strncat(transformed_file_name, file_name, old_extension_start);
		}

		if (extension != NULL) {
			strcat(transformed_file_name, extension);
		}
	}

	return transformed_file_name;
}

void File::add_column(const char* common_name, const char* actual_name) throw (FileException) {
	try {
		add(columns, common_name, actual_name);
	} catch (FileException &e) {
		FileException* new_e = new FileException(3, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

const char* File::get_column(const char* common_name) throw (FileException) {
	try {
		return get(columns, common_name);
	} catch (FileException &e) {
		FileException* new_e = new FileException(4, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

const char* File::get_common_column(const char* actual_name) {
	if ((actual_name != NULL) && (strlen(actual_name) > 0)) {
		if (case_sensitive) {
			for (map_char_it = columns.begin(); map_char_it != columns.end(); map_char_it++) {
				if (strcmp(map_char_it->second, actual_name) == 0) {
					return map_char_it->first;
				}
			}
		}
		else {
			for (map_char_it = columns.begin(); map_char_it != columns.end(); map_char_it++) {
				if (sstrcmp(map_char_it->second, actual_name) == 0) {
					return map_char_it->first;
				}
			}
		}
	}

	return NULL;
}

bool File::remove_column(const char* common_name) throw (FileException) {
	try {
		return remove(columns, common_name);
	} catch (FileException &e) {
		FileException* new_e = new FileException(5, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

void File::add_property(const char* name, const char* value) throw (FileException) {
	try {
		add(properties, name, value);
	} catch (FileException &e) {
		FileException* new_e = new FileException(6, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

const char* File::get_property(const char* value) throw (FileException) {
	try {
		return get(properties, value);
	} catch (FileException &e) {
		FileException* new_e = new FileException(7, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

bool File::remove_property(const char* name) throw (FileException) {
	try {
		return remove(properties, name);
	} catch (FileException &e) {
		FileException* new_e = new FileException(8, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

void File::add_threshold(const char* name, double value) throw (FileException) {
	try {
		add(thresholds, name, value);
	} catch (FileException &e) {
		FileException* new_e = new FileException(9, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

vector<double>* File::get_threshold(const char* name) throw (FileException) {
	try {
		return get(thresholds, name);
	} catch (FileException &e) {
		FileException* new_e = new FileException(10, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

bool File::remove_threshold(const char* name) throw (FileException) {
	try {
		return remove(thresholds, name);
	} catch (FileException &e) {
		FileException* new_e = new FileException(11, NULL, NULL, NULL, NULL);
		new_e->set_source(e);
		throw *new_e;
	}
}

int File::get_columns_number() {
	return columns.size();
}

int File::get_properties_number() {
	return properties.size();
}

int File::get_thresholds_number() {
	return thresholds.size();
}

vector<Meta*>* File::get_metas() {
	return &metas;
}

vector<MetaNumeric*>* File::get_numeric_metas() {
	return &numeric_metas;
}

vector<MetaFiltered*>* File::get_filtered_metas() {
	return &filtered_metas;
}

vector<MetaRatio*>* File::get_ratio_metas() {
	return &ratio_metas;
}

vector<MetaCrossTable*>* File::get_cross_table_metas() {
	return &cross_table_metas;
}

vector<Plottable*>* File::get_plots() {
	return &plots;
}

vector<char*>* File::get_missed_columns() {
	return &missed_columns;
}

vector<char*>* File::get_not_required_columns() {
	return &not_required_columns;
}

void File::set_header_separator(char separator) {
	header_separator = separator;
}

char File::get_header_separator() {
	return header_separator;
}

void File::set_data_separator(char separator) {
	data_separator = separator;
}

char File::get_data_separator() {
	return data_separator;
}

void File::set_output_prefix(const char* prefix) throw (FileException) {
	if (prefix == NULL) {
		throw FileException(1, file_name, "set_output_prefix( ... )", "prefix", "NULL");
	}

	if (strlen(prefix) <= 0) {
		throw FileException(2, file_name, "set_output_prefix( ... )", "prefix", NULL);
	}

	free(output_prefix);
	output_prefix = NULL;

	try {
		output_prefix = get_file_name(prefix, file_separator);
	}
	catch (FileException &e) {
		FileException* new_e = new FileException(0, file_name, "set_output_prefix( ... )", "output_prefix", prefix);
		new_e->set_source(e);
		throw *new_e;
	}
}

void File::set_output_name(const char* name) throw (FileException) {
	if (name == NULL) {
		throw FileException(1, file_name, "set_output_name( ... )", "name", "NULL");
	}

	if (strlen(name) <= 0) {
		throw FileException(2, file_name, "set_output_name( ... )", "name", NULL);
	}

	free(output_name);
	output_name = NULL;
	output_name = (char*)malloc((strlen(name) + 1) * sizeof(char));
	if (output_name == NULL) {
		throw FileException(0, file_name, "set_output_name( ... )", "output_name", name);
	}
	strcpy(output_name, name);
}

const char* File::get_output_prefix() {
	return output_prefix;
}

const char* File::get_output_name() {
	return output_name;
}

void File::set_case_sensitive(bool sensitive) {
	case_sensitive = sensitive;
}

bool File::is_case_sensitive() {
	return case_sensitive;
}

vector< pair<pair<Meta*, Meta*>, int> >* File::get_dependencies() {
	return &dependencies;
}

bool File::compare (const char* first, const char* second) {
	return strcmp(first, second) < 0;
}

void File::add(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key, double value) throw (FileException) {
	if (key == NULL) {
		throw FileException(1, file_name, "add( ... , const char* key, double value)", "key", "NULL");
	}

	if (strlen(key) <= 0) {
		throw FileException(2, file_name, "add( ... , const char* key, double value)", "key", NULL);
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
			throw FileException(0, file_name, "add( ... , const char* key, double value)", "key_copy", "NULL");
		}
		strcpy(key_copy, key);

		values = new vector<double>();

		values->push_back(value);
		map.insert(pair<const char* const, vector<double>*>(key_copy, values));
	}
}

void File::add(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key, const char* value) throw (FileException) {
	if (key == NULL) {
		throw FileException(1, file_name, "add( ... , const char* key, const char* value)", "key", "NULL");
	}

	if (strlen(key) <= 0) {
		throw FileException(2, file_name, "add( ... , const char* key, const char* value)", "key", NULL);
	}

	if (value == NULL) {
		throw FileException(1, file_name, "add( ... , const char* key, const char* value)", "value", "NULL");
	}

	if (strlen(value) <= 0) {
		throw FileException(2, file_name, "add( ... , const char* key, const char* value)", "value", NULL);
	}

	char* value_copy = NULL;

	value_copy = (char*)malloc((strlen(value) + 1) * sizeof(char));
	if (value_copy == NULL) {
		throw FileException(0, file_name, "add( ... , const char* key, const char* value)", "value_copy", "NULL");
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
			throw FileException(0, file_name, "add( ... , const char* key, const char* value)", "key_copy", "NULL");
		}
		strcpy(key_copy, key);

		map.insert(pair<const char* const, char*>(key_copy, value_copy));
	}
}

vector<double>* File::get(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException) {
	if (key == NULL) {
		throw FileException(1, file_name, "vector<double>* get( ... )", "key", "NULL");
	}

	if (strlen(key) <= 0) {
		throw FileException(2, file_name, "vector<double>* get( ... )", "key", NULL);
	}

	map_vect_it = map.find(key);
	if (map_vect_it != map.end()) {
		return map_vect_it->second;
	}

	return NULL;
}

const char* File::get(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException) {
	if (key == NULL) {
		throw FileException(1, file_name, "const char* get( ... )", "key", "NULL");
	}

	if (strlen(key) <= 0) {
		throw FileException(2, file_name, "const char* get( ... )", "key", NULL);
	}

	map_char_it = map.find(key);
	if (map_char_it != map.end()) {
		return map_char_it->second;
	}

	return NULL;
}

bool File::remove(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException) {
	if (key == NULL) {
		throw FileException(1, file_name, "remove(map<const char* const, vector<double>*> , ... )", "key", "NULL");
	}

	if (strlen(key) <= 0) {
		throw FileException(2, file_name, "remove(map<const char* const, vector<double>*> , ... )", "key", NULL);
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

bool File::remove(map<const char* const, char*, bool(*)(const char*, const char*)>& map, const char* key) throw (FileException) {
	if (key == NULL) {
		throw FileException(1, file_name, "remove(map<const char* const, char*> , ... )", "key", "NULL");
	}

	if (strlen(key) <= 0) {
		throw FileException(2, file_name, "remove(map<const char* const, char*> , ... )", "key", NULL);
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

void File::copy(map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_to, map<const char* const, vector<double>*, bool(*)(const char*, const char*)>& map_from) throw (FileException) {
	char* key_copy = NULL;
	vector<double>* values = NULL;

	for (map_vect_it = map_from.begin(); map_vect_it != map_from.end(); map_vect_it++) {
		key_copy = (char*)malloc((strlen(map_vect_it->first) + 1) * sizeof(char));
		if (key_copy == NULL) {
			throw FileException(0, file_name, "copy(map<const char* const, vector<double>*> , ... )", "key_copy", map_vect_it->first);
		}
		strcpy(key_copy, map_vect_it->first);

		values = new vector<double>();
		vector_it = map_vect_it->second->begin();
		while (vector_it != map_vect_it->second->end()) {
			values->push_back(*vector_it);
			vector_it++;
		}

		map_to.insert(pair<const char* const, vector<double>*>(key_copy, values));
	}
}

void File::copy(map<const char* const, char*, bool(*)(const char*, const char*)>& map_to, map<const char* const, char*, bool(*)(const char*, const char*)>& map_from) throw (FileException) {
	char* key_copy = NULL;
	char* value_copy = NULL;

	for (map_char_it = map_from.begin(); map_char_it != map_from.end(); map_char_it++) {
		key_copy = (char*)malloc((strlen(map_char_it->first) + 1) * sizeof(char));
		if (key_copy == NULL) {
			throw FileException(0, file_name, "copy(map<const char* const, char*> , ... )", "key_copy", map_char_it->first);
		}

		value_copy = (char*)malloc((strlen(map_char_it->second) + 1) * sizeof(char));
		if (value_copy == NULL) {
			free(key_copy);
			throw FileException(0, file_name, "copy(map<const char* const, char*> , ... )", "value_copy", map_char_it->second);
		}

		strcpy(key_copy, map_char_it->first);
		strcpy(value_copy, map_char_it->second);

		map_to.insert(pair<const char* const, char*>(key_copy, value_copy));
	}
}

int File::sstrcmp(const char* first, const char* second) {
	int i = 0;

	while ((first[i] != '\0') && (second[i] != '\0')) {
		if (tolower(first[i]) < tolower(second[i])) {
			return -1;
		}
		else if (tolower(first[i]) > tolower(second[i])) {
			return 1;
		}
		i++;
	}

	if (first[i] == '\0') {
		if (second[i] == '\0') {
			return 0;
		}
		return -1;
	}
	return 1;
}

char* File::get_file_name(const char* path, char file_sep) throw (FileException) {
	int file_name_start = 0, file_name_length = 0, current_position = 0;;
	char* file_name = NULL;

	if (path == NULL) {
		throw FileException(1, NULL, "get_file_name(cons char* path, char file_sep)", "path", "NULL");
	}

	if (strlen(path) <= 0) {
		throw FileException(2, NULL, "get_file_name(cons char* path, char file_sep)", "path", NULL);
	}

	for (int i = strlen(path) - 1; i >= 0; i--) {
		if (path[i] == file_sep) {
			file_name_start = i + 1;
			break;
		}

		file_name_length += 1;
	}

	if (file_name_length > 0) {
		file_name = (char*)malloc((file_name_length + 1) * sizeof(char));
		if (file_name == NULL) {
			throw FileException(0, NULL, "get_file_name(cons char* path, char file_sep)", "path", path);
		}

		for (int i = file_name_start; i < file_name_start + file_name_length; i++) {
			file_name[current_position++] = path[i];
		}

		file_name[current_position] = '\0';
	}

	return file_name;
}

char* File::get_file_path(const char* path, char file_sep) throw (FileException) {
	int file_path_length = 0;
	char* file_path = NULL;

	if (path == NULL) {
		throw FileException(1, NULL, "get_file_path(cons char* path, char file_sep)", "path", "NULL");
	}

	if (strlen(path) <= 0) {
		throw FileException(2, NULL, "get_file_path(cons char* path, char file_sep)", "path", NULL);
	}

	for (int i = strlen(path) - 1; i >= 0; i--) {
		if (path[i] == file_sep) {
			file_path_length = i + 1;
			break;
		}
	}

	if (file_path_length > 0) {
		file_path = (char*)malloc((file_path_length + 1) * sizeof(char));
		if (file_path == NULL) {
			throw FileException(0, NULL, "get_file_path(cons char* path, char file_sep)", "path", path);
		}
		strncpy(file_path, path, file_path_length);
		file_path[file_path_length] = '\0';
	}

	return file_path;
}

void File::set_estimated_lines_count(int estimated_lines_count) {
	this->estimated_lines_count = estimated_lines_count;
}

int File::get_estimated_lines_count() {
	return estimated_lines_count;
}

void File::set_lambda(double lambda) {
	this->lambda = lambda;
}

double File::get_lambda() {
	return lambda;
}

int File::get_memory_usage() {
	vector<Meta*>::iterator meta_it;
	vector<MetaFiltered*>::iterator filtered_metas_it;
	vector<MetaRatio*>::iterator ratio_metas_it;
	vector<MetaCrossTable*>::iterator cross_table_meta_it;

	int memory = 0;

	for (meta_it = metas.begin(); meta_it != metas.end(); meta_it++) {
		if ((*meta_it) != NULL) {
			memory += (*meta_it)->get_memory_usage();
		}
	}

	for (filtered_metas_it = filtered_metas.begin(); filtered_metas_it != filtered_metas.end(); filtered_metas_it++) {
		memory += (*filtered_metas_it)->get_memory_usage();
	}

	for (ratio_metas_it = ratio_metas.begin(); ratio_metas_it != ratio_metas.end(); ratio_metas_it++) {
		memory += (*ratio_metas_it)->get_memory_usage();
	}

	for (cross_table_meta_it = cross_table_metas.begin(); cross_table_meta_it != cross_table_metas.end(); cross_table_meta_it++) {
		memory += (*cross_table_meta_it)->get_memory_usage();
	}

	return memory;
}

