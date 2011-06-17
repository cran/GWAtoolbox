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

#include "../../include/metas/Meta.h"

const int Meta::HEAP_SIZE = 2800000;
const int Meta::HEAP_INCREMENT = 100000;

Meta::Meta():
	id(0), common_name(NULL), actual_name(NULL), full_name(NULL), description(NULL), na_marker(NULL)  {

}

Meta::~Meta() {
	free(common_name);
	free(actual_name);
	free(full_name);
	free(description);
	free(na_marker);

	common_name = NULL;
	actual_name = NULL;
	full_name = NULL;
	description = NULL;
	na_marker = NULL;
}

void Meta::set_id(int id) {
	this->id = id;
}

void Meta::set_common_name(const char* common_name) throw (MetaException) {
	if (common_name == NULL) {
		throw MetaException(1, "Meta", "set_common_name( ... )", "common_name", "NULL");
	}

	if (strlen(common_name) <= 0) {
		throw MetaException(2, "Meta", "set_common_name( ... )", "common_name", (const char*)NULL);
	}

	free(this->common_name);
	this->common_name = NULL;
	this->common_name = (char*)malloc((strlen(common_name) + 1) * sizeof(char));
	if (this->common_name == NULL) {
		throw MetaException(0, "Meta", "set_common_name( ... )", "common_name", common_name);
	}
	strcpy(this->common_name, common_name);
}

void Meta::set_actual_name(const char* actual_name) throw (MetaException) {
	if (actual_name == NULL) {
		throw MetaException(1, "Meta", "set_actual_name( ... )", "actual_name", "NULL");
	}

	if (strlen(actual_name) <= 0) {
		throw MetaException(2, "Meta", "set_actual_name( ... )", "actual_name", (const char*)NULL);
	}

	free(this->actual_name);
	this->actual_name = NULL;
	this->actual_name = (char*)malloc((strlen(actual_name) + 1) * sizeof(char));
	if (this->actual_name == NULL) {
		throw MetaException(0, "Meta", "set_actual_name( ... )", "actual_name", actual_name);
	}
	strcpy(this->actual_name, actual_name);
}

void Meta::set_full_name(const char* full_name) throw (MetaException) {
	if (full_name == NULL) {
		throw MetaException(1, "Meta", "set_full_name( ... )", "full_name", "NULL");
	}

	if (strlen(full_name) <= 0) {
		throw MetaException(2, "Meta", "set_full_name( ... )", "full_name", (const char*)NULL);
	}

	free(this->full_name);
	this->full_name = NULL;
	this->full_name = (char*)malloc((strlen(full_name) + 1) * sizeof(char));
	if (this->full_name == NULL) {
		throw MetaException(0, "Meta", "set_full_name( ... )", "full_name", full_name);
	}
	strcpy(this->full_name, full_name);
}

void Meta::set_description(const char* description) throw (MetaException) {
	if (description == NULL) {
		throw MetaException(1, "Meta", "set_description( ... )", "description", "NULL");
	}

	if (strlen(description) <= 0) {
		throw MetaException(2, "Meta", "set_description( ... )", "description", (const char*)NULL);
	}

	free(this->description);
	this->description = NULL;
	this->description = (char*)malloc((strlen(description) + 1) * sizeof(char));
	if (this->description == NULL) {
		throw MetaException(0, "Meta", "set_description( ... )", "description", description);
	}
	strcpy(this->description, description);
}

void Meta::set_na_marker(const char* na_marker) throw (MetaException) {
	if (na_marker == NULL) {
		throw MetaException(1, "Meta", "set_na_marker( ... )", "na_marker", "NULL");
	}

	if (strlen(na_marker) <= 0) {
		throw MetaException(2, "Meta", "set_na_marker( ... )", "na_marker", (const char*)NULL);
	}

	free(this->na_marker);
	this->na_marker = NULL;
	this->na_marker = (char*)malloc((strlen(na_marker) + 1) * sizeof(char));
	if (this->na_marker == NULL) {
		throw MetaException(0, "Meta", "set_na_marker( ... )", "na_marker", na_marker);
	}
	strcpy(this->na_marker, na_marker);
}

int Meta::get_id() {
	return id;
}

const char* Meta::get_actual_name() {
	return actual_name;
}

const char* Meta::get_common_name() {
	return common_name;
}

const char* Meta::get_full_name() {
	return full_name;
}

const char* Meta::get_description() {
	return description;
}

const char* Meta::get_na_marker() {
	return na_marker;
}

int Meta::get_memory_usage() {
	return 0;
}
