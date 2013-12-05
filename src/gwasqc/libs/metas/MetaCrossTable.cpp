/*
 * Copyright � 2011 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
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

#include "../../include/metas/MetaCrossTable.h"

MetaCrossTable::MetaCrossTable(MetaNumeric* x_meta, MetaNumeric* y_meta) : Meta(),
	x_meta(x_meta), y_meta(y_meta), xy_na(0), x_na_total(0), y_na_total(0), xy_total(0), row(NULL),
	new_x_value(0), new_y_value(0) {

}

MetaCrossTable::~MetaCrossTable() {
	for (column_it = table.begin(); column_it != table.end(); column_it++) {
		column_it->second->clear();
		delete column_it->second;
	}

	table.clear();
	x_na.clear();
	y_na.clear();
	x_total.clear();
	y_total.clear();

	row = NULL;
}

void MetaCrossTable::put(char* value) throw (MetaException) {
	if ((!x_meta->is_numeric()) || (!y_meta->is_numeric())) {
		return;
	}

	new_x_value = x_meta->get_value();
	new_y_value = y_meta->get_value();

	if (ISNAN(new_x_value) || ISNAN(new_y_value)) {
		return;
	}

	xy_total += 1;

	if (x_meta->is_na()) {
		x_na_total += 1;

		if (y_meta->is_na()) {
			y_na_total += 1;
			xy_na += 1;
		}
		else {
			row_it = y_total.find(new_y_value);
			if (row_it != y_total.end()) {
				row_it->second += 1;
			}
			else {
				y_total.insert(pair<double, int>(new_y_value, 1));

				for (column_it = table.begin(); column_it != table.end(); column_it++) {
					column_it->second->insert(pair<double, int>(new_y_value, 0));
				}
				x_na.insert(pair<double, int>(new_y_value, 0));
			}

			x_na.find(new_y_value)->second += 1;
		}
	}
	else if (y_meta->is_na()) {
		y_na_total += 1;

		row_it = x_total.find(new_x_value);
		if (row_it != x_total.end()) {
			row_it->second += 1;
		}
		else {
			x_total.insert(pair<double, int>(new_x_value, 1));

			row = new map<double, int>();
			for (row_it = y_total.begin(); row_it != y_total.end(); row_it++) {
				row->insert(pair<double, int>(row_it->first, 0));
			}
			y_na.insert(pair<double, int>(new_x_value, 0));

			table.insert(pair<double, map<double, int>* >(new_x_value, row));
		}

		y_na.find(new_x_value)->second += 1;
	}
	else {
		row_it = x_total.find(new_x_value);
		if (row_it != x_total.end()) {
			row_it->second += 1;
		}
		else {
			x_total.insert(pair<double, int>(new_x_value, 1));

			row = new map<double, int>();
			for (row_it = y_total.begin(); row_it != y_total.end(); row_it++) {
				row->insert(pair<double, int>(row_it->first, 0));
			}
			y_na.insert(pair<double, int>(new_x_value, 0));

			table.insert(pair<double, map<double, int>* >(new_x_value, row));
		}

		row_it = y_total.find(new_y_value);
		if (row_it != y_total.end()) {
			row_it->second += 1;
		}
		else {
			y_total.insert(pair<double, int>(new_y_value, 1));

			for (column_it = table.begin(); column_it != table.end(); column_it++) {
				column_it->second->insert(pair<double, int>(new_y_value, 0));
			}
			x_na.insert(pair<double, int>(new_y_value, 0));
		}

		table.find(new_x_value)->second->find(new_y_value)->second += 1;
	}
}

void MetaCrossTable::finalize() throw (MetaException) {

}

bool MetaCrossTable::is_na() {
	return false;
}

void MetaCrossTable::print(ostream& stream) {
	if ((x_meta->is_numeric()) && (y_meta->is_numeric())) {
		const char* x_name = x_meta->get_actual_name();
		const char* y_name = y_meta->get_actual_name();
		double x_value = 0;
		int width = strlen(x_name);

		if (width < 12) {
			width = 12;
		}

		stream << resetiosflags(stream.flags());
		stream << setfill(' ');

		stream << " " << setw(width) << " " << " | " << y_name << endl;
		stream << " " << setw(width) << x_name << " |";
		for (row_it = y_total.begin(); row_it != y_total.end(); row_it++) {
			stream << setw(12) << row_it->first << " |";
		}
		if (y_na_total > 0) {
			stream << setw(12) << "NA" << " |";
		}
		stream << setw(12) << "Row Total" << " |" << endl;

		stream << " ";
		stream << setfill('-');
		stream << setw(width + 2) << "|";
		for (unsigned int i = 0; i <= y_total.size(); i++) {
			stream << setw(14) << "|";
		}
		if (y_na_total > 0) {
			stream << setw(14) << "|";
		}
		stream << setfill(' ') << endl;

		for (column_it = table.begin(); column_it != table.end(); column_it++) {
			x_value = column_it->first;

			/* COUNT */
			stream << resetiosflags(ios_base::fixed);
			stream << " ";
			stream << setw(width) << x_value << " |";
			for (row_it = column_it->second->begin(); row_it != column_it->second->end(); row_it++) {
				stream << setw(12) << row_it->second << " |";
			}
			if (y_na_total > 0) {
				stream << setw(12) << y_na.find(x_value)->second << " |";
			}
			stream << setw(12) << x_total.find(x_value)->second << " |" << endl;

			/* ROW DELIMITER */
			stream << " ";
			stream << setfill('-');
			stream << setw(width + 2) << "|";
			for (unsigned int i = 0; i <= y_total.size(); i++) {
				stream << setw(14) << "|";
			}
			if (y_na_total > 0) {
				stream << setw(14) << "|";
			}
			stream << setfill(' ') << endl;
		}

		if (x_na_total > 0) {
			/* COUNT */
			stream << " ";
			stream << setw(width) << "NA" << " |";
			for (row_it = x_na.begin(); row_it != x_na.end(); row_it++) {
				stream << setw(12) << row_it->second << " |";
			}
			if (y_na_total > 0) {
				stream << setw(12) << xy_na << " |";
			}
			stream << setw(12) << x_na_total << " |" << endl;

			/* ROW DELIMITER */
			stream << " ";
			stream << setfill('-');
			stream << setw(width + 2) << "|";
			for (unsigned int i = 0; i <= y_total.size(); i++) {
				stream << setw(14) << "|";
			}
			if (y_na_total > 0) {
				stream << setw(14) << "|";
			}
			stream << setfill(' ') << endl;
		}

		/* COUNT */
		stream << " ";
		stream << setw(width) << "Column Total" << " |";
		for (row_it = y_total.begin(); row_it != y_total.end(); row_it++) {
			stream << setw(12) << row_it->second << " |";
		}
		if (y_na_total > 0) {
			stream << setw(12) << y_na_total << " |";
		}
		stream << setw(12) << xy_total << " |" << endl;


		/* ROW DELIMITER */
		stream << " ";
		stream << setfill('-');
		stream << setw(width + 2) << "|";
		for (unsigned int i = 0; i <= y_total.size(); i++) {
			stream << setw(14) << "|";
		}
		if (y_na_total > 0) {
			stream << setw(14) << "|";
		}
		stream << setfill(' ') << endl << endl;

		stream << resetiosflags(stream.flags());
		stream << setfill(' ');
	}
	else {
		if (!x_meta->is_numeric()) {
			stream << resetiosflags(stream.flags());
			stream << setfill(' ');
			stream << " '" << x_meta->get_actual_name() << "' : non-numeric values are present or all values are missing" << endl;
		}

		if (!y_meta->is_numeric()) {
			stream << resetiosflags(stream.flags());
			stream << setfill(' ');
			stream << " '" << y_meta->get_actual_name() << "' : non-numeric values are present or all values are missing" << endl;
		}
	}
}

