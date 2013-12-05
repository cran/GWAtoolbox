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

#include "include/Harmonizer.h"

const char Harmonizer::VCF_FIELD_SEPARATOR = '\t';
const char* Harmonizer::VCF_FILE_FORMAT = "##fileformat";
const char* Harmonizer::VCF_CHROM = "#CHROM";
const char* Harmonizer::VCF_POS = "POS";
const char* Harmonizer::VCF_ID = "ID";
const char* Harmonizer::VCF_REF = "REF";
const char* Harmonizer::VCF_ALT = "ALT";
const char* Harmonizer::VCF_QUAL = "QUAL";
const char* Harmonizer::VCF_FILTER = "FILTER";
const char* Harmonizer::VCF_INFO = "INFO";
const char* Harmonizer::VCF_FORMAT = "FORMAT";
const int Harmonizer::VCF_MANDATORY_COLUMNS_SIZE = 9;
const char* Harmonizer::vcf_mandatory_columns[VCF_MANDATORY_COLUMNS_SIZE] = {
		VCF_CHROM, VCF_POS, VCF_ID, VCF_REF, VCF_ALT, VCF_QUAL, VCF_FILTER, VCF_INFO, VCF_FORMAT
};
const char Harmonizer::VCF_INFO_FIELD_SEPARATOR = ';';
const char* Harmonizer::VCF_VARIANT_TYPE = "VT";
const char* Harmonizer::VCF_SNP_TYPE = "SNP";
const char* Harmonizer::VCF_INDEL_TYPE_01 = "INDEL";
const char* Harmonizer::VCF_INDEL_TYPE_02 = "I";

const unsigned int Harmonizer::MAP_HEAP_SIZE = 3000000;
const unsigned int Harmonizer::MAP_HEAP_INCREMENT = 1000000;

Harmonizer::Harmonizer() : map_file(NULL), map_reader(NULL), map_file_line_number(0u), map_file_column_number(0),
		file(NULL), reader(NULL), id_column(NULL), ref_allele_column(NULL), nonref_allele_column(NULL),
		separator('\0'), header_backup(NULL), file_column_number(0),
		id_column_pos(numeric_limits<int>::min()), ref_allele_column_pos(numeric_limits<int>::min()), nonref_allele_column_pos(numeric_limits<int>::min()),
		map_index(NULL), id_index(NULL), index_size(0u), current_index_heap_size(0u) {

}

Harmonizer::~Harmonizer() {
	if (map_file != NULL) {
		free(map_file);
		map_file = NULL;
	}

	if (map_reader != NULL) {
		delete map_reader;
		map_reader = NULL;
	}

	if (file != NULL) {
		free(file);
		file = NULL;
	}

	if (reader != NULL) {
		delete reader;
		reader = NULL;
	}

	if (id_column != NULL) {
		free(id_column);
		id_column = NULL;
	}

	if (ref_allele_column != NULL) {
		free(ref_allele_column);
		ref_allele_column = NULL;
	}

	if (nonref_allele_column != NULL) {
		free(nonref_allele_column);
		nonref_allele_column = NULL;
	}

	if (header_backup != NULL) {
		free(header_backup);
		header_backup = NULL;
	}

	if (map_index != NULL) {
		for (unsigned int i = 0u; i < index_size; ++i) {
			free(map_index[i].chromosome);
			map_index[i].chromosome = NULL;

			free(map_index[i].id);
			map_index[i].id = NULL;
		}

		free(map_index);
		map_index = NULL;
	}

	if (id_index != NULL) {
		for (unsigned int i = 0u; i < index_size; ++i) {
			id_index[i].id = NULL;
		}

		free(id_index);
		id_index = NULL;
	}
}

void Harmonizer::open_map_file(const char* file_name) throw (HarmonizerException) {
	try {
		if (file_name == NULL) {
			throw HarmonizerException("Harmonizer", "open_map_file( const char* )", __LINE__, 0, "file_name");
		}

		if (strlen(file_name) <= 0) {
			throw HarmonizerException("Harmonizer", "open_map_file( const char* )", __LINE__, 1, "file_name");
		}

		if ((map_file != NULL) || (map_reader != NULL)) {
			close_map_file();
		}

		map_file = (char*)malloc((strlen(file_name) + 1u) * sizeof(char));
		if (map_file == NULL) {
			throw HarmonizerException("Harmonizer", "open_map_file( const char* )", __LINE__, 2, (strlen(file_name) + 1u) * sizeof(char));
		}
		strcpy(map_file, file_name);

		map_reader = ReaderFactory::create(map_file);
		map_reader->set_file_name(map_file);
		map_reader->open();
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "open_map_file( const char* )", __LINE__, 3, (map_file != NULL) ? map_file : "NULL");
		throw new_e;
	} catch (HarmonizerException &e) {
		e.add_message("Harmonizer", "open_map_file( const char* )", __LINE__, 3, (map_file != NULL) ? map_file : "NULL");
		throw;
	}
}

void Harmonizer::close_map_file() throw (HarmonizerException) {
	try {
		if ((map_reader != NULL) && (map_reader->is_open())) {
			map_reader->close();

			delete map_reader;
			map_reader = NULL;
		}

		if (map_file != NULL) {
			free(map_file);
			map_file = NULL;
		}

		map_file_line_number = 0u;
		map_file_column_number = 0;
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "close_map_file()", __LINE__, 4, (map_file != NULL) ? map_file : "NULL");
		throw new_e;
	}
}


