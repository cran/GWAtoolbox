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

#ifndef HARMONIZER_H_
#define HARMONIZER_H_

#include <cstdio>

#include "HarmonizerException.h"
#include "../../reader/include/ReaderFactory.h"
#include "../../writer/include/WriterFactory.h"
#include "../../auxiliary/include/auxiliary.h"

class Harmonizer {
private:
	static const char VCF_FIELD_SEPARATOR;
	static const char* VCF_FILE_FORMAT;
	static const char* VCF_CHROM;
	static const char* VCF_POS;
	static const char* VCF_ID;
	static const char* VCF_REF;
	static const char* VCF_ALT;
	static const char* VCF_QUAL;
	static const char* VCF_FILTER;
	static const char* VCF_INFO;
	static const char* VCF_FORMAT;
	static const int VCF_MANDATORY_COLUMNS_SIZE;
	static const char* vcf_mandatory_columns[];
	static const char VCF_INFO_FIELD_SEPARATOR;
	static const char* VCF_VARIANT_TYPE;
	static const char* VCF_SNP_TYPE;
	static const char* VCF_INDEL_TYPE_01;
	static const char* VCF_INDEL_TYPE_02;

	char* map_file;
	Reader* map_reader;
	unsigned int map_file_line_number;
	int map_file_column_number;

	char* file;
	Reader* reader;
	char* id_column;
	char* ref_allele_column;
	char* nonref_allele_column;
	char separator;
	char* header_backup;
	int file_column_number;
	int id_column_pos;
	int ref_allele_column_pos;
	int nonref_allele_column_pos;

	struct map_index_entry {
		unsigned long int position;
		char* chromosome;
		char* id;
		char type;
	};

	struct id_index_entry {
		char* id;
		unsigned int location;
	};

	map_index_entry* map_index;
	id_index_entry* id_index;

	unsigned int index_size;
	unsigned int current_index_heap_size;

	static int qsort_map_index_entry_cmp(const void* first, const void* second);
	static int qsort_id_index_entry_cmp(const void* first, const void* second);

	void open_map_file(const char* file_name) throw (HarmonizerException);
	void close_map_file() throw (HarmonizerException);
	void process_map_file_header() throw (HarmonizerException);
	void process_map_file_data() throw (HarmonizerException);

	void harmonize_id_with_alleles();

	void harmonize_without_alleles(const char* output_file_name, bool gzip) throw (HarmonizerException);
	void harmonize_with_alleles(const char* output_file_name, bool drop, bool gzip) throw (HarmonizerException);

public:
	static const unsigned int MAP_HEAP_SIZE;
	static const unsigned int MAP_HEAP_INCREMENT;

	Harmonizer();
	virtual ~Harmonizer();

	void open_file(const char* file_name, const char* id_column_name, const char* ref_allele_column_name, const char* nonref_allele_column_name, char field_separator) throw (HarmonizerException);
	void close_file() throw (HarmonizerException);

	void process_header() throw (HarmonizerException);
	void index_map(const char* file_name) throw (HarmonizerException);
	void harmonize(const char* output_file_name, bool drop, bool gzip) throw (HarmonizerException);
};

#endif
