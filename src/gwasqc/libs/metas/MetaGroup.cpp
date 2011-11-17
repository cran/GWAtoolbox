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

#include "../../include/metas/MetaGroup.h"

MetaGroup::MetaGroup(): Meta(), na_value(false), new_value(NULL), groups(bool_strcmp) {
}

MetaGroup::~MetaGroup() {
	for (groups_it = groups.begin(); groups_it != groups.end(); groups_it++) {
		free(groups_it->first);
	}

	groups.clear();
}

void MetaGroup::put(char* value) throw (MetaException) {
	if (strcmp(na_marker, value) == 0) {
		na_value = true;
		return;
	}
	else {
		na_value = false;
	}

	groups_it = groups.find(value);

	if (groups_it != groups.end()) {
		groups_it->second += 1;
	}
	else {
		new_value = (char*)malloc((strlen(value) + 1) * sizeof(char));
		if (new_value == NULL) {
			throw MetaException("MetaGroup", "put( char* )", __LINE__, 2, (strlen(value) + 1) * sizeof(char));
		}
		strcpy(new_value, value);

		groups.insert(pair<char*, int>(new_value, 1));
	}
}

void MetaGroup::finalize() throw (MetaException) {

}

bool MetaGroup::is_na() {
	return na_value;
}

void MetaGroup::print(ostream& stream) {
	if (!groups.empty()) {
		map<int, int> numeric_groups;
		map<int, int>::iterator numeric_groups_it;
		vector< pair<const char*, int> > non_numeric_groups;
		vector< pair<const char*, int> >::iterator non_numeric_groups_it;
		int numeric_value = 0;
		char* end_ptr = NULL;

		unsigned int max_length = 0;

		for (groups_it = groups.begin(); groups_it != groups.end(); groups_it++) {
			if (max_length < strlen(groups_it->first)) {
				max_length = strlen(groups_it->first);
			}

			numeric_value = strtol(groups_it->first, &end_ptr, 10);
			if (*end_ptr == '\0') {
				numeric_groups.insert(pair<int, int>(numeric_value, groups_it->second));
			}
			else {
				non_numeric_groups.push_back(pair<const char*, int>(groups_it->first, groups_it->second));
			}
		}
		max_length += 1;

		stream << resetiosflags(stream.flags());
		stream << " |" << setfill('-') << setw(max_length) << "-" << "|" << setw(12) << "-" << "|" << endl;
		stream << setfill(' ');
		for (numeric_groups_it = numeric_groups.begin(); numeric_groups_it != numeric_groups.end(); numeric_groups_it++) {
			stream  << " |" << setw(max_length) << numeric_groups_it->first << "|" << setw(12) << numeric_groups_it->second << "|" << endl;
		}
		for (non_numeric_groups_it = non_numeric_groups.begin(); non_numeric_groups_it != non_numeric_groups.end(); non_numeric_groups_it++) {
			stream  << " |" << setw(max_length) << non_numeric_groups_it->first << "|" << setw(12) << non_numeric_groups_it->second << "|" << endl;
		}
		stream << " |" << setfill('-') << setw(max_length) << "-" << "|" << setw(12) << "-" << "|" << endl;

		numeric_groups.clear();
		non_numeric_groups.clear();
	}
}

void MetaGroup::print_html(ostream& stream, char path_separator) {
	if (!groups.empty()) {
		map<int, int> numeric_groups;
		map<int, int>::iterator numeric_groups_it;
		vector< pair<const char*, int> > non_numeric_groups;
		vector< pair<const char*, int> >::iterator non_numeric_groups_it;
		int numeric_value = 0;
		char* end_ptr = NULL;

		for (groups_it = groups.begin(); groups_it != groups.end(); groups_it++) {
			numeric_value = strtol(groups_it->first, &end_ptr, 10);

			if (*end_ptr == '\0') {
				numeric_groups.insert(pair<int, int>(numeric_value, groups_it->second));
			}
			else {
				non_numeric_groups.push_back(pair<const char*, int>(groups_it->first, groups_it->second));
			}
		}

		stream << "<div class = \"groups\">";
		stream << "<table class=\"groups\">";
		stream << "<tr><th>Name</th><th># Of Occurrences</th></tr>";

		for (numeric_groups_it = numeric_groups.begin(); numeric_groups_it != numeric_groups.end(); numeric_groups_it++) {
			stream << "<tr>";
			stream << "<td>" << numeric_groups_it->first << "</td>";
			stream << "<td>" << numeric_groups_it->second << "</td>";
			stream << "</tr>";
		}
		for (non_numeric_groups_it = non_numeric_groups.begin(); non_numeric_groups_it != non_numeric_groups.end(); non_numeric_groups_it++) {
			stream << "<tr>";
			stream << "<td>" << non_numeric_groups_it->first << "</td>";
			stream << "<td>" << non_numeric_groups_it->second << "</td>";
			stream << "</tr>";
		}
		stream << "</table>";
		stream << "</div>";

		numeric_groups.clear();
		non_numeric_groups.clear();
	}
}

double MetaGroup::get_memory_usage() {
	unsigned long int memory = 0;

	for (groups_it = groups.begin(); groups_it != groups.end(); groups_it++) {
		memory += (strlen(groups_it->first) + 1) * sizeof(char);
		memory += sizeof(int);
		memory += sizeof(char*);
	}

	return memory / 1048576.0;
}