void Harmonizer::process_map_file_header() throw (HarmonizerException) {
	char* line = NULL;
	int line_length = 0;

	char* token = NULL;

	if ((map_file == NULL) || (map_reader == NULL)) {
		return;
	}

	map_file_line_number = 0u;
	map_file_column_number = 0;

	try {
		/* Read the first required line with file format description. */
		if ((line_length = map_reader->read_line()) > 0) {
			++map_file_line_number;
			line = *(map_reader->line);
			if ((token = auxiliary::strtok(&line, '=')) != NULL) {
				auxiliary::trim_end(token);
				if (auxiliary::strcmp_ignore_case(token, VCF_FILE_FORMAT) != 0) {
					throw HarmonizerException("Harmonizer", "process_map_file_header()", __LINE__, 8);
				}
			} else {
				throw HarmonizerException("Harmonizer", "process_map_file_header()", __LINE__, 8);
			}
		}

		if (line_length == 0) {
			throw HarmonizerException("Harmonizer", "process_map_file_header()", __LINE__, 9);
		}

		/* Read the mandatory header. Meta-info lines are optional. */
		while ((line_length = map_reader->read_line()) > 0) {
			++map_file_line_number;
			line = *(map_reader->line);

			if (line_length > 1) {
				if (line[0u] != '#') {
					throw HarmonizerException("Harmonizer", "process_map_file_header()", __LINE__, 10);
				}

				if (line[1u] != '#') {
					while ((token = auxiliary::strtok(&line, VCF_FIELD_SEPARATOR)) != NULL) {
						if (map_file_column_number < VCF_MANDATORY_COLUMNS_SIZE) {
							if (auxiliary::strcmp_ignore_case(token, vcf_mandatory_columns[map_file_column_number]) != 0) {
								throw HarmonizerException("Harmonizer", "process_map_file_header()", __LINE__, 11, vcf_mandatory_columns[map_file_column_number], map_file_column_number + 1u);
							}
						} else {
							/* sample columns */
						}
						++map_file_column_number;
					}
					break;
				} else {
					/* process meta-info line if necessary */
				}
			} else {
				throw HarmonizerException("Harmonizer", "process_map_file_header()", __LINE__, 12, map_file_line_number);
			}
		}

		if (line_length == 0) {
			throw HarmonizerException("Harmonizer", "process_map_file_header()", __LINE__, 13, map_file_line_number + 1u);
		}
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "process_map_file_header()", __LINE__, 5, (map_file != NULL) ? map_file : "NULL");
		throw new_e;
	}
}

void Harmonizer::process_map_file_data() throw (HarmonizerException) {
	char* line = NULL;
	int line_length = 0;

	int column_number = 0;

	char* token = NULL;
	char** tokens = NULL;

	if ((map_file == NULL) || (map_reader == NULL)) {
		return;
	}

	map_index_entry* map_index_new = NULL;

	char* chromosome = NULL;
	char* id = NULL;
	unsigned long int position = 0ul;
	char type = '\0';

	int vt_length = strlen(VCF_VARIANT_TYPE);

	try {
		current_index_heap_size = MAP_HEAP_SIZE;
		map_index = (map_index_entry*)malloc(current_index_heap_size * sizeof(map_index_entry));
		if (map_index == NULL) {
			throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 2, MAP_HEAP_SIZE * sizeof(map_index_entry));
		}

		/* Read data. */
		tokens = (char**)malloc(VCF_MANDATORY_COLUMNS_SIZE * sizeof(char*));
		if (tokens == NULL) {
			throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 2, VCF_MANDATORY_COLUMNS_SIZE * sizeof(char*));
		}

		while ((line_length = map_reader->read_line()) > 0) {
			++map_file_line_number;
			line = *(map_reader->line);
			column_number = 0;

			while ((token = auxiliary::strtok(&line, VCF_FIELD_SEPARATOR)) != NULL) {
				if (column_number < VCF_MANDATORY_COLUMNS_SIZE) {
					tokens[column_number] = token;
				}
				++column_number;
			}

			if (column_number < map_file_column_number) {
				throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 14, map_file_line_number, map_file, column_number, map_file_column_number);
			} if (column_number > map_file_column_number) {
				throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 15, map_file_line_number, map_file, column_number, map_file_column_number);
			}

			chromosome = (char*)malloc((strlen(tokens[0u]) + 1u) * sizeof(char));
			if (chromosome == NULL) {
				throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 2, (strlen(tokens[0u]) + 1u) * sizeof(char));
			}
			strcpy(chromosome, tokens[0u]);

			if (!auxiliary::to_ulong_int(tokens[1u], &position)) {
				throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 20, tokens[1u], map_file_line_number, map_file);
			}

			id = (char*)malloc((strlen(tokens[2u]) + 1u) * sizeof(char));
			if (id == NULL) {
				throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 2, (strlen(tokens[2u]) + 1u) * sizeof(char));
			}
			strcpy(id, tokens[2u]);

			type = 'I';

			while ((token = auxiliary::strtok(&(tokens[7u]), VCF_INFO_FIELD_SEPARATOR)) != NULL) {
				auxiliary::trim_start(token);
				if (auxiliary::strcmp_ignore_case(token, VCF_VARIANT_TYPE, vt_length) == 0) {
					token = strchr(token, '=');
					if (token != NULL) {
						++token;

						auxiliary::trim_start(token);
						auxiliary::trim_end(token);

						if (auxiliary::strcmp_ignore_case(token, VCF_SNP_TYPE) == 0) {
							type = 'S';
						}
					}
					break;
				}
			}

			if (index_size >= current_index_heap_size) {
				current_index_heap_size += MAP_HEAP_INCREMENT;

				map_index_new = (map_index_entry*)realloc(map_index, current_index_heap_size * sizeof(map_index_entry));
				if (map_index_new == NULL) {
					throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 19, current_index_heap_size * sizeof(map_index_entry));
				}
				map_index = map_index_new;
				map_index_new = NULL;
			}

			map_index[index_size].chromosome = chromosome;
			map_index[index_size].position = position;
			map_index[index_size].id = id;
			map_index[index_size].type = type;

			++index_size;
		}

		if (line_length == 0) {
			throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 13, map_file_line_number + 1u);
		}

		free(tokens);
		tokens = NULL;

		qsort(map_index, index_size, sizeof(map_index_entry), qsort_map_index_entry_cmp);

		id_index = (id_index_entry*)malloc(index_size * sizeof(id_index_entry));
		if (id_index == NULL) {
			throw HarmonizerException("Harmonizer", "process_map_file_data()", __LINE__, 2, index_size * sizeof(id_index_entry));
		}

		for (unsigned int i = 0u; i < index_size; ++i) {
			id_index[i].id = map_index[i].id;
			id_index[i].location = i;
		}

		qsort(id_index, index_size, sizeof(id_index_entry), qsort_id_index_entry_cmp);
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "process_map_file_data()", __LINE__, 5, (map_file != NULL) ? map_file : "NULL");
		throw new_e;
	}
}

