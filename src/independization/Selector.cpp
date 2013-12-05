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

#include "include/Selector.h"

const double Selector::EPSILON = 0.00000001;

Selector::Selector() : gwafile(NULL), reader(NULL), /*header_backup(NULL),*/
	total_columns(numeric_limits<int>::min()),
	marker_column_pos(numeric_limits<int>::min()),
	chr_column_pos(numeric_limits<int>::min()),
	pvalue_column_pos(numeric_limits<int>::min()),
	markers_by_chr(auxiliary::bool_strcmp) {

}

Selector::~Selector() {
	if (reader != NULL) {
		reader->close();

		delete reader;
		reader = NULL;
	}

//	if (header_backup != NULL) {
//		free(header_backup);
//		header_backup = NULL;
//	}

	markers_by_chr_it = markers_by_chr.begin();
	while (markers_by_chr_it != markers_by_chr.end()) {
		markers_by_chr_it->second->clear();
		delete markers_by_chr_it->second;
		free(markers_by_chr_it->first);
		markers_by_chr_it++;
	}
	markers_by_chr.clear();

	markers_ld_it = markers_ld.begin();
	while (markers_ld_it != markers_ld.end()) {
		markers_ld_it->second->clear();
		delete markers_ld_it->second;
		markers_ld_it++;
	}
	markers_ld.clear();

	all_marker_names_it = all_marker_names.begin();
	while (all_marker_names_it != all_marker_names.end()) {
		free(*all_marker_names_it);
		all_marker_names_it++;
	}
	all_marker_names.clear();

	gwafile = NULL;
}

void Selector::open_gwafile(GwaFile* gwafile) throw (SelectorException) {
	if (gwafile == NULL) {
		throw SelectorException("Selector", "open_gwafile( GwaFile* )", __LINE__, 0, "gwafile");
	}

	try {
		close_gwafile();

		this->gwafile = gwafile;

		reader = ReaderFactory::create(gwafile->get_descriptor()->get_full_path());
		reader->open();
	} catch (DescriptorException& e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "open_gwafile( GwaFile* )", __LINE__, 3, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (ReaderException& e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "open_gwafile( GwaFile* )", __LINE__, 3, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (SelectorException& e) {
		e.add_message("Selector", "open_gwafile( GwaFile* )", __LINE__, 3, gwafile->get_descriptor()->get_full_path());
		throw;
	}
}

void Selector::close_gwafile() throw (SelectorException) {
	try {
		if (reader != NULL) {
			reader->close();
			delete reader;
			reader = NULL;
		}
	} catch (ReaderException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "close_gwafile()", __LINE__, 4, (gwafile != NULL) ? gwafile->get_descriptor()->get_full_path() : "NULL");
		throw new_e;
	}

//	if (header_backup != NULL) {
//		free(header_backup);
//		header_backup = NULL;
//	}

	markers_by_chr_it = markers_by_chr.begin();
	while (markers_by_chr_it != markers_by_chr.end()) {
		markers_by_chr_it->second->clear();
		delete markers_by_chr_it->second;
		free(markers_by_chr_it->first);
		markers_by_chr_it++;
	}
	markers_by_chr.clear();

	markers_ld_it = markers_ld.begin();
	while (markers_ld_it != markers_ld.end()) {
		markers_ld_it->second->clear();
		delete markers_ld_it->second;
		markers_ld_it++;
	}
	markers_ld.clear();

	all_marker_names_it = all_marker_names.begin();
	while (all_marker_names_it != all_marker_names.end()) {
		free(*all_marker_names_it);
		all_marker_names_it++;
	}
	all_marker_names.clear();

	gwafile = NULL;
}

void Selector::process_header() throw (SelectorException) {
	Descriptor* descriptor = NULL;
	char header_separator = '\0';
	char* header = NULL;
	char* token = NULL;
	int column_position = 0;
	const char* column_name = NULL;

	if (gwafile == NULL) {
		return;
	}

	try {
		descriptor = gwafile->get_descriptor();
		header_separator = gwafile->get_header_separator();

		if (reader->read_line() <= 0) {
			throw SelectorException("Selector", "process_header()", __LINE__, 5, 1, gwafile->get_descriptor()->get_name());
		}

		header = *(reader->line);

//		header_backup = (char*)malloc((strlen(header) + 1u) * sizeof(char));
//		if (header_backup == NULL) {
//			throw SelectorException("Selector", "process_header()", __LINE__, 2, ((strlen(header) + 1u) * sizeof(char)));
//		}
//		strcpy(header_backup, header);

		total_columns = numeric_limits<int>::min();
		marker_column_pos = numeric_limits<int>::min();
		chr_column_pos = numeric_limits<int>::min();
		pvalue_column_pos = numeric_limits<int>::min();

		token = auxiliary::strtok(&header, header_separator);
		while (token != NULL) {
			column_name = descriptor->get_default_column(token, gwafile->is_case_sensitive());
			if (column_name != NULL) {
				if (strcmp(column_name, Descriptor::MARKER) == 0) {
					marker_column_pos = column_position;
				} else if (strcmp(column_name, Descriptor::CHR) == 0) {
					chr_column_pos = column_position;
				} else if (strcmp(column_name, Descriptor::PVALUE) == 0) {
					pvalue_column_pos = column_position;
				}
			}
			token = auxiliary::strtok(&header, header_separator);
			++column_position;
		}

		total_columns = column_position;

		if (marker_column_pos < 0) {
			throw SelectorException("Selector", "process_header()", __LINE__, 7, ((column_name = descriptor->get_column(Descriptor::MARKER)) != NULL) ? column_name : Descriptor::MARKER, gwafile->get_descriptor()->get_name());
		}

		if (chr_column_pos < 0) {
			throw SelectorException("Selector", "process_header()", __LINE__, 7, ((column_name = descriptor->get_column(Descriptor::CHR)) != NULL) ? column_name : Descriptor::CHR, gwafile->get_descriptor()->get_name());
		}

		if (pvalue_column_pos < 0) {
			throw SelectorException("Selector", "process_header()", __LINE__, 7, ((column_name = descriptor->get_column(Descriptor::PVALUE)) != NULL) ? column_name : Descriptor::PVALUE, gwafile->get_descriptor()->get_name());
		}
	} catch (ReaderException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_header()", __LINE__, 6, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (DescriptorException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_header()", __LINE__, 6, gwafile->get_descriptor()->get_name());
		throw new_e;
	}
}

