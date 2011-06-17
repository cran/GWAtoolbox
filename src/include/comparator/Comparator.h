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

/*
 * Author: Daniel Taliun
 * Date: 03-06-2010
 *
 * Description:
 * Class Comparator is the main class, that extracts top SNPs from different studies after the meta-analysis.
 *
 */

#ifndef COMPARATOR_H_
#define COMPARATOR_H_

#include <map>

#include "ComparatorException.h"
#include "../file/File.h"
#include "Data.h"

class Comparator {
private:
	static const int LINE_BUFFER_SIZE;
	static const int HEAP_SIZE;
	static const int HEAP_INCREMENT;

	char* line_buffer;
	char* cur;
	char* beg;

	void detect_separators(File* file) throw (ComparatorException);
	void check_separators(File* file) throw (ComparatorException, FileException);
	int sstrcmp(const char* first, const char* second);
	char* lstrtok(const char sep);
	static bool compare_chars (const char* first, const char* second);
	static int compare_double (const void* first, const void* second);

public:
	/* Columns */
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

	/* Separators */
	static const char* SEPARATOR;
	static const char* HEADER_SEPARATOR;
	static const char* DATA_SEPARATOR;
	static const char* WHITESPACE;
	static const char* COMMA;
	static const char* COMMAS;
	static const char* SEMICOLON;
	static const char* TAB;
	static const char* MISSED;
	static const int CHECK_ROW_COUNT;
	static const int SEPARATORS_SIZE;
	static const char separators[];

	/* Commands */
	static const char* PROCESS;
	static const char* ANALYZE;
	static const char* OUTFILE;

	/* Metal default values */
	static const char* METAL_OUTPUT;
	static const char* METAL_MARKER;
	static const char* METAL_ALLELE1;
	static const char* METAL_ALLELE2;
	static const char* METAL_PVALUE;
	static const char* METAL_EFFECT;
	static const char* METAL_STDERR;
	static const char* METAL_FREQLABEL;
	static const char* METAL_DIRECTION;
	static const char* METAL_ABBREVIATION;

	/* Other default values */
	static const char* CORRECTED_PVALUE;
	static const char* CORRECTED_STDERR;
	static const char* FILTERED_SNPS;

	/* Columns to compare */
	static const int COLUMNS_SIZE;
	static const char* columns[];

	Comparator() throw (ComparatorException);

	vector<File*>* process_script(const char* file_name) throw (ComparatorException, FileException);
	double get_lambda(File* file) throw (ComparatorException, FileException);
	map<char*, Data*, bool(*)(const char*, const char*)>* get_filtered_snps(File* file, double lambda, double pval_threshold, double maf_threshold) throw (ComparatorException, FileException);
	void add_corrected_p_values(File* file, double lambda, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException);
	void add_corrected_stderr(File* file, double lambda, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException);
	void add_effect_values(File* file, unsigned int seq_pos, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException);
	void add_freq_values(File* file, unsigned int seq_pos, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException);
	void add_values(File* file, const char* column_name, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException);

	void print_all_columns(File* file, const char* output_file_name, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException);
	void print_single_column(vector<File*>* files, const char* column_name, bool skip_metal, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException);

	void clear_values(map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps);

	virtual ~Comparator();
};

#endif /* COMPARATOR_H_ */