void Harmonizer::index_map(const char* file_name) throw (HarmonizerException) {
	try {
		if (file_name == NULL) {
			throw HarmonizerException("Harmonizer", "index_map( const char* )", __LINE__, 0, "file_name");
		}

		if (strlen(file_name) <= 0) {
			throw HarmonizerException("Harmonizer", "index_map( const char* )", __LINE__, 1, "file_name");
		}

		if (map_index != NULL) {
			for (unsigned int i = 0u; i < index_size; ++i) {
				free(map_index[i].chromosome);
				map_index[i].chromosome = NULL;

				free(map_index[i].id);
				map_index[i].id = NULL;
			}

			free(map_index);
			map_index = NULL;
		}

		if (id_index != NULL) {
			for (unsigned int i = 0u; i < index_size; ++i) {
				id_index[i].id = NULL;
			}

			free(id_index);
			id_index = NULL;
		}

		index_size = 0u;
		current_index_heap_size = 0u;

		open_map_file(file_name);
		process_map_file_header();
		process_map_file_data();
		close_map_file();
	} catch (HarmonizerException &e) {
		e.add_message("Harmonizer", "index_map( const char* )", __LINE__, 7);
		throw;
	}
}

void Harmonizer::open_file(const char* file_name, const char* id_column_name, const char* ref_allele_column_name, const char* nonref_allele_column_name, char field_separator) throw (HarmonizerException) {
	try {
		if ((file != NULL) || (reader != NULL)) {
			close_file();
		}

		if (file_name == NULL) {
			throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 0, "file_name");
		}

		if (strlen(file_name) <= 0) {
			throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 1, "file_name");
		}

		file = (char*)malloc((strlen(file_name) + 1u) * sizeof(char));
		if (file == NULL) {
			throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 2, (strlen(file_name) + 1u) * sizeof(char));
		}
		strcpy(file, file_name);

		if (id_column_name == NULL) {
			throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 0, "id_column_name");
		}

		if (strlen(id_column_name) <= 0) {
			throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 1, "id_column_name");
		}

		id_column = (char*)malloc((strlen(id_column_name) + 1u) * sizeof(char));
		if (id_column == NULL) {
			throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 2, (strlen(id_column_name) + 1u) * sizeof(char));
		}
		strcpy(id_column, id_column_name);

		if ((ref_allele_column_name != NULL) && (nonref_allele_column_name != NULL)) {
			if (strlen(ref_allele_column_name) <= 0) {
				throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 1, "ref_allele_column_name");
			}

			if (strlen(nonref_allele_column_name) <= 0) {
				throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 1, "nonref_allele_column_name");
			}

			ref_allele_column = (char*)malloc((strlen(ref_allele_column_name) + 1u) * sizeof(char));
			if (ref_allele_column == NULL) {
				throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 2, (strlen(ref_allele_column_name) + 1u) * sizeof(char));
			}
			strcpy(ref_allele_column, ref_allele_column_name);

			nonref_allele_column = (char*)malloc((strlen(nonref_allele_column_name) + 1u) * sizeof(char));
			if (nonref_allele_column == NULL) {
				throw HarmonizerException("Harmonizer", "open_file( const char*, const char*, const char*, const char*, char )", __LINE__, 2, (strlen(nonref_allele_column_name) + 1u) * sizeof(char));
			}
			strcpy(nonref_allele_column, nonref_allele_column_name);
		}

		separator = field_separator;

		reader = ReaderFactory::create(file);
		reader->set_file_name(file);
		reader->open();
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "open_file( const char*, const char*, char )", __LINE__, 3, (file != NULL) ? file : "NULL");
		throw new_e;
	} catch (HarmonizerException &e) {
		e.add_message("Harmonizer", "open_file( const char*, const char*, char )", __LINE__, 3, (file != NULL) ? file : "NULL");
		throw;
	}
}