void Selector::process_data() throw (SelectorException) {
	Descriptor* descriptor = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 2u;

	char data_separator = '\0';
	const char* missing_value = NULL;

	char* token = NULL;
	char* end_ptr = NULL;

	int column_position = 0;

	const char* column_name = NULL;

	char* marker_token = NULL;
	char* chr_token = NULL;
	char* pvalue_token = NULL;

	char* marker = NULL;
	char* chr = NULL;
	double pvalue = 0.0;

	if (gwafile == NULL) {
		return;
	}

	try {
		descriptor = gwafile->get_descriptor();
		data_separator = gwafile->get_data_separator();
		missing_value = descriptor->get_property(Descriptor::MISSING);

		while ((line_length = reader->read_line()) > 0) {
			line = *(reader->line);

			column_position = 0;
			marker_token = NULL;
			chr_token = NULL;
			pvalue_token = NULL;
			token = auxiliary::strtok(&line, data_separator);
			while (token != NULL) {
				if (column_position == marker_column_pos) {
					marker_token = token;
				} else if (column_position == chr_column_pos) {
					chr_token = token;
				} else if (column_position == pvalue_column_pos) {
					pvalue_token = token;
				}
				token = auxiliary::strtok(&line, data_separator);
				++column_position;
			}

			if (column_position < total_columns) {
				throw SelectorException("Selector", "process_data()", __LINE__, 8, line_number, descriptor->get_name(), column_position, total_columns);
			} else if (column_position > total_columns) {
				throw SelectorException("Selector", "process_data()", __LINE__, 9, line_number, descriptor->get_name(), column_position, total_columns);
			}

			if (strcmp(missing_value, pvalue_token) == 0) {
				++line_number;
				continue;
			}

			pvalue = strtod(pvalue_token, &end_ptr);
			if (*end_ptr != '\0') {
				throw SelectorException("Selector", "process_data()",  __LINE__, 10, pvalue_token, ((column_name = descriptor->get_column(Descriptor::PVALUE)) != NULL) ? column_name : Descriptor::PVALUE, line_number);
			}

			if (ISNAN(pvalue)) {
				++line_number;
				continue;
			}

			markers_by_chr_it = markers_by_chr.find(chr_token);
			if (markers_by_chr_it == markers_by_chr.end()) {
				markers = new map<const char*, double, bool(*)(const char*, const char*)>(auxiliary::bool_strcmp);

				chr = (char*)malloc((strlen(chr_token) + 1u) * sizeof(char));
				if (chr == NULL) {
					throw SelectorException("Selector", "process_data()",  __LINE__, 2, ((strlen(chr_token) + 1u) * sizeof(char)));
				}
				strcpy(chr, chr_token);

				markers_by_chr.insert(pair<char*, map<const char*, double, bool(*)(const char*, const char*)>*>(chr, markers));
			} else {
				markers = markers_by_chr_it->second;
			}

			markers_it = markers->find(marker_token);
			if (markers_it == markers->end()) {
				marker = (char*)malloc((strlen(marker_token) + 1u) * sizeof(char));
				if (marker == NULL) {
					throw SelectorException("Selector", "process_data()",  __LINE__, 2, ((strlen(marker_token) + 1u) * sizeof(char)));
				}
				strcpy(marker, marker_token);

				all_marker_names.push_back(marker);

				markers->insert(pair<char*, double>(marker, pvalue));
			} else {
				/* trhow error about duplicates */
				throw SelectorException("Selector", "process_data()",  __LINE__);
			}

			++line_number;
		}

		if (line_length == 0) {
			throw SelectorException("Selector", "process_data()", __LINE__, 11, line_number, descriptor->get_name());
		}
	} catch (DescriptorException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_data()", __LINE__, 12, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (ReaderException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_data()", __LINE__, 12, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (SelectorException &e) {
		e.add_message("Selector", "process_data()", __LINE__, 12, gwafile->get_descriptor()->get_name());
		throw;
	}
}

void Selector::open_ld_file(const char* file_path) throw (SelectorException) {
	if (file_path == NULL) {
		throw SelectorException("Selector", "open_ld_file( const char* )", __LINE__, 0, "file_name");
	}

	try {
		close_ld_file();

		ld_file_path = file_path;

		ld_reader = ReaderFactory::create(file_path);
		ld_reader->open();
	} catch (ReaderException& e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "open_ld_file( GwaFile* )", __LINE__, 3, file_path);
		throw new_e;
	} catch (SelectorException& e) {
		e.add_message("Selector", "open_ld_file( GwaFile* )", __LINE__, 3, file_path);
		throw;
	}
}

