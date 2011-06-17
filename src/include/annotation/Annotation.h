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
 * Date: 16-06-2010
 *
 * Description:
 * Class Annotation contains various methods for annotating SNPs to genes and vice versa.
 *
 */

#ifndef ANNOTATION_H_
#define ANNOTATION_H_

#include <fstream>
#include <sstream>
#include <map>
#include <set>

#include "Bin.h"
#include "Hotspot.h"
#include "IntervalTree.h"
#include "Snp.h"
#include "AnnotationException.h"

class Annotation {
private:
	static const int LINE_BUFFER_SIZE;

	static const char* SNP_FILE_SEPARATOR;
	static const char* LD_FILE_SEPARATOR;
	static const char* HOTSPOT_FILE_CHR_SEPARATOR;
	static const char* HOTSPOT_FILE_POS_SEPARATOR;
	static const char* GENE_FILE_SEPARATOR;

	static const char* GENE_START;
	static const char* GENE_END;
	static const char* GENE_NAME_1;
	static const char* GENE_NAME_2;
	static const char* GENE_NAME_3;
	static const char* CHROMOSOME_1;
	static const char* CHROMOSOME_2;
	static const char* MARKER_1;
	static const char* MARKER_2;
	static const char* MARKER_3;
	static const char* POSITION_1;
	static const char* POSITION_2;

	static const int SEPARATORS_SIZE;
	static const char separators[];
	static const char* separators_str[];
	static const int CHECK_ROW_COUNT;

	static const double LD;
	static const int EXTENTION;

	char* line_buffer;
	char* line_copy;

	static bool compare(const char* first, const char* second);
	static int compare_int (const void * first, const void * second);
	static bool compare_snps(Snp* first, Snp* second);
	static bool compare_hotspots(Hotspot* first, Hotspot* second);
	void output_char_vector(ostream &ostream, vector<char*>* values, const char* separator);
	void output_int_vector(ostream &ostream, vector<int>* values, const char* separator);
	char* lowercase(char* string);
	int detect_separator(const char* file_name, ifstream& ifile_stream) throw (AnnotationException);

public:
	Annotation() throw (AnnotationException);
	virtual ~Annotation();

	void snp_to_gene(const char* genes_file, const char* snps_file, const char* output_file, bool append, const char* delimiter, int* deviations, int deviations_count) throw (AnnotationException);
	void k_nearest_genes(const char* genes_file, const char* snps_file, const char* output_file, bool append, bool multicolumn, int k_max) throw (AnnotationException);
	void snp_to_gene_hs(const char* chromosome, const char* snps_file, const char* ld_file, const char* hotspots_file, const char* genes_file, const char* prefix) throw (AnnotationException);
	void gene_to_snp(const char* genes_file, const char* snps_file, const char* output_file, bool append, bool multicolumn) throw (AnnotationException);
	void gene_to_snp_by_name(const char* genes_file, const char* genesdb_file, const char* snps_file, const char* output_file, bool append, bool multicolumn) throw (AnnotationException);
};

#endif