void Harmonizer::close_file() throw (HarmonizerException) {
	try {
		if ((reader != NULL) && (reader->is_open())) {
			reader->close();

			delete reader;
			reader = NULL;
		}

		if (file != NULL) {
			free(file);
			file = NULL;
		}

		if (id_column != NULL) {
			free(id_column);
			id_column = NULL;
		}

		if (ref_allele_column != NULL) {
			free(ref_allele_column);
			ref_allele_column = NULL;
		}

		if (nonref_allele_column != NULL) {
			free(nonref_allele_column);
			nonref_allele_column = NULL;
		}

		separator = '\0';

		if (header_backup != NULL) {
			free(header_backup);
			header_backup = NULL;
		}

		file_column_number = 0;
		id_column_pos = numeric_limits<int>::min();
		ref_allele_column_pos = numeric_limits<int>::min();
		nonref_allele_column_pos = numeric_limits<int>::min();
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "close_file()", __LINE__, 4, (file != NULL) ? file : "NULL");
		throw new_e;
	}
}

void Harmonizer::process_header() throw (HarmonizerException) {
	char* header = NULL;
	char* token = NULL;

	if ((file == NULL) || (reader == NULL) || (id_column == NULL)) {
		return;
	}

	file_column_number = 0;
	id_column_pos = numeric_limits<int>::min();
	ref_allele_column_pos = numeric_limits<int>::min();
	nonref_allele_column_pos = numeric_limits<int>::min();

	try {
		if (reader->read_line() <= 0) {
			throw HarmonizerException("Harmonizer", "process_header()", __LINE__, 16, 1, file);
		}

		header = *(reader->line);

		header_backup = (char*)malloc((strlen(header) + 1u) * sizeof(char));
		if (header_backup == NULL) {
			throw HarmonizerException("Harmonizer", "process_header()", __LINE__, 2, ((strlen(header) + 1u) * sizeof(char)));
		}
		strcpy(header_backup, header);

		if ((ref_allele_column != NULL) && (nonref_allele_column != NULL)) {
			while ((token = auxiliary::strtok(&header, separator)) != NULL) {
				if (auxiliary::strcmp_ignore_case(token, id_column) == 0) {
					id_column_pos = file_column_number;
				} else if (auxiliary::strcmp_ignore_case(token, ref_allele_column) == 0) {
					ref_allele_column_pos = file_column_number;
				} else if (auxiliary::strcmp_ignore_case(token, nonref_allele_column) == 0) {
					nonref_allele_column_pos = file_column_number;
				}
				++file_column_number;
			}

			if (ref_allele_column_pos < 0) {
				throw HarmonizerException("Harmonizer", "process_header()", __LINE__, 17, ref_allele_column, file);
			}

			if (nonref_allele_column_pos < 0) {
				throw HarmonizerException("Harmonizer", "process_header()", __LINE__, 17, nonref_allele_column, file);
			}
		} else {
			while ((token = auxiliary::strtok(&header, separator)) != NULL) {
				if (auxiliary::strcmp_ignore_case(token, id_column) == 0) {
					id_column_pos = file_column_number;
				}
				++file_column_number;
			}
		}

		if (id_column_pos < 0) {
			throw HarmonizerException("Harmonizer", "process_header()", __LINE__, 17, id_column, file);
		}
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "process_header()", __LINE__, 5, (file != NULL) ? file : "NULL");
		throw new_e;
	}
}