void Selector::close_ld_file() throw (SelectorException) {
	try {
		if (ld_reader != NULL) {
			ld_reader->close();
			delete ld_reader;
			ld_reader = NULL;

			ld_file_path = NULL;
		}
	} catch (ReaderException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "close_ld_file()", __LINE__, 4, (ld_file_path != NULL) ? ld_file_path : "NULL");
		throw new_e;
	}
}

void Selector::process_ld_header() throw (SelectorException) {
	Descriptor* descriptor = NULL;
	char header_separator = '\0';
	char data_separator = '\0';
	char* header = NULL;
	char* token = NULL;

	int column_position = 0;

	const char* ld_marker1_column = NULL;
	const char* ld_marker2_column = NULL;
	const char* ld_value_column = NULL;

	if ((gwafile == NULL) || (ld_file_path == NULL)) {
		return;
	}

	try {
		descriptor = gwafile->get_descriptor();
		header_separator = gwafile->get_ld_file_header_separator();

		ld_marker1_column = descriptor->get_property(Descriptor::LD_MARKER1);
		ld_marker2_column = descriptor->get_property(Descriptor::LD_MARKER2);
		ld_value_column = descriptor->get_property(Descriptor::LD_VALUE);

		if ((ld_marker1_column != NULL) && (ld_marker2_column != NULL) && (ld_value_column != NULL)) {
			if (ld_reader->read_line() <= 0) {
				throw SelectorException("Selector", "process_ld_header()", __LINE__, 5, 1, ld_file_path);
			}

			header = *(ld_reader->line);

			ld_total_columns = numeric_limits<int>::min();
			ld_marker1_column_pos = numeric_limits<int>::min();
			ld_marker2_column_pos = numeric_limits<int>::min();
			ld_value_column_pos = numeric_limits<int>::min();

			token = auxiliary::strtok(&header, header_separator);
			if (gwafile->is_case_sensitive()) {
				while (token != NULL) {
					if (strcmp(token, ld_marker1_column) == 0) {
						ld_marker1_column_pos = column_position;
					} else if (strcmp(token, ld_marker2_column) == 0) {
						ld_marker2_column_pos = column_position;
					} else if (strcmp(token, ld_value_column) == 0) {
						ld_value_column_pos = column_position;
					}
					token = auxiliary::strtok(&header, header_separator);
					++column_position;
				}
			} else {
				while (token != NULL) {
					if (auxiliary::strcmp_ignore_case(token, ld_marker1_column) == 0) {
						ld_marker1_column_pos = column_position;
					} else if (auxiliary::strcmp_ignore_case(token, ld_marker2_column) == 0) {
						ld_marker2_column_pos = column_position;
					} else if (auxiliary::strcmp_ignore_case(token, ld_value_column) == 0) {
						ld_value_column_pos = column_position;
					}
					token = auxiliary::strtok(&header, header_separator);
					++column_position;
				}
			}

			ld_total_columns = column_position;

			if (ld_marker1_column_pos < 0) {
				throw SelectorException("Selector", "process_ld_header()", __LINE__, 7, ld_marker1_column, ld_file_path);
			}

			if (ld_marker2_column_pos < 0) {
				throw SelectorException("Selector", "process_ld_header()", __LINE__, 7, ld_marker2_column, ld_file_path);
			}

			if (ld_value_column_pos < 0) {
				throw SelectorException("Selector", "process_ld_header()", __LINE__, 7, ld_value_column, ld_file_path);
			}
		} else {
			data_separator = gwafile->get_ld_file_data_separator();

			if (header_separator != data_separator) {
				// throw something!!!
				throw SelectorException("Selector", "process_ld_header()", __LINE__);
			}

			ld_total_columns = 3;
			ld_marker1_column_pos = 0;
			ld_marker2_column_pos = 1;
			ld_value_column_pos = 2;
		}
	} catch (ReaderException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_ld_header()", __LINE__, 6, ld_file_path);
		throw new_e;
	} catch (DescriptorException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_ld_header()", __LINE__, 6, ld_file_path);
		throw new_e;
	}
}