void MetaCrossTable::print_html(ostream& stream, char path_separator) {
	if ((x_meta->is_numeric()) && (y_meta->is_numeric())) {
		const char* x_name = x_meta->get_actual_name();
		const char* y_name = y_meta->get_actual_name();
		double x_value = 0;
		int colspan = 0;
		int width = 0;

		width = 100 / (y_total.size() + (y_na_total > 0 ? 1 : 0) + 2);

		stream << "<div class = \"groups\">";
		stream << "<table class = \"cross\">";

		stream << "<tr>";

		stream << "<th class = \"header\">";
		stream << "</th>";
		stream << "<th class = \"header\">" << y_name << "</th>";
		for (row_it = y_total.begin(); row_it != y_total.end(); row_it++) {
			colspan += 1;
		}
		if (y_na_total > 0) {
			colspan += 1;
		}
		stream << "<th class = \"header\" colspan = \"" << colspan << "\"></th>";
		stream << "</tr>";

		stream << "<tr>";
		stream << "<th class = \"header\"  style = \"width: " << width << "%;\">" << x_name << "</th>";
		for (row_it = y_total.begin(); row_it != y_total.end(); row_it++) {
			stream << "<th class = \"header\"  style = \"width: " << width << "%;\">" << row_it->first << "</th>";
		}
		if (y_na_total > 0) {
			stream << "<th class = \"header\"  style = \"width: " << width << "%;\">NA</th>";
		}
		stream << "<th class = \"header\" style = \"width: " << width << "%;\">Row Total</th>";
		stream << "</tr>";

		for (column_it = table.begin(); column_it != table.end(); column_it++) {
			x_value = column_it->first;

			stream << "<tr>";
			stream << "<td>" << x_value << "</td>";
			for (row_it = column_it->second->begin(); row_it != column_it->second->end(); row_it++) {
				stream << "<td>" << row_it->second << "</td>";
			}
			if (y_na_total > 0) {
				stream << "<td>" << y_na.find(x_value)->second << "</td>";
			}
			stream << "<td>" << x_total.find(x_value)->second << "</td>";

			stream << "</tr>";
		}

		if (x_na_total > 0) {
			stream << "<tr>";
			stream << "<td>NA</td>";
			for (row_it = x_na.begin(); row_it != x_na.end(); row_it++) {
				stream << "<td>" << row_it->second << "</td>";
			}
			if (y_na_total > 0) {
				stream << "<td>" << xy_na << "</td>";
			}
			stream << "<td>" << x_na_total << "</td>";
			stream << "</tr>";
		}

		stream << "<tr>";
		stream << "<td class = \"footer\">Column Total</td>";
		for (row_it = y_total.begin(); row_it != y_total.end(); row_it++) {
			stream << "<td class = \"footer\">" << row_it->second << "</td>";
		}
		if (y_na_total > 0) {
			stream << "<td class = \"footer\">" << y_na_total << "</td>";
		}
		stream << "<td class = \"footer\">" << xy_total << "</td>";
		stream << "</tr>";

		stream << "</table>";
		stream << "</div>";
	}
	else {
		stream << "<div class = \"result\">";
		if (!x_meta->is_numeric()) {
			stream << " '" << x_meta->get_actual_name() << "' : non-numeric values are present or all values are missing";
		}

		if (!y_meta->is_numeric()) {
			stream << " '" << y_meta->get_actual_name() << "' : non-numeric values are present or all values are missing";
		}
		stream << "</div>";
	}
}

double MetaCrossTable::get_memory_usage() {
	unsigned long int memory = 0;

	for (column_it = table.begin(); column_it != table.end(); column_it++) {
		memory += (column_it->second->size() * (sizeof(double) + sizeof(int)));
		memory += sizeof(map<double, int>*);
		memory += sizeof(double);
	}
	memory += (x_na.size() * (sizeof(double) + sizeof(int)));
	memory += (y_na.size() * (sizeof(double) + sizeof(int)));
	memory += (x_total.size() * (sizeof(double) + sizeof(int)));
	memory += (y_total.size() * (sizeof(double) + sizeof(int)));

	return memory / 1048576.0;
}