void Harmonizer::harmonize_without_alleles(const char* output_file_name, bool gzip) throw (HarmonizerException) {
	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 2u;

	char* token = NULL;
	char** tokens = NULL;

	int column_number = 0;

	id_index_entry query_id;
	map_index_entry query_map;
	id_index_entry* found_id = NULL;
	map_index_entry* found_map = NULL;

	char variant_type = '\0';

	char* buffer = NULL;
	char* buffer_tmp = NULL;
	unsigned int buffer_tokens_number = 0u;
	char* buffer_token = NULL;
	char* buffer_tokens[3u];
	char* end_ptr = NULL;

	Writer* writer = NULL;

	char* log_file_name = NULL;
	Writer* log_writer = NULL;

	if ((file == NULL) || (reader == NULL) || (id_column == NULL) || (id_column_pos < 0)) {
		return;
	}

	try {
		if (output_file_name == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 0, "output_file_name");
		}

		if (strlen(output_file_name) <= 0) {
			throw HarmonizerException("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 1, "output_file_name");
		}

		auxiliary::transform_file_name(&log_file_name, NULL, output_file_name, ".log", true);
		if (log_file_name == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 18);
		}

		tokens = (char**)malloc(file_column_number * sizeof(char*));
		if (tokens == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 2, file_column_number * sizeof(char*));
		}

		buffer = (char*)malloc(16777216 * sizeof(char));
		if (buffer == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 2, 16777216 * sizeof(char));
		}

		writer = WriterFactory::create(gzip ? WriterFactory::GZIP : WriterFactory::TEXT);
		writer->set_file_name(output_file_name);
		writer->open();

		log_writer = WriterFactory::create(WriterFactory::TEXT);
		log_writer->set_file_name(log_file_name);
		log_writer->open();

		writer->write("%s\n", header_backup);

		while ((line_length = reader->read_line()) > 0) {
			line = *(reader->line);

			column_number = 0;
			while ((token = auxiliary::strtok(&line, separator)) != NULL) {
				if (column_number < file_column_number) {
					tokens[column_number] = token;
				}
				++column_number;
			}

			if (column_number < file_column_number) {
				throw HarmonizerException("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 14, line_number, file, column_number, file_column_number);
			} else if (column_number > file_column_number) {
				throw HarmonizerException("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 15, line_number, file, column_number, file_column_number);
			}

			/* BEGIN: parse and map SNP identifier. */
			if (auxiliary::strcmp_ignore_case(tokens[id_column_pos], "rs", 2) == 0) {
				query_id.id = tokens[id_column_pos];
				found_id = (id_index_entry*)bsearch(&query_id, id_index, index_size, sizeof(id_index_entry), qsort_id_index_entry_cmp);
				if (found_id != NULL) {
					found_map = &(map_index[found_id->location]);
					variant_type = '\0';
				} else {
					found_map = NULL;
				}
			} else {
				strcpy(buffer, tokens[id_column_pos]);
				buffer_tmp = buffer;

				buffer_tokens_number = 0u;
				buffer_tokens[0u] = buffer_tokens[1u] = buffer_tokens[2u] = NULL;
				while ((buffer_tokens_number < 3u) && ((buffer_token = auxiliary::strtok(&buffer_tmp, ':')) != NULL)) {
					buffer_tokens[buffer_tokens_number++] = buffer_token;
				}

				if ((buffer_tokens[0u] != NULL) && (buffer_tokens[1u] != NULL)) {
					if (auxiliary::strcmp_ignore_case(buffer_tokens[0u], "chr", 3) == 0) {
						query_map.chromosome = buffer_tokens[0u] + 3;
					} else {
						query_map.chromosome = buffer_tokens[0u];
					}

					query_map.position = strtoul(buffer_tokens[1u], &end_ptr, 10);
					if (end_ptr != buffer_tokens[1u]) {
						found_map = (map_index_entry*)bsearch(&query_map, map_index, index_size, sizeof(map_index_entry), qsort_map_index_entry_cmp);
					} else {
						found_map = NULL;
					}

					if (buffer_tokens[2u] != NULL) {
						if (auxiliary::strcmp_ignore_case(buffer_tokens[2u], VCF_SNP_TYPE) == 0) {
							variant_type = 'S';
						} else if ((auxiliary::strcmp_ignore_case(buffer_tokens[2u], VCF_INDEL_TYPE_01) == 0) ||
								(auxiliary::strcmp_ignore_case(buffer_tokens[2u], VCF_INDEL_TYPE_02) == 0)) {
							variant_type = 'I';
						} else {
							variant_type = 'O';
						}
					} else {
						variant_type = '\0';
					}
				} else {
					found_map = NULL;
				}
			}
			/* END: parse and map SNP identifier. */

			/* BEGIN: output new columns. */
			if (id_column_pos == 0) {
				if (found_map != NULL) {
					if (found_map->type == 'S') {
						writer->write("%s:%u:%s", found_map->chromosome, found_map->position, VCF_SNP_TYPE);

						if ((variant_type != '\0') && (variant_type != 'S')) {
							log_writer->write("Line %u: Variant type of variant %s doesn't match the variant type in the map file.\n", line_number, tokens[0]);
						}
					} else if (found_map->type == 'I') {
						writer->write("%s:%u:%s", found_map->chromosome, found_map->position, VCF_INDEL_TYPE_01);

						if ((variant_type != '\0') && (variant_type != 'I')) {
							log_writer->write("Line %u: Variant type of variant %s doesn't match the variant type in the map file.\n", line_number, tokens[0]);
						}
					} else {
						writer->write("%s:%u:%s", found_map->chromosome, found_map->position);
						log_writer->write("Line %u: Variant type of variant %s was not found in the map file.", line_number, tokens[0]);
					}
				} else {
					writer->write("%s", tokens[0]);
					log_writer->write("Line %u: Variant %s was not found in the map file.\n", line_number, tokens[0]);
				}
			} else {
				writer->write("%s", tokens[0]);
			}

			column_number = 1;
			while (column_number < file_column_number) {
				if (column_number == id_column_pos) {
					if (found_map != NULL) {
						if (found_map->type == 'S') {
							writer->write("\t%s:%u:%s", found_map->chromosome, found_map->position, VCF_SNP_TYPE);

							if ((variant_type != '\0') && (variant_type != 'S')) {
								log_writer->write("Line %u: Variant type of variant %s doesn't match the variant type in the map file.\n", line_number, tokens[column_number]);
							}
						} else if (found_map->type == 'I') {
							writer->write("\t%s:%u:%s", found_map->chromosome, found_map->position, VCF_INDEL_TYPE_01);

							if ((variant_type != '\0') && (variant_type != 'I')) {
								log_writer->write("Line %u: Variant type of variant %s doesn't match the variant type in the map file.\n", line_number, tokens[column_number]);
							}
						} else {
							writer->write("\t%s:%u:%s", found_map->chromosome, found_map->position);
							log_writer->write("Line %u: Variant type of variant %s was not found in the map file.", line_number, tokens[column_number]);
						}
					} else {
						writer->write("\t%s", tokens[column_number]);
						log_writer->write("Line %u: Variant %s was not found in the map file.\n", line_number, tokens[column_number]);
					}
				} else {
					writer->write("\t%s", tokens[column_number]);
				}
				++column_number;
			}

			writer->write("\n");
			/* END: output new columns. */

			++line_number;
		}

		writer->close();
		log_writer->close();

		delete(writer);
		writer = NULL;

		delete(log_writer);
		log_writer = NULL;

		free(log_file_name);
		log_file_name = NULL;

		free(buffer);
		buffer = NULL;

		if (line_length == 0) {
			throw HarmonizerException("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 13, line_number, file);
		}
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 5, (file != NULL) ? file : "NULL");
		throw new_e;
	} catch (WriterException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 5, (file != NULL) ? file : "NULL");
		throw new_e;
	} catch (HarmonizerException &e) {
		e.add_message("Harmonizer", "harmonize_without_alleles( const char*, bool )", __LINE__, 5, (file != NULL) ? file : "NULL");
		throw;
	}
}