void Selector::process_ld_data() throw (SelectorException) {
	Descriptor* descriptor = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 1u;

	char data_separator = '\0';

	char* token = NULL;
	char* end_ptr = NULL;

	int column_position = 0;

	const char* column_name = NULL;

	char* ld_marker1_token = NULL;
	char* ld_marker2_token = NULL;
	char* ld_value_token = NULL;

	double ld_value = 0.0;
	double ld_threhold = 0.0;

	const char* ld_marker1 = NULL;
	const char* ld_marker2 = NULL;

	vector<const char*>* ld_marker1_neighbours = NULL;
	vector<const char*>* ld_marker2_neighbours = NULL;

	if ((gwafile == NULL) || (ld_file_path == NULL)) {
		return;
	}

	try {
		descriptor = gwafile->get_descriptor();
		data_separator = gwafile->get_ld_file_data_separator();
		ld_threhold = descriptor->get_threshold(Descriptor::LD)->front();

		if (!ld_reader->sof()) {
			line_number = 2u;
		}

		while ((line_length = ld_reader->read_line()) > 0) {
			line = *(ld_reader->line);

			column_position = 0;
			ld_marker1_token = NULL;
			ld_marker2_token = NULL;
			ld_value_token = NULL;
			token = auxiliary::strtok(&line, data_separator);
			while (token != NULL) {
				if (column_position == ld_marker1_column_pos) {
					ld_marker1_token = token;
				} else if (column_position == ld_marker2_column_pos) {
					ld_marker2_token = token;
				} else if (column_position == ld_value_column_pos) {
					ld_value_token = token;
				}
				token = auxiliary::strtok(&line, data_separator);
				++column_position;
			}

			if (column_position < ld_total_columns) {
				throw SelectorException("Selector", "process_ld_data()", __LINE__, 8, line_number, ld_file_path, column_position, ld_total_columns);
			}

			ld_value = strtod(ld_value_token, &end_ptr);
			if (*end_ptr != '\0') {
				throw SelectorException("Selector", "process_ld_data()",  __LINE__, 10, ld_value_token, ((column_name = descriptor->get_column(Descriptor::LD_VALUE)) != NULL) ? column_name : Descriptor::LD_VALUE, line_number);
			}

			if (ISNAN(ld_value)) {
				++line_number;
				continue;
			}

			if (auxiliary::fcmp(ld_value, ld_threhold, EPSILON) < 0) {
				++line_number;
				continue;
			}

			markers_it = markers->find(ld_marker1_token);
			if (markers_it == markers->end()) {
				++line_number;
				continue;
			}
			ld_marker1 = markers_it->first;

			markers_it = markers->find(ld_marker2_token);
			if (markers_it == markers->end()) {
				++line_number;
				continue;
			}
			ld_marker2 = markers_it->first;

			markers_ld_it = markers_ld.find(ld_marker1);
			if (markers_ld_it == markers_ld.end()) {
				ld_marker1_neighbours = new vector<const char*>();
				markers_ld.insert(pair<const char*, vector<const char*>*>(ld_marker1, ld_marker1_neighbours));
			} else {
				ld_marker1_neighbours = markers_ld_it->second;
			}

			markers_ld_it = markers_ld.find(ld_marker2);
			if (markers_ld_it == markers_ld.end()) {
				ld_marker2_neighbours = new vector<const char*>();
				markers_ld.insert(pair<const char*, vector<const char*>*>(ld_marker2, ld_marker2_neighbours));
			} else {
				ld_marker2_neighbours = markers_ld_it->second;
			}

			ld_marker1_neighbours->push_back(ld_marker2);
			ld_marker2_neighbours->push_back(ld_marker1);

			++line_number;
		}

		if (line_length == 0) {
			throw SelectorException("Selector", "process_ld_data()", __LINE__, 11, line_number, ld_file_path);
		}
	} catch (DescriptorException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_ld_data()", __LINE__, 12, ld_file_path);
		throw new_e;
	} catch (ReaderException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "process_ld_data()", __LINE__, 12, ld_file_path);
		throw new_e;
	} catch (SelectorException &e) {
		e.add_message("Selector", "process_ld_data()", __LINE__, 12, ld_file_path);
		throw;
	}
}

