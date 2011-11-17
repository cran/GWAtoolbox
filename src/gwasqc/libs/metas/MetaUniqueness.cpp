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

#include "../../include/metas/MetaUniqueness.h"

MetaUniqueness::MetaUniqueness(unsigned int heap_size) throw (MetaException) : Meta(),
	n(0), na_value(false), data(NULL), new_data(NULL), new_value(NULL),
	current_heap_size(heap_size) {

	if (heap_size > 0) {
		data = (char**)malloc(heap_size * sizeof(char*));
		if (data == NULL) {
			throw MetaException("MetaUniqueness", "MetaUniqueness( unsigned int )", __LINE__, 2, heap_size * sizeof(char*));
		}
	}
}

MetaUniqueness::~MetaUniqueness() {
	if (data != NULL) {
		for (int i = 0; i < n; i++) {
			free(data[i]);
		}
		free(data);
		data = NULL;
	}

	for (duplicates_it = duplicates.begin(); duplicates_it != duplicates.end(); duplicates_it++) {
		free(*duplicates_it);
	}

	duplicates.clear();
}

void MetaUniqueness::put(char* value) throw (MetaException) {
	if (strcmp(na_marker, value) == 0) {
		na_value = true;
		return;
	}
	else {
		na_value = false;
	}

	new_value = (char*)malloc((strlen(value) + 1) * sizeof(char));
	if (new_value == NULL) {
		throw MetaException("MetaUniqueness", "put( char* )", __LINE__, 2, (strlen(value) + 1) * sizeof(char));
	}
	strcpy(new_value, value);

	n += 1;

	if (n > current_heap_size) {
		current_heap_size += Meta::HEAP_INCREMENT;

		new_data = (char**)realloc(data, current_heap_size * sizeof(char*));
		if (new_data == NULL) {
			for (int i = 0; i < n; i++) {
				free(data[i]);
			}
			free(data);
			free(new_value);
			data = NULL;
			throw MetaException("MetaUniqueness", "put( char* )", __LINE__, 3, current_heap_size * sizeof(char*));
		}
		data = new_data;
	}

	data[n - 1] = new_value;
}

void MetaUniqueness::finalize() throw (MetaException) {
	if ((data != NULL) && (n > 0)) {
		char* prev_duplicate = NULL;
		char* new_string = NULL;

		qsort(data, n, sizeof(char*), qsort_strcmp);

		for (int i = 0; i < n - 1; i++) {
			if (strcmp(data[i], data[i + 1]) == 0) {
				if ((prev_duplicate == NULL) || (strcmp(prev_duplicate, data[i + 1]) != 0)) {
					new_string = (char*)malloc((strlen(data[i + 1]) + 1) * sizeof(char));
					if (new_string == NULL) {
						throw MetaException("MetaUniqueness", "finalize()", __LINE__, 2, (strlen(data[i + 1]) + 1) * sizeof(char));
					}
					strcpy(new_string, data[i + 1]);

					duplicates.push_back(new_string);

					prev_duplicate = new_string;
				}
			}

			free(data[i]);
		}

		free(data[n - 1]);
	}

	free(data);
	data = NULL;
}

bool MetaUniqueness::is_na() {
	return na_value;
}

void MetaUniqueness::print(ostream& stream) {
	stream << resetiosflags(stream.flags());
	stream << setfill(' ');

	if (duplicates.empty()) {
		stream << " None" << endl;
	}
	else {
		for (duplicates_it = duplicates.begin(); duplicates_it != duplicates.end(); duplicates_it++) {
			stream << " " << *duplicates_it << endl;
		}
	}
}

void MetaUniqueness::print_html(ostream& stream, char path_separator) {
	stream << resetiosflags(stream.flags());

	if (duplicates.empty()) {
		stream << "<div class = \"result\">";
		stream << "None";
		stream << "</div>";
	}
	else {
		stream << "<div class = \"groups\">";
		stream << "<table class = \"groups\">";
		stream << "<tr><th>Name</th></tr>";
		for (duplicates_it = duplicates.begin(); duplicates_it != duplicates.end(); duplicates_it++) {
			stream << "<tr><td>" << *duplicates_it << "</td></tr>";
		}
		stream << "</table>";
		stream << "</div>";
	}
}

double MetaUniqueness::get_memory_usage() {
	unsigned long int memory = 0;

	if (data != NULL) {
		for (int i = 0; i < n; i++) {
			memory += ((strlen(data[i]) + 1) * sizeof(char));
		}
	}

	memory += current_heap_size * sizeof(char*);

	return memory / 1048576.0;
}