void Harmonizer::harmonize_with_alleles(const char* output_file_name, bool drop, bool gzip) throw (HarmonizerException) {
	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 2u;

	char* token = NULL;
	char** tokens = NULL;

	int column_number = 0;

	id_index_entry query_id;
	map_index_entry query_map;
	id_index_entry* found_id = NULL;
	map_index_entry* found_map = NULL;

	unsigned int found_id_index_entry_pos = 0u;
	unsigned int found_map_index_entry_pos = 0u;
	long int position = 0;

	char* ref_allele = NULL;
	char* nonref_allele = NULL;

	bool type_consistent = false;

	char* buffer = NULL;
	char* buffer_tmp = NULL;
	unsigned int buffer_tokens_number = 0u;
	char* buffer_token = NULL;
	char* buffer_tokens[3u];
	char* end_ptr = NULL;

	Writer* writer = NULL;

	char* log_file_name = NULL;
	Writer* log_writer = NULL;

	bool unresolved = true;

	if ((file == NULL) || (reader == NULL) || (id_column == NULL) || (id_column_pos < 0)) {
		return;
	}

	try {
		if (output_file_name == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 0, "output_file_name");
		}

		if (strlen(output_file_name) <= 0) {
			throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 1, "output_file_name");
		}

		auxiliary::transform_file_name(&log_file_name, NULL, output_file_name, ".log", true);
		if (log_file_name == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 18);
		}

		tokens = (char**)malloc(file_column_number * sizeof(char*));
		if (tokens == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 2, file_column_number * sizeof(char*));
		}

		buffer = (char*)malloc(16777216 * sizeof(char));
		if (buffer == NULL) {
			throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 2, 16777216 * sizeof(char));
		}

		writer = WriterFactory::create(gzip ? WriterFactory::GZIP : WriterFactory::TEXT);
		writer->set_file_name(output_file_name);
		writer->open();

		log_writer = WriterFactory::create(WriterFactory::TEXT);
		log_writer->set_file_name(log_file_name);
		log_writer->open();

		writer->write("%s\n", header_backup);

		while ((line_length = reader->read_line()) > 0) {
			line = *(reader->line);

			found_id = NULL;
			found_map = NULL;

			column_number = 0;
			while ((token = auxiliary::strtok(&line, separator)) != NULL) {
				if (column_number < file_column_number) {
					tokens[column_number] = token;
				}
				++column_number;
			}

			if (column_number < file_column_number) {
				throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 14, line_number, file, column_number, file_column_number);
			} else if (column_number > file_column_number) {
				throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 15, line_number, file, column_number, file_column_number);
			}

			ref_allele = tokens[ref_allele_column_pos];
			auxiliary::trim_start(ref_allele);
			auxiliary::trim_end(ref_allele);

			nonref_allele = tokens[nonref_allele_column_pos];
			auxiliary::trim_start(nonref_allele);
			auxiliary::trim_end(nonref_allele);

			/* BEGIN: parse and map SNP identifier. */
			if (auxiliary::strcmp_ignore_case(tokens[id_column_pos], "rs", 2) == 0) {
				/* BEGIN: check alleles. */
				if (((auxiliary::strcmp_ignore_case(ref_allele, "A") != 0) && (auxiliary::strcmp_ignore_case(ref_allele, "C") != 0) &&
						(auxiliary::strcmp_ignore_case(ref_allele, "G") != 0) && (auxiliary::strcmp_ignore_case(ref_allele, "T") != 0)) ||
						((auxiliary::strcmp_ignore_case(nonref_allele, "A") != 0) && (auxiliary::strcmp_ignore_case(nonref_allele, "C") != 0) &&
								(auxiliary::strcmp_ignore_case(nonref_allele, "G") != 0) && (auxiliary::strcmp_ignore_case(nonref_allele, "T") != 0))) {
					log_writer->write("Line %u (WARNING): %s has incorrect alleles (only A/C/G/T are allowed).\n", line_number, tokens[id_column_pos]);
				}
				/* END: check alleles. */

				/* BEGIN: Find matches in map by chromosome and position. */
				query_id.id = tokens[id_column_pos];
				found_id = (id_index_entry*)bsearch(&query_id, id_index, index_size, sizeof(id_index_entry), qsort_id_index_entry_cmp);
				/* END: Find matches in map by chromosome and position. */

				/* BEGIN: Check all matches for consistent type. */
				if (found_id != NULL) {
					found_id_index_entry_pos = found_id - id_index;
					type_consistent = false;

					position = found_id_index_entry_pos;
					while ((position >= 0) && (qsort_id_index_entry_cmp(&(id_index[position]), &query_id) == 0)) {
						if (map_index[id_index[position].location].type == 'S') {
							type_consistent = true;
							break;
						}
						--found_id_index_entry_pos;
					}

					if (!type_consistent) {
						position = found_id_index_entry_pos;
						++position;
						while ((position < index_size) && (qsort_id_index_entry_cmp(&(id_index[position]), &query_id) == 0)) {
							if (map_index[id_index[position].location].type == 'S') {
								type_consistent = true;
								break;
							}
							++found_id_index_entry_pos;
						}
					}

					if (type_consistent) {
						found_map = &(map_index[id_index[position].location]);
						sprintf(buffer, "%s:%lu:%s", found_map->chromosome, found_map->position, VCF_SNP_TYPE);
						log_writer->write("Line %u: %s changed to %s.\n", line_number, tokens[id_column_pos], buffer);
						tokens[id_column_pos] = buffer;
						unresolved = false;
					} else {
						log_writer->write("Line %u (WARNING): Type of %s doesn't match type in the map.\n", line_number, tokens[id_column_pos]);
					}
				} else {
					log_writer->write("Line %u (WARNING): %s wasn't found in the map.\n", line_number, tokens[id_column_pos]);
				}
				/* END: Check all matches for consistent type. */
			} else {
				strcpy(buffer, tokens[id_column_pos]);
				buffer_tmp = buffer;

				buffer_tokens_number = 0u;
				buffer_tokens[0u] = buffer_tokens[1u] = buffer_tokens[2u] = NULL;
				while ((buffer_tokens_number < 3u) && ((buffer_token = auxiliary::strtok(&buffer_tmp, ':')) != NULL)) {
					buffer_tokens[buffer_tokens_number++] = buffer_token;
				}

				/* BEGIN: Check/guess variant type and check alleles. */
				if (buffer_tokens[2u] != NULL) {
					if (auxiliary::strcmp_ignore_case(buffer_tokens[2u], VCF_SNP_TYPE) == 0) {
						query_map.type = 'S';

						if (((auxiliary::strcmp_ignore_case(ref_allele, "A") != 0) && (auxiliary::strcmp_ignore_case(ref_allele, "C") != 0) &&
								(auxiliary::strcmp_ignore_case(ref_allele, "G") != 0) && (auxiliary::strcmp_ignore_case(ref_allele, "T") != 0)) ||
								((auxiliary::strcmp_ignore_case(nonref_allele, "A") != 0) && (auxiliary::strcmp_ignore_case(nonref_allele, "C") != 0) &&
										(auxiliary::strcmp_ignore_case(nonref_allele, "G") != 0) && (auxiliary::strcmp_ignore_case(nonref_allele, "T") != 0))) {
							log_writer->write("Line %u (WARNING): %s has incorrect alleles (only A/C/G/T are allowed).\n", line_number, tokens[id_column_pos]);
						}
					} else {
						query_map.type = 'I';

						if ((auxiliary::strcmp_ignore_case(ref_allele, "A") == 0) || (auxiliary::strcmp_ignore_case(ref_allele, "C") == 0) ||
								(auxiliary::strcmp_ignore_case(ref_allele, "G") == 0) || (auxiliary::strcmp_ignore_case(ref_allele, "T") == 0) ||
								(auxiliary::strcmp_ignore_case(nonref_allele, "A") == 0) || (auxiliary::strcmp_ignore_case(nonref_allele, "C") == 0) ||
								(auxiliary::strcmp_ignore_case(nonref_allele, "G") == 0) || (auxiliary::strcmp_ignore_case(nonref_allele, "T") == 0)) {
							log_writer->write("Line %u (WARNING): %s has incorrect alleles (A/C/G/T are not allowed).\n", line_number, tokens[id_column_pos]);
						}
					}
				} else {
					if (((auxiliary::strcmp_ignore_case(ref_allele, "A") == 0) || (auxiliary::strcmp_ignore_case(ref_allele, "C") == 0) ||
							(auxiliary::strcmp_ignore_case(ref_allele, "G") == 0) || (auxiliary::strcmp_ignore_case(ref_allele, "T") == 0)) &&
							((auxiliary::strcmp_ignore_case(nonref_allele, "A") == 0) || (auxiliary::strcmp_ignore_case(nonref_allele, "C") == 0) ||
									(auxiliary::strcmp_ignore_case(nonref_allele, "G") == 0) || (auxiliary::strcmp_ignore_case(nonref_allele, "T") == 0))) {
						query_map.type = 'S';
					} else {
						query_map.type = 'I';
					}
				}
				/* END: Check/guess variant type and check alleles. */

				/* BEGIN: Find matches in map by chromosome and position. */
				if ((buffer_tokens[0u] != NULL) && (buffer_tokens[1u] != NULL)) {
					if (auxiliary::strcmp_ignore_case(buffer_tokens[0u], "chr", 3) == 0) {
						query_map.chromosome = buffer_tokens[0u] + 3;
					} else {
						query_map.chromosome = buffer_tokens[0u];
					}

					query_map.position = strtoul(buffer_tokens[1u], &end_ptr, 10);
					if (end_ptr != buffer_tokens[1u]) {
						found_map = (map_index_entry*)bsearch(&query_map, map_index, index_size, sizeof(map_index_entry), qsort_map_index_entry_cmp);
					}
				}
				/* END: Find matches in map by chromosome and position. */

				/* BEGIN: Check all matches for consistent type. */
				if (found_map != NULL) {
					found_map_index_entry_pos = found_map - map_index;
					type_consistent = false;

					position = found_map_index_entry_pos;
					while ((position >= 0) && (qsort_map_index_entry_cmp(&(map_index[position]), &query_map) == 0)) {
						if (map_index[position].type == query_map.type) {
							type_consistent = true;
							break;
						}
						--position;
					}

					if (!type_consistent) {
						position = found_map_index_entry_pos;
						++position;
						while ((position < index_size) && (qsort_map_index_entry_cmp(&(map_index[position]), &query_map) == 0)) {
							if (map_index[position].type == query_map.type) {
								type_consistent = true;
								break;
							}
							++position;
						}
					}

					if (type_consistent) {
						found_map = &(map_index[position]);
						if (found_map->type == 'S') {
							sprintf(buffer, "%s:%lu:%s", found_map->chromosome, found_map->position, VCF_SNP_TYPE);
						} else {
							sprintf(buffer, "%s:%lu:%s", found_map->chromosome, found_map->position, VCF_INDEL_TYPE_01);
						}

						if (strcmp(buffer,  tokens[id_column_pos]) != 0) {
							log_writer->write("Line %u: %s changed to %s.\n", line_number, tokens[id_column_pos], buffer);
							tokens[id_column_pos] = buffer;
						}

						unresolved = false;
					} else {
						log_writer->write("Line %u (WARNING): Type of %s doesn't match type in the map.\n", line_number, tokens[id_column_pos]);
					}
				} else {
					log_writer->write("Line %u (WARNING): %s wasn't found in the map.\n", line_number, tokens[id_column_pos]);
				}
				/* END: Check all matches for consistent type. */
			}
			/* END: parse and map SNP identifier. */

			/* BEGIN: output new columns. */
			if (drop) {
				if (!unresolved) {
					writer->write("%s", tokens[0]);
					column_number = 1;
					while (column_number < file_column_number) {
						writer->write("\t%s", tokens[column_number]);
						++column_number;
					}
					writer->write("\n");

					unresolved = true;
				}
			} else {
				writer->write("%s", tokens[0]);
				column_number = 1;
				while (column_number < file_column_number) {
					writer->write("\t%s", tokens[column_number]);
					++column_number;
				}
				writer->write("\n");
			}
			/* END: output new columns. */

			++line_number;
		}

		writer->close();
		log_writer->close();

		delete(writer);
		writer = NULL;

		delete(log_writer);
		log_writer = NULL;

		free(log_file_name);
		log_file_name = NULL;

		free(tokens);
		tokens = NULL;

		free(buffer);
		buffer = NULL;

		if (line_length == 0) {
			throw HarmonizerException("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 13, line_number, file);
		}
	} catch (ReaderException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 5, (file != NULL) ? file : "NULL");
		throw new_e;
	} catch (WriterException &e) {
		HarmonizerException new_e(e);
		new_e.add_message("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 5, (file != NULL) ? file : "NULL");
		throw new_e;
	} catch (HarmonizerException &e) {
		e.add_message("Harmonizer", "harmonize_with_alleles( const char*, bool )", __LINE__, 5, (file != NULL) ? file : "NULL");
		throw;
	}
}

void Harmonizer::harmonize(const char* output_file_name, bool drop, bool gzip) throw (HarmonizerException) {
	if ((file == NULL) || (reader == NULL) || (id_column == NULL) || (id_column_pos < 0)) {
		return;
	}

	if ((ref_allele_column != NULL) && (nonref_allele_column != NULL)) {
		harmonize_with_alleles(output_file_name, drop, gzip);
	} else {
		/* Is never called, since alleles columns are mandatory. */
		harmonize_without_alleles(output_file_name, gzip);
	}
}

inline int Harmonizer::qsort_map_index_entry_cmp(const void* first, const void* second) {
	map_index_entry* first_entry = (map_index_entry*)first;
	map_index_entry* second_entry = ((map_index_entry*)second);

	if (first_entry->position > second_entry->position) {
		return 1;
	} else if (first_entry->position < second_entry->position) {
		return -1;
	} else {
		return auxiliary::strcmp_ignore_case(first_entry->chromosome, second_entry->chromosome);
	}
}

inline int Harmonizer::qsort_id_index_entry_cmp(const void* first, const void* second) {
	return auxiliary::strcmp_ignore_case(((id_index_entry*)first)->id, ((id_index_entry*)second)->id);
}