void Selector::index_ld(const char* file_path) throw (SelectorException) {
	try {
		open_ld_file(file_path);
		process_ld_header();
		process_ld_data();
		close_ld_file();
	} catch (SelectorException &e) {
		throw;
	}
}

void Selector::drop_correlated_markers() throw (SelectorException) {
	Descriptor* descriptor = NULL;

	const char* ld_file_name = NULL;
	const char* ld_file_path = NULL;

	map<const char*, double, bool(*)(const char*, const char*)>* independent_markers = NULL;
	map<const char*, double, bool(*)(const char*, const char*)>::iterator independent_markers_it;

	map<const char*, double, bool(*)(const char*, const char*)>::iterator min_pvalue_it;

	vector<const char*>* marker_neighbours = NULL;
	vector<const char*>::iterator marker_neighbours_it;

	try {
		descriptor = gwafile->get_descriptor();

		markers_by_chr_it = markers_by_chr.begin();
		while (markers_by_chr_it != markers_by_chr.end()) {
			ld_file_name = markers_by_chr_it->first;
			ld_file_path = descriptor->get_ld_file(ld_file_name);

			cout << markers_by_chr_it->first << " " << markers_by_chr_it->second->size() << endl;

			if (ld_file_path != NULL) {
				markers = markers_by_chr_it->second;

				markers_ld_it = markers_ld.begin();
				while (markers_ld_it != markers_ld.end()) {
					markers_ld_it->second->clear();
					delete markers_ld_it->second;

					markers_ld_it++;
				}
				markers_ld.clear();

				index_ld(ld_file_path);

				independent_markers = new map<const char*, double, bool(*)(const char*, const char*)>(auxiliary::bool_strcmp);

				while (markers->size() > 0) {
					markers_it = markers->begin();
					min_pvalue_it = markers_it;
					while (++markers_it != markers->end()) {
						if (auxiliary::fcmp(markers_it->second, min_pvalue_it->second, EPSILON) < 0) {
							min_pvalue_it = markers_it;
						}
					}

					independent_markers->insert(pair<const char*, double>(min_pvalue_it->first, min_pvalue_it->second));

					markers_ld_it = markers_ld.find(min_pvalue_it->first);

					markers->erase(min_pvalue_it);

					if (markers_ld_it != markers_ld.end()) {
						marker_neighbours = markers_ld_it->second;
						marker_neighbours_it = marker_neighbours->begin();
						while (marker_neighbours_it != marker_neighbours->end()) {
							markers_it = markers->find(/*(char*)*/ *marker_neighbours_it);
							if (markers_it != markers->end()) {
								markers->erase(markers_it);
							}
							marker_neighbours_it++;
						}
					}
				}

				markers_by_chr_it->second = independent_markers;

				delete markers;
				markers = NULL;
			}

			cout << markers_by_chr_it->first << " " << markers_by_chr_it->second->size() << endl;

			markers_by_chr_it++;
		}
	} catch (DescriptorException &e) {
		throw;
	}
}

void Selector::write_remaining_markers() throw (SelectorException) {
	Descriptor* descriptor = NULL;

	Writer* writer = NULL;

	const char* output_prefix = NULL;
	const char* file_name = NULL;
	char* output_file_name = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 2u;
	char* line_backup = NULL;

	char header_separator = '\0';
	char data_separator = '\0';

	char* token = NULL;

	int column_position = 0;

	char* marker_token = NULL;
	char* chr_token = NULL;

	if (gwafile == NULL) {
		return;
	}

	try {
		descriptor = gwafile->get_descriptor();
		output_prefix = descriptor->get_property(Descriptor::PREFIX);
		file_name = descriptor->get_name();
		header_separator = gwafile->get_header_separator();
		data_separator = gwafile->get_data_separator();

		auxiliary::transform_file_name(&output_file_name, output_prefix, file_name, NULL, true);
		if (output_file_name == NULL) {
			throw SelectorException("Selector", "independize()", __LINE__/*, Selector exception: 16 */);
		}

		line_backup = (char*)malloc(reader->get_buffer_size() * sizeof(char));
		if (line_backup == NULL) {
			throw SelectorException("Selector", "independize()", __LINE__, 2, (reader->get_buffer_size() * sizeof(char)));
		}

		writer = WriterFactory::create(WriterFactory::TEXT);
		writer->set_file_name(output_file_name);
		writer->open();

		reader->reset();

		if (reader->read_line() <= 0) {
			throw SelectorException("Selector", "independize()", __LINE__, 5, 1, gwafile->get_descriptor()->get_name());
		} else {
			writer->write("%s\n", *(reader->line));
		}

		while ((line_length = reader->read_line()) > 0) {
			line = *(reader->line);

			strcpy(line_backup, line);

			column_position = 0;
			marker_token = NULL;
			chr_token = NULL;
			token = auxiliary::strtok(&line, data_separator);
			while (token != NULL) {
				if (column_position == marker_column_pos) {
					marker_token = token;
				} else if (column_position == chr_column_pos) {
					chr_token = token;
				}
				token = auxiliary::strtok(&line, data_separator);
				++column_position;
			}

			if (column_position < total_columns) {
				throw SelectorException("Selector", "process_data()", __LINE__, 8, line_number, descriptor->get_name(), column_position, total_columns);
			} else if (column_position > total_columns) {
				throw SelectorException("Selector", "process_data()", __LINE__, 9, line_number, descriptor->get_name(), column_position, total_columns);
			}

			markers_by_chr_it = markers_by_chr.find(chr_token);
			if (markers_by_chr_it != markers_by_chr.end()) {
				markers = markers_by_chr_it->second;
				if (markers->count(marker_token) > 0) {
					writer->write("%s\n", line_backup);
				}
			}

			++line_number;
		}

		free(output_file_name);
		output_file_name = NULL;

		free(line_backup);
		line_backup = NULL;

		writer->close();
		delete writer;
		writer = NULL;

		if (line_length == 0) {
			throw SelectorException("Selector", "independize()", __LINE__, 11, line_number, descriptor->get_name());
		}
	} catch (DescriptorException &e) {
		throw;
	} catch (ReaderException &e) {
		SelectorException new_e(e);
		new_e.add_message("Selector", "independize()", __LINE__, 14 /* Selector exception */, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (SelectorException &e) {
		e.add_message("Selector", "independize()", __LINE__, 14 /* Selector exception */, gwafile->get_descriptor()->get_name());
		throw;
	}
}

void Selector::independize() throw (SelectorException) {
	drop_correlated_markers();
	write_remaining_markers();
}
