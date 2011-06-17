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

#include "../../include/annotation/Annotation.h"

const int Annotation::LINE_BUFFER_SIZE = 4000;

const char* Annotation::SNP_FILE_SEPARATOR = ",";
const char* Annotation::LD_FILE_SEPARATOR = " \t";
const char* Annotation::HOTSPOT_FILE_CHR_SEPARATOR = ":";
const char* Annotation::HOTSPOT_FILE_POS_SEPARATOR = "-";
const char* Annotation::GENE_FILE_SEPARATOR = "\t";

const char* Annotation::GENE_START = "start";
const char* Annotation::GENE_END = "end";
const char* Annotation::GENE_NAME_1 = "name";
const char* Annotation::GENE_NAME_2 = "associated gene name";
const char* Annotation::GENE_NAME_3 = "gene";
const char* Annotation::CHROMOSOME_1 = "chromosome";
const char* Annotation::CHROMOSOME_2 = "chr";
const char* Annotation::MARKER_1 = "marker";
const char* Annotation::MARKER_2 = "snp";
const char* Annotation::MARKER_3 = "markername";
const char* Annotation::POSITION_1 = "position";
const char* Annotation::POSITION_2 = "pos";

const int Annotation::SEPARATORS_SIZE = 4;
const char Annotation::separators[SEPARATORS_SIZE] = {
		' ', '\t', ',', ';'
};
const char* Annotation::separators_str[SEPARATORS_SIZE] = {
		" ", "\t", ",", ";"
};
const int Annotation::CHECK_ROW_COUNT = 3;

const double Annotation::LD = 0.5;
const int Annotation::EXTENTION = 250000;

Annotation::Annotation() throw (AnnotationException): line_buffer(NULL), line_copy(NULL) {
	line_buffer = (char*)malloc(LINE_BUFFER_SIZE * sizeof(char));
	if (line_buffer == NULL) {
		throw AnnotationException(0);
	}

	line_copy = (char*)malloc(LINE_BUFFER_SIZE * sizeof(char));
	if (line_copy == NULL) {
		throw AnnotationException(1);
	}
}

Annotation::~Annotation() {
	free(line_buffer);
	free(line_copy);
	line_buffer = NULL;
	line_copy = NULL;
}

bool Annotation::compare(const char* first, const char* second) {
	return strcmp(first, second) < 0;
}

int Annotation::compare_int (const void * first, const void * second) {
	return (*(int*)first - *(int*)second);
}

bool Annotation::compare_snps(Snp* first, Snp* second) {
	return strcmp(first->marker, second->marker) < 0;
}

bool Annotation::compare_hotspots(Hotspot* first, Hotspot* second) {
	return first->center < second->center;
}

void Annotation::output_char_vector(ostream &ostream, vector<char*>* values, const char* separator) {
	int i = 0, size = 0;

	if ((values != NULL) && ((size = values->size()) > 0)) {
		size -= 1;

		for (i = 0; i < size; i++) {
			ostream << values->at(i) << separator;
		}

		if (size >= 0) {
			ostream << values->at(size);
		}
	}
}

void Annotation::output_int_vector(ostream &ostream, vector<int>* values, const char* separator) {
	int i = 0, size = 0;

	if ((values != NULL) && ((size = values->size()) > 0)) {
		size -= 1;

		for (i = 0; i < size; i++) {
			ostream << values->at(i) << separator;
		}

		if (size >= 0) {
			ostream << values->at(size);
		}
	}
}

char* Annotation::lowercase(char* string) {
	if (string != NULL) {
		int length = strlen(string);

		for (int i = 0; i < length; i++) {
			string[i] = tolower(string[i]);
		}
	}

	return string;
}

int Annotation::detect_separator(const char* file_name, ifstream& ifile_stream) throw (AnnotationException) {
	int row_tokens[SEPARATORS_SIZE];
	int i = 0, length = 0, tokens = 0, row = 0;
	unsigned int char_pos  = 0;
	bool not_empty = false;
	int separator = -1;
	int matches = 0;

	i = 0;
	while (i < SEPARATORS_SIZE) {
		row_tokens[i] = 0;
		i += 1;
	}

	/* Read first row and initialize array with number of tokens for each separator. */
	try {
		ifile_stream.seekg((streampos)0, ifstream::beg);

		while ((!ifile_stream.eof()) && (not_empty != true)) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (length >= 0) {
				not_empty = true;

				i = 0;
				while (i < SEPARATORS_SIZE) {
					tokens = 0;
					char_pos = 0;
					while (char_pos < strlen(line_buffer)) {
						if (line_buffer[char_pos] == separators[i]) {
							tokens += 1;
						}
						char_pos += 1;
					}

					row_tokens[i] = tokens;
					i += 1;
				}
			}
		}
	}
	catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, file_name);
		}
	}

	/* Read next N rows and calculate number of tokens for each separator. */
	if (not_empty) {
		try {
			while ((!ifile_stream.eof()) && (row < CHECK_ROW_COUNT)) {
				ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

				length = ifile_stream.gcount() - 2;
				while ((length >= 0) && (line_buffer[length] == '\r')) {
					line_buffer[length] = '\0';
					length -= 1;
				}

				if (length >= 0) {
					i = 0;
					while (i < SEPARATORS_SIZE) {
						tokens = 0;
						char_pos = 0;
						while (char_pos < strlen(line_buffer)) {
							if (line_buffer[char_pos] == separators[i]) {
								tokens += 1;
							}
							char_pos += 1;
						}

						if ((row_tokens[i] != tokens) && (row_tokens[i] != INT_MIN)) {
							row_tokens[i] = INT_MIN;
						}
						i += 1;
					}

					row += 1;
				}
			}
		}
		catch (ifstream::failure &e) {
			if (!ifile_stream.eof()) {
				throw AnnotationException(4, file_name);
			}
		}

		i = 0;
		while (i < SEPARATORS_SIZE) {
			if (row_tokens[i] > 0) {
				separator = i;
				matches += 1;
			}
			i += 1;
		}

		if (matches != 1) {
			throw AnnotationException(30, file_name);
		}
	}
	else {
		throw AnnotationException(30, file_name);
	}

	try {
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, file_name);
		}
	}

	return separator;
}

/*
 *	Associates SNPs with genes.
 *	Arguments:
 *		genes_file		-	name of an input file with genes
 *		snps_file		-	name of an input file with SNPs
 *		output_file		-	name of an output file
 *		append			-	if true, write all columns to the result
 *		delimiter		-	delimiter for the names of genes, that are within the same deviation
 *		deviations		-	vector with integer deviation values
 */
void Annotation::snp_to_gene(const char* genes_file, const char* snps_file, const char* output_file, bool append, const char* delimiter, int* deviations, int deviations_count) throw (AnnotationException) {
	int separator = 0;
	int common_separator = 0;
	char* token = NULL;
	char* end_ptr = NULL;
	unsigned int file_column = 0;
	char* columns[4] = {NULL, NULL, NULL, NULL};
	int length = 0;
	int index = 0;

	int gene_start = 0, gene_end = 0;
	char* gene_name = NULL;
	char* chromosome =NULL;
	char* marker = NULL;
	int position = 0;

	unsigned int gene_start_column = UINT_MAX;
	unsigned int gene_end_column = UINT_MAX;
	unsigned int gene_name_column = UINT_MAX;
	unsigned int chromosome_column = UINT_MAX;
	unsigned int marker_column = UINT_MAX;
	unsigned int position_column = UINT_MAX;

	map<char*, IntervalTree<char*>*, bool(*)(const char*, const char*)> chromosomes(Annotation::compare);
	map<char*, IntervalTree<char*>*, bool(*)(const char*, const char*)>::iterator chromosomes_it;

	IntervalTree<char*>* genes = NULL;
	IntervalTree<char*> result;
	map<int, vector<char*>*> marked;
	map<int, vector<char*>*>::iterator marked_it;

	ifstream ifile_stream;
	ofstream ofile_stream;

	ifile_stream.exceptions(ios_base::failbit | ios_base::badbit);
	ifile_stream.setf(ios_base::skipws);
	ofile_stream.exceptions(ios_base::failbit | ios_base::badbit);

	qsort(deviations, deviations_count, sizeof(int), Annotation::compare_int);

	try {
		ifile_stream.open(genes_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, genes_file);
	}

	separator = detect_separator(genes_file, ifile_stream);
	if (separator == -1) {
		throw AnnotationException(30, genes_file);
	}

	try {
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if (strcmp(token, GENE_START) == 0) {
					gene_start_column = file_column;
				}
				else if (strcmp(token, GENE_END) == 0) {
					gene_end_column =  file_column;
				}
				else if ((strcmp(token, GENE_NAME_1) == 0) ||
						(strcmp(token, GENE_NAME_2) == 0) ||
						(strcmp(token, GENE_NAME_3) == 0)) {
					gene_name_column = file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, genes_file);
		}
		else if (gene_name_column == UINT_MAX) {
			throw AnnotationException(8, genes_file);
		}
		else if (gene_start_column == UINT_MAX) {
			throw AnnotationException(9, genes_file);
		}
		else if (gene_end_column == UINT_MAX) {
			throw AnnotationException(10, genes_file);
		}


		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = columns[3] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == gene_start_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(15, genes_file);
					}
					columns[0] = token;
				}
				else if (file_column == gene_end_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(16, genes_file);
					}
					columns[1] = token;
				}
				else if (file_column == gene_name_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(14, genes_file);
					}
					columns[2] = token;
				}
				else if (file_column == chromosome_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, genes_file);
					}
					columns[3] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL) && (columns[3] != NULL)) {
				gene_start = strtol(columns[0], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(20, genes_file);
				}

				gene_end = strtol(columns[1], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(21, genes_file);
				}

				gene_name = (char*)malloc((strlen(columns[2]) + 1) * sizeof(char));
				if (gene_name == NULL) {
					throw AnnotationException(27, genes_file);
				}
				strcpy(gene_name, columns[2]);

				chromosomes_it = chromosomes.find(columns[3]);
				if (chromosomes_it == chromosomes.end()) {
					genes = new IntervalTree<char*>();

					chromosome = (char*)malloc((strlen(columns[3]) + 1) * sizeof(char));
					if (chromosome == NULL) {
						throw AnnotationException(28, genes_file);
					}
					strcpy(chromosome, columns[3]);

					chromosomes.insert(pair<char*, IntervalTree<char*>*>(chromosome, genes));
				}
				else {
					genes = (*chromosomes_it).second;
				}

				genes->add(gene_start, gene_end, gene_name);
			}
			else {
				throw AnnotationException(6, genes_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, genes_file);
		}
	}

	try {
		ifile_stream.close();
		ifile_stream.clear();
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(3, genes_file);
	}

	try {
		ifile_stream.open(snps_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
		ofile_stream.open(output_file);
	}
	catch (ios_base::failure &e) {
		if (ifile_stream.fail()) {
			throw AnnotationException(2, snps_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(2, output_file);
		}
	}

	separator = detect_separator(snps_file, ifile_stream);
	if (separator == -1) {
		throw AnnotationException(30, snps_file);
	}

	if (append) {
		common_separator = separator;
	}
	else {
		common_separator = 1;
	}

	try {
		chromosome_column = UINT_MAX;

		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (append) {
				strcpy(line_copy, line_buffer);
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if ((strcmp(token, MARKER_1) == 0) ||
					(strcmp(token, MARKER_2) == 0) ||
					(strcmp(token, MARKER_3) == 0)) {
					marker_column = file_column;
				}
				else if ((strcmp(token, POSITION_1) == 0) ||
						(strcmp(token, POSITION_2) == 0)) {
					position_column =  file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, snps_file);
		}
		else if (marker_column == UINT_MAX) {
			throw AnnotationException(11, snps_file);
		}
		else if (position_column == UINT_MAX) {
			throw AnnotationException(12, snps_file);
		}

		if (append) {
			ofile_stream << line_copy;
		}
		else {
			ofile_stream << "SNP" << separators_str[common_separator] << "Chromosome" << separators_str[common_separator] << "Position";
		}
		for (int i = 0; i < deviations_count; i++) {
			if (deviations[i] != 0) {
				ofile_stream << separators_str[common_separator] << "+/-" << deviations[i];
			}
			else {
				ofile_stream << separators_str[common_separator] << "IN";
			}
		}
		ofile_stream << endl;

		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (append) {
				strcpy(line_copy, line_buffer);
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == marker_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(17, snps_file);
					}
					columns[0] = token;
				}
				else if (file_column == position_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(18, snps_file);
					}
					columns[1] = token;
				}
				else if (file_column == chromosome_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, snps_file);
					}
					columns[2] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL)) {
				position = strtol(columns[1], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(22, snps_file);
				}

				marker = columns[0];
				chromosome = columns[2];

				chromosomes_it = chromosomes.find(chromosome);
				if (chromosomes_it != chromosomes.end()) {
					genes = chromosomes_it->second;

					result.clear();
					index = deviations_count - 1;
					genes->get_intersecting_intervals(position - deviations[index], position + deviations[index], deviations[index], result);
					for (int i = index - 1; i >= 0; i--) {
						result.mark_intersecting_intervals(position - deviations[i], position + deviations[i], deviations[i]);
					}

					result.get_marked_values(marked);

					if (append) {
						ofile_stream << line_copy << separators_str[common_separator];
					}
					else {
						ofile_stream << marker << separators_str[common_separator] << chromosome << separators_str[common_separator] << position << separators_str[common_separator];
					}
					if (marked.size() > 0) {
						for(int i = 0; i < index; i++) {
							marked_it = marked.find(deviations[i]);
							if (marked_it != marked.end()) {
								output_char_vector(ofile_stream, marked_it->second, delimiter);
							}
							ofile_stream << separators_str[common_separator];
						}

						marked_it = marked.find(deviations[index]);
						if (marked_it != marked.end()) {
							output_char_vector(ofile_stream, marked_it->second, delimiter);
						}
						ofile_stream << endl;
					}
					else {
						for (int i = 0; i < index; i++) {
							ofile_stream << separators_str[common_separator];
						}
						ofile_stream << endl;
					}

					for (marked_it = marked.begin(); marked_it != marked.end(); marked_it++) {
						delete marked_it->second;
					}
					marked.clear();
				}
			}
			else {
				throw AnnotationException(6, snps_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, snps_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(5, output_file);
		}
	}

	try {
		ifile_stream.close();
		ofile_stream.close();
	}
	catch (ios_base::failure &e) {
		if (ifile_stream.fail()) {
			throw AnnotationException(3, snps_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(3, output_file);
		}
	}

	result.clear();
	for (chromosomes_it = chromosomes.begin(); chromosomes_it != chromosomes.end(); chromosomes_it++) {
		free(chromosomes_it->first);
		delete chromosomes_it->second;
	}
	chromosomes.clear();
}

/*
 *	Finds k nearest genes to an SNP.
 *	Arguments:
 *		genes_file		-	name of an input file with genes
 *		snps_file		-	name of an input file with SNPs
 *		output_file		-	name of an output file
 *		append			- 	if true, then write all column to the result
 *		multicolumn		-	the way of output
 *		k_max			-	max number of nearest genes to report
 */
void Annotation::k_nearest_genes(const char* genes_file, const char* snps_file, const char* output_file, bool append, bool multicolumn, int k_max) throw (AnnotationException) {
	int separator = 0;
	int common_separator = 0;
	char* token = NULL;
	char* end_ptr = NULL;
	unsigned int file_column = 0;
	char* columns[4] = {NULL, NULL, NULL, NULL};
	int length = 0;

	int gene_start = 0, gene_end = 0;
	char* gene_name = NULL;
	char* chromosome = NULL;
	char* marker = NULL;
	int position = 0;

	unsigned int gene_start_column = UINT_MAX;
	unsigned int gene_end_column = UINT_MAX;
	unsigned int gene_name_column = UINT_MAX;
	unsigned int chromosome_column = UINT_MAX;
	unsigned int marker_column = UINT_MAX;
	unsigned int position_column = UINT_MAX;

	map<char*, pair<IntervalTree<char*>*, IntervalTree<char*>*>, bool(*)(const char*, const char*)> chromosomes(compare);
	map<char*, pair<IntervalTree<char*>*, IntervalTree<char*>*>, bool(*)(const char*, const char*)>::iterator chromosomes_it;

	IntervalTree<char*>* genes = NULL;
	IntervalTree<char*>* reverse_genes = NULL;

	vector<char*> in;
	vector<Interval<char*>*> l_nearest;
	vector<Interval<char*>*> r_nearest;

	vector<char*>::iterator in_it;
	vector<Interval<char*>*>::iterator l_nearest_it;
	vector<Interval<char*>*>::iterator r_nearest_it;

	Interval<char*>* l_interval = NULL;
	Interval<char*>* r_interval = NULL;

	int l_distance = 0;
	int r_distance = 0;
	int k = 0;

	ifstream ifile_stream;
	ofstream ofile_stream;

	ifile_stream.exceptions(ios_base::failbit | ios_base::badbit);
	ifile_stream.setf(ios_base::skipws);
	ofile_stream.exceptions(ios_base::failbit | ios_base::badbit);

	try {
		ifile_stream.open(genes_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, genes_file);
	}

	separator = detect_separator(genes_file, ifile_stream);
	if (separator == -1) {
		throw AnnotationException(30, genes_file);
	}

	try {
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if (strcmp(token, GENE_START) == 0) {
					gene_start_column = file_column;
				}
				else if (strcmp(token, GENE_END) == 0) {
					gene_end_column =  file_column;
				}
				else if ((strcmp(token, GENE_NAME_1) == 0) ||
						(strcmp(token, GENE_NAME_2) == 0) ||
						(strcmp(token, GENE_NAME_3) == 0)){
					gene_name_column = file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)){
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, genes_file);
		}
		else if (gene_name_column == UINT_MAX) {
			throw AnnotationException(8, genes_file);
		}
		else if (gene_start_column == UINT_MAX) {
			throw AnnotationException(9, genes_file);
		}
		else if (gene_end_column == UINT_MAX) {
			throw AnnotationException(10, genes_file);
		}

		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = columns[3] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == gene_start_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(15, genes_file);
					}
					columns[0] = token;
				}
				else if (file_column == gene_end_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(16, genes_file);
					}
					columns[1] = token;
				}
				else if (file_column == gene_name_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(14, genes_file);
					}
					columns[2] = token;
				}
				else if (file_column == chromosome_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, genes_file);
					}
					columns[3] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL) && (columns[3] != NULL)) {
				gene_start = strtol(columns[0], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(20, genes_file);
				}

				gene_end = strtol(columns[1], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(21, genes_file);
				}

				gene_name = (char*)malloc((strlen(columns[2]) + 1) * sizeof(char));
				if (gene_name == NULL) {
					throw AnnotationException(27, genes_file);
				}
				strcpy(gene_name, columns[2]);

				chromosomes_it = chromosomes.find(columns[3]);

				if (chromosomes_it != chromosomes.end()) {
					genes = chromosomes_it->second.first;
					reverse_genes = chromosomes_it->second.second;
				}
				else {
					genes = new IntervalTree<char*>();
					reverse_genes = new IntervalTree<char*>();

					chromosome = (char*)malloc((strlen(columns[3]) + 1) * sizeof(char));
					if (chromosome == NULL) {
						throw AnnotationException(28, genes_file);
					}
					strcpy(chromosome, columns[3]);

					chromosomes.insert(pair<char*, pair<IntervalTree<char*>*, IntervalTree<char*>*> >(chromosome, pair<IntervalTree<char*>*, IntervalTree<char*>*>(genes, reverse_genes)));
				}

				genes->add(gene_start, gene_end, gene_name);
				reverse_genes->add(gene_end, gene_start, gene_name);
			}
			else {
				throw AnnotationException(6, genes_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, genes_file);
		}
	}

	try {
		ifile_stream.close();
		ifile_stream.clear();
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(3, genes_file);
	}

	try {
		ifile_stream.open(snps_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
		ofile_stream.open(output_file);
	}
	catch (ios_base::failure &e) {
		if (ifile_stream.fail()) {
			throw AnnotationException(2, snps_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(2, output_file);
		}
	}

	separator = detect_separator(snps_file, ifile_stream);
	if (separator == -1) {
		throw AnnotationException(30, snps_file);
	}

	if (append) {
		common_separator = separator;
	}
	else {
		common_separator = 1;
	}

	try {
		chromosome_column = UINT_MAX;

		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (append) {
				strcpy(line_copy, line_buffer);
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if ((strcmp(token, MARKER_1) == 0) ||
					(strcmp(token, MARKER_2) == 0) ||
					(strcmp(token, MARKER_3) == 0)) {
					marker_column = file_column;
				}
				else if ((strcmp(token, POSITION_1) == 0) ||
						(strcmp(token, POSITION_2) == 0)) {
					position_column =  file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, snps_file);
		}
		else if (marker_column == UINT_MAX) {
			throw AnnotationException(11, snps_file);
		}
		else if (position_column == UINT_MAX) {
			throw AnnotationException(12, snps_file);
		}

		if (multicolumn) {
			/**************************************************/
			if (append) {
				ofile_stream << line_copy;
			}
			else {
				ofile_stream << "SNP" << separators_str[common_separator] << "Chromosome" << separators_str[common_separator] << "Position";
			}
			ofile_stream << separators_str[common_separator];

			for (k = 1; k < k_max; k++) {
				ofile_stream << "Gene " << k << separators_str[common_separator]<< "Distance " << k << separators_str[common_separator];
			}
			ofile_stream << "Gene " << k << separators_str[common_separator]<< "Distance " << k << endl;
			/**************************************************/
		}
		else {
			if (append) {
				ofile_stream << line_copy;
			}
			else {
				ofile_stream << "SNP" << separators_str[common_separator] << "Chromosome" << separators_str[common_separator] << "Position";
			}
			ofile_stream << separators_str[common_separator] << "Gene" << separators_str[common_separator] << "Distance" << endl;
		}

		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (append) {
				strcpy(line_copy, line_buffer);
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == marker_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(17, snps_file);
					}
					columns[0] = token;
				}
				else if (file_column == position_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(18, snps_file);
					}
					columns[1] = token;
				}
				else if (file_column == chromosome_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, snps_file);
					}
					columns[2] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL)) {
				position = strtol(columns[1], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(22, snps_file);
				}

				marker = columns[0];
				chromosome = columns[2];

				chromosomes_it = chromosomes.find(chromosome);
				if (chromosomes_it != chromosomes.end()) {
					genes = chromosomes_it->second.first;
					reverse_genes = chromosomes_it->second.second;

					in.clear();
					genes->get_intersecting_intervals(position, in);

					/******************************************************************/
					vector<char*> names;
					vector<int> positions;

					for (in_it = in.begin(); in_it != in.end(); in_it++) {
						names.push_back(*in_it);
						positions.push_back(0);
					}
					/******************************************************************/

					if ((k = k_max - in.size()) > 0) {
						l_nearest.clear();
						r_nearest.clear();

						genes->get_knearest_from_right(position, k, r_nearest);
						reverse_genes->get_knearest_from_left(position, k, l_nearest);

						l_nearest_it = l_nearest.begin();
						r_nearest_it = r_nearest.begin();
						l_interval = NULL;
						r_interval = NULL;

						while (k > 0) {
							if ((l_interval == NULL) && (l_nearest_it != l_nearest.end())) {
								l_interval = *l_nearest_it;
								l_nearest_it++;
							}

							if ((r_interval == NULL) && (r_nearest_it != r_nearest.end())) {
								r_interval = *r_nearest_it;
								r_nearest_it++;
							}

							if (l_interval == NULL) {
								if (r_interval == NULL) {
									break;
								}

								/*********************************************************/
								for (in_it = r_interval->values.begin(); in_it != r_interval->values.end(); in_it++) {
									names.push_back(*in_it);
									positions.push_back(r_interval->start - position);
								}
								/*********************************************************/

								r_interval = NULL;
							}
							else if (r_interval == NULL) {

								/*********************************************************/
								for (in_it = l_interval->values.begin(); in_it != l_interval->values.end(); in_it++) {
									names.push_back(*in_it);
									positions.push_back(position - l_interval->start);
								}
								/*********************************************************/

								l_interval = NULL;
							}
							else {
								l_distance = position - l_interval->start;
								r_distance = r_interval->start - position;

								if (l_distance == r_distance) {

									/*******************************************************/
									for (in_it = l_interval->values.begin(); in_it != l_interval->values.end(); in_it++) {
										names.push_back(*in_it);
										positions.push_back(l_distance);
									}
									for (in_it = r_interval->values.begin(); in_it != r_interval->values.end(); in_it++) {
										names.push_back(*in_it);
										positions.push_back(r_distance);
									}
									/*******************************************************/
									l_interval = NULL;
									r_interval = NULL;
								}
								else if (l_distance < r_distance) {

									/*******************************************************/
									for (in_it = l_interval->values.begin(); in_it != l_interval->values.end(); in_it++) {
										names.push_back(*in_it);
										positions.push_back(l_distance);
									}
									/*******************************************************/

									l_interval = NULL;
								}
								else {

									/*******************************************************/
									for (in_it = r_interval->values.begin(); in_it != r_interval->values.end(); in_it++) {
										names.push_back(*in_it);
										positions.push_back(r_distance);
									}
									/*******************************************************/

									r_interval = NULL;
								}
							}

							k -= 1;
						}
					}

					/*************************************************/
					int size = names.size();
					if (multicolumn) {
						if (append) {
							ofile_stream << line_copy;
						}
						else {
							ofile_stream << marker << separators_str[common_separator] << chromosome << separators_str[common_separator] << position;
						}
						ofile_stream << separators_str[common_separator];

						for (k = 0; k < k_max - 1; k++) {
							if (k < size) {
								ofile_stream << names.at(k) << separators_str[common_separator] << positions.at(k) << separators_str[common_separator];
							}
							else {
								ofile_stream << separators_str[common_separator] << separators_str[common_separator];
							}
						}
						if (k < size) {
							ofile_stream << names.at(k) << separators_str[common_separator] << positions.at(k) << endl;
						}
						else {
							ofile_stream << separators_str[common_separator] << endl;
						}
					}
					else {
						if (append) {
							for (k = 0; k < size; k++) {
								ofile_stream << line_copy;
								ofile_stream << separators_str[common_separator] << names.at(k) << separators_str[common_separator] << positions.at(k) << endl;
							}
						}
						else {
							for (k = 0; k < size; k++) {
								ofile_stream << marker << separators_str[common_separator] << chromosome << separators_str[common_separator] << position;
								ofile_stream << separators_str[common_separator] << names.at(k) << separators_str[common_separator] << positions.at(k) << endl;
							}
						}
					}
					/*************************************************/
				}
			}
			else {
				throw AnnotationException(6, snps_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, snps_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(5, output_file);
		}
	}

	try {
		ifile_stream.close();
		ofile_stream.close();
	}
	catch (ios_base::failure &e) {
		if (ifile_stream.fail()) {
			throw AnnotationException(3, snps_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(3, output_file);
		}
	}

	for (chromosomes_it = chromosomes.begin(); chromosomes_it != chromosomes.end(); chromosomes_it++) {
		free(chromosomes_it->first);
		chromosomes_it->second.first->clear();
		delete chromosomes_it->second.second;
	}
	chromosomes.clear();
}

void Annotation::snp_to_gene_hs(const char* chromosome, const char* snps_file, const char* ld_file, const char* hotspots_file, const char* genes_file, const char* prefix) throw (AnnotationException) {
	/* Files' reading/writing */
	ifstream ifile_stream;
	ofstream ofile_stream_bins_pos;
	ofstream ofile_stream_bins_snp;
	ofstream ofile_stream_bins_gene;
	stringstream string_stream;
	char* token = NULL;
	unsigned int file_column = 0;
	char* columns[] = {NULL, NULL, NULL, NULL};
	char* end_ptr = NULL;
	int length = 0;

	/* Data */
	double r2_value = 0.0;
	int position = 0;

	map<Snp*, vector<Snp*>*, bool(*)(Snp*, Snp*)> ld(compare_snps);
	map<Snp*, vector<Snp*>*, bool(*)(Snp*, Snp*)>::iterator ld_it;

	vector<Snp*>* neighbours = NULL;
	vector<Snp*>* first_neighbours = NULL;
	vector<Snp*>* second_neighbours = NULL;
	vector<Snp*>::iterator neighbours_it;

	Snp* snp = NULL;
	Snp* first_snp = NULL;
	Snp* second_snp = NULL;

	vector<Bin*> bins;
	vector<Bin*>::iterator bins_it;
	Bin* bin = NULL;

	set<Hotspot*, bool(*)(Hotspot*, Hotspot*)> hotspots(Annotation::compare_hotspots);
	set<Hotspot*, bool(*)(Hotspot*, Hotspot*)>::iterator hotspots_it;
	Hotspot* hotspot = NULL;

	vector<char*>::iterator names_it;

	unsigned int gene_start_column = UINT_MAX;
	unsigned int gene_end_column = UINT_MAX;
	unsigned int gene_name_column = UINT_MAX;
	unsigned int chr_column = UINT_MAX;
	unsigned int marker_column = UINT_MAX;
	unsigned int position_column = UINT_MAX;

	IntervalTree<char*> genes;
	int gene_start = 0, gene_end = 0;
	char* gene_name = NULL;

	/* Set input/output file stream properties */
	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);
	ofile_stream_bins_pos.exceptions(ofstream::failbit | ofstream::badbit);
	ofile_stream_bins_snp.exceptions(ofstream::failbit | ofstream::badbit);
	ofile_stream_bins_gene.exceptions(ofstream::failbit | ofstream::badbit);

	/* Read LD file and prepare all possible bins */
	try {
		ifile_stream.open(ld_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(2, ld_file);
	}

	snp = new Snp();

	try {
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = NULL;
			token = strtok(line_buffer, LD_FILE_SEPARATOR);
			while (token != NULL) {
				switch (file_column) {
					case 0:
					case 1:
					case 2:
						if (strlen(token) <= 0) {
							throw AnnotationException(19, ld_file);
						}
						columns[file_column] = token;
						break;
					default:
						break;
				}

				file_column += 1;
				token = strtok(NULL, LD_FILE_SEPARATOR);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL)) {
				r2_value = strtod(columns[2], &end_ptr);
				if (*end_ptr != '\0') {
					throw AnnotationException(23, ld_file);
				}

				if (r2_value > LD) {
					snp->marker = columns[0];
					ld_it = ld.find(snp);
					if (ld_it != ld.end()) {
						first_neighbours = ld_it->second;
						first_snp = ld_it->first;
					}
					else {
						first_neighbours = new vector<Snp*>();
						first_snp = new Snp();
						first_snp->set_marker(columns[0]);
						ld.insert(pair<Snp*, vector<Snp*>*>(first_snp, first_neighbours));
					}

					snp->marker = columns[1];
					ld_it = ld.find(snp);
					if (ld_it != ld.end()) {
						second_neighbours = ld_it->second;
						second_snp = ld_it->first;
					}
					else {
						second_neighbours = new vector<Snp*>();
						second_snp = new Snp();
						second_snp->set_marker(columns[1]);
						ld.insert(pair<Snp*, vector<Snp*>*>(second_snp, second_neighbours));
					}

					first_neighbours->push_back(second_snp);
					second_neighbours->push_back(first_snp);
				}
			}
			else {
				throw AnnotationException(6, ld_file);
			}
		}
	}
	catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, ld_file);
		}
	}

	delete snp;
	snp = NULL;

	try {
		ifile_stream.close();
		ifile_stream.clear();
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(3, ld_file);
	}

	try {
		ifile_stream.open(snps_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(2, snps_file);
	}

	snp = new Snp();

	try {
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, SNP_FILE_SEPARATOR);
			while (token != NULL) {
				token = lowercase(token);

				if ((strcmp(token, CHROMOSOME_1) == 0) ||
					(strcmp(token, CHROMOSOME_2) == 0)) {
					chr_column = file_column;
				}
				else if ((strcmp(token, MARKER_1) == 0) ||
						(strcmp(token, MARKER_2) == 0) ||
						(strcmp(token, MARKER_3) == 0)) {
					marker_column =  file_column;
				}
				else if ((strcmp(token, POSITION_1) == 0) ||
						(strcmp(token, POSITION_2) == 0)) {
					position_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, SNP_FILE_SEPARATOR);
			}
		}

		if (chr_column == UINT_MAX) {
			throw AnnotationException(7, snps_file);
		}
		else if (marker_column == UINT_MAX) {
			throw AnnotationException(11, snps_file);
		}
		else if (position_column == UINT_MAX) {
			throw AnnotationException(12, snps_file);
		}

		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			if ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = NULL;
			token = strtok(line_buffer, SNP_FILE_SEPARATOR);
			while (token != NULL) {
				if (file_column == marker_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(17, snps_file);
					}
					columns[0] = token;
				}
				else if (file_column == chr_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, snps_file);
					}
					columns[1] = token;
				}
				else if (file_column == position_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(18, snps_file);
					}
					columns[2] = token;
				}

				file_column += 1;
				token = strtok(NULL, SNP_FILE_SEPARATOR);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL)) {
				if (strcmp(chromosome, columns[1]) == 0) {
					position = strtol(columns[2], &end_ptr, 10);
					if (*end_ptr != '\0') {
						throw AnnotationException(22, snps_file);
					}

					snp->marker = columns[0];
					ld_it = ld.find(snp);
					if (ld_it != ld.end()) {
						ld_it->first->position = position;
						if (!ld_it->first->used) {
							ld_it->first->used = true;
							neighbours = ld_it->second;
							for (neighbours_it = neighbours->begin(); neighbours_it != neighbours->end(); neighbours_it++) {
								(*neighbours_it)->used = true;
							}
							ld_it->first->prime = true;
						}
					}
				}
			}
			else {
				throw AnnotationException(6, snps_file);
			}
		}
	}
	catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, snps_file);
		}
	}

	delete snp;
	snp = NULL;

	try {
		ifile_stream.close();
		ifile_stream.clear();
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(3, snps_file);
	}

	for (ld_it = ld.begin(); ld_it != ld.end(); ld_it++) {
		neighbours = ld_it->second;

		if (ld_it->first->prime) {
			bin = new Bin();
			bin->set_snp(ld_it->first);

			for (neighbours_it = neighbours->begin(); neighbours_it != neighbours->end(); neighbours_it++) {
				if ((*neighbours_it)->position > 0) {
					bin->set_snp(*neighbours_it);
				}
			}

			bins.push_back(bin);
		}

		neighbours->clear();
	}

	int deleted_snp_names = 0;

	for (ld_it = ld.begin(); ld_it != ld.end(); ld_it++) {
		if ((!ld_it->first->used) || (ld_it->first->position <= 0)) {
			free(ld_it->first->marker);
			deleted_snp_names += 1;
			ld_it->first->marker = NULL;
		}
		ld_it->second->clear();

		delete ld_it->first;
		delete ld_it->second;
	}
	ld.clear();

	try {
		ifile_stream.open(hotspots_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(2, hotspots_file);
	}

	try {
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = NULL;
			token = strtok(line_buffer, HOTSPOT_FILE_CHR_SEPARATOR);
			while (token != NULL) {
				switch (file_column) {
					case 0:
					case 1:
					case 2:
						if (strlen(token) <= 0) {
							throw AnnotationException(19, hotspots_file);
						}
						columns[file_column] = token;
						break;
					default:
						break;
				}

				file_column += 1;
				token = strtok(NULL, HOTSPOT_FILE_POS_SEPARATOR);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL)) {
				if (strstr(columns[0], "chr") == columns[0]) {
					columns[0] = columns[0] + 3;
				}

				if (strcmp(chromosome, columns[0]) == 0) {
					hotspot = new Hotspot();

					hotspot->start = strtol(columns[1], &end_ptr, 10);
					if (*end_ptr != '\0') {
						throw AnnotationException(24, hotspots_file);
					}

					hotspot->end = strtol(columns[2], &end_ptr, 10);
					if (*end_ptr != '\0') {
						throw AnnotationException(25, hotspots_file);
					}

					hotspot->center = hotspot->start + (hotspot->end - hotspot->start) / 2;

					if (hotspot->start > hotspot->center) {
						throw AnnotationException(26, hotspots_file);
					}

					hotspots.insert(hotspot);
				}
			}
			else {
				throw AnnotationException(6, hotspots_file);
			}
		}
	}
	catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, hotspots_file);
		}
	}

	try {
		ifile_stream.close();
		ifile_stream.clear();
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(3, hotspots_file);
	}

	hotspot = new Hotspot();

	for (bins_it = bins.begin(); bins_it != bins.end(); bins_it++) {
		bin = *bins_it;

		hotspot->center = bin->start;
		hotspots_it = hotspots.lower_bound(hotspot);
		if (hotspots_it != hotspots.begin()) {
			hotspots_it--;
			bin->start = (*hotspots_it)->center;
		}

		hotspot->center = bin->end;
		hotspots_it = hotspots.upper_bound(hotspot);
		if (hotspots_it != hotspots.end()) {
			bin->end = (*hotspots_it)->center;
		}
	}

	delete hotspot;
	hotspot = NULL;

	try {
		ifile_stream.open(genes_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(2, genes_file);
	}

	try {
		chr_column = UINT_MAX;

		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, GENE_FILE_SEPARATOR);
			while (token != NULL) {
				token = lowercase(token);

				if (strcmp(token, GENE_START) == 0) {
					gene_start_column = file_column;
				}
				else if (strcmp(token, GENE_END) == 0) {
					gene_end_column =  file_column;
				}
				else if ((strcmp(token, GENE_NAME_1) == 0) ||
						(strcmp(token, GENE_NAME_2) == 0) ||
						(strcmp(token, GENE_NAME_3) == 0)){
					gene_name_column = file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chr_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, GENE_FILE_SEPARATOR);
			}
		}

		if (chr_column == UINT_MAX) {
			throw AnnotationException(7, genes_file);
		}
		else if (gene_name_column == UINT_MAX) {
			throw AnnotationException(8, genes_file);
		}
		else if (gene_start_column == UINT_MAX) {
			throw AnnotationException(9, genes_file);
		}
		else if (gene_end_column == UINT_MAX) {
			throw AnnotationException(10, genes_file);
		}

		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = columns[3] = NULL;
			token = strtok(line_buffer, GENE_FILE_SEPARATOR);
			while (token != NULL) {
				if (file_column == gene_start_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(15, genes_file);
					}
					columns[0] = token;
				}
				else if (file_column == gene_end_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(16, genes_file);
					}
					columns[1] = token;
				}
				else if (file_column == gene_name_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(14, genes_file);
					}
					columns[3] = token;
				}
				else if (file_column == chr_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, genes_file);
					}
					columns[2] = token;
				}

				file_column += 1;
				token = strtok(NULL, GENE_FILE_SEPARATOR);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL) && (columns[3] != NULL)) {
				if (strcmp(chromosome, columns[2]) == 0) {
					gene_start = strtol(columns[0], &end_ptr, 10);
					if (*end_ptr != '\0') {
						throw AnnotationException(20, genes_file);
					}

					gene_end = strtol(columns[1], &end_ptr, 10);
					if (*end_ptr != '\0') {
						throw AnnotationException(21, genes_file);
					}

					if ((gene_name = (char*)malloc((strlen(columns[3]) + 1) * sizeof(char))) == NULL) {
						throw AnnotationException(27, genes_file);
					}
					strcpy(gene_name, columns[3]);
					genes.add(gene_start, gene_end, gene_name);
				}
			}
			else {
				throw AnnotationException(6, genes_file);
			}
		}
	}
	catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, genes_file);
		}
	}

	try {
		ifile_stream.close();
		ifile_stream.clear();
	}
	catch (ifstream::failure &e) {
		throw AnnotationException(3, genes_file);
	}

	for (bins_it = bins.begin(); bins_it != bins.end(); bins_it++) {
		bin = *bins_it;
		genes.get_intersecting_intervals(bin->start, bin->end, bin->genes);
		if (bin->genes.size() <= 0) {
			bin->start -= EXTENTION;
			bin->end += EXTENTION;
			genes.get_intersecting_intervals(bin->start, bin->end, bin->genes);
		}
	}

	try {
		string_stream << prefix << "_bins2intervals.txt";
		ofile_stream_bins_pos.open(string_stream.str().c_str());

		string_stream.str("");
		string_stream.clear();
		string_stream << prefix << "_bins2snps.txt";
		ofile_stream_bins_snp.open(string_stream.str().c_str());

		string_stream.str("");
		string_stream.clear();
		string_stream << prefix << "_bins2genes.txt";
		ofile_stream_bins_gene.open(string_stream.str().c_str());
	}
	catch (ofstream::failure &e) {
		throw AnnotationException(2, e.what());
	}

	int bin_id = 0;

	try {
		ofile_stream_bins_pos << "Bin\tStart\tEnd" << endl;
		ofile_stream_bins_snp << "Bin\tSNP" << endl;
		ofile_stream_bins_gene << "Bin\tGene" << endl;

		for (bins_it = bins.begin(); bins_it != bins.end(); bins_it++) {
			bin_id += 1;

			ofile_stream_bins_pos << bin_id << "\t" << (*bins_it)->start << "\t" << (*bins_it)->end << endl;

			for (names_it = (*bins_it)->snps.begin(); names_it != (*bins_it)->snps.end(); names_it++) {
				ofile_stream_bins_snp << bin_id << "\t" << *names_it << endl;
			}

			for (names_it = (*bins_it)->genes.begin(); names_it != (*bins_it)->genes.end(); names_it++) {
				ofile_stream_bins_gene << bin_id << "\t" << *names_it << endl;
			}
		}
	}
	catch (ofstream::failure &e) {
		throw AnnotationException(5, e.what());
	}

	try {
		ofile_stream_bins_pos.close();
		ofile_stream_bins_pos.clear();
		ofile_stream_bins_snp.close();
		ofile_stream_bins_snp.clear();
		ofile_stream_bins_gene.close();
		ofile_stream_bins_gene.clear();
	}
	catch (ofstream::failure &e) {
		throw AnnotationException(3, e.what());
	}

	for (bins_it = bins.begin(); bins_it != bins.end(); bins_it++) {
		delete *bins_it;
	}
	bins.clear();

	for (hotspots_it = hotspots.begin(); hotspots_it != hotspots.end(); hotspots_it++) {
		delete *hotspots_it;
	}
	hotspots.clear();
}

void Annotation::gene_to_snp(const char* genes_file, const char* snps_file, const char* output_file, bool append, bool multicolumn) throw (AnnotationException) {
	int separator = 0;
	int common_separator = 0;
	char* token = NULL;
	char* end_ptr = NULL;
	unsigned int file_column = 0;
	char* columns[4] = {NULL, NULL, NULL, NULL};
	int length = 0;

	int gene_start = 0, gene_end = 0;
	char* chromosome =NULL;
	char* marker = NULL;
	int position = 0;

	unsigned int gene_start_column = UINT_MAX;
	unsigned int gene_end_column = UINT_MAX;
	unsigned int gene_name_column = UINT_MAX;
	unsigned int chromosome_column = UINT_MAX;
	unsigned int marker_column = UINT_MAX;
	unsigned int position_column = UINT_MAX;

	int snps_count = 0;
	int max_snps_count = INT_MIN;

	map<char*, map<int, char*>*, bool(*)(const char*, const char*)> chromosomes(compare);
	map<char*, map<int, char*>*, bool(*)(const char*, const char*)>::iterator chromosomes_it;
	map<int, char*>* snps = NULL;
	map<int, char*>::iterator snps_it;

	ifstream ifile_stream;
	ofstream ofile_stream;

	ifile_stream.exceptions(ios_base::failbit | ios_base::badbit);
	ifile_stream.setf(ios_base::skipws);
	ofile_stream.exceptions(ios_base::failbit | ios_base::badbit);

	/****************** PROCESS SNPs FILE *******************/
	try {
		ifile_stream.open(snps_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, snps_file);
	}

	separator = detect_separator(snps_file, ifile_stream);
	if (separator == -1) {
		throw AnnotationException(30, snps_file);
	}

	try {
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if ((strcmp(token, MARKER_1) == 0) ||
					(strcmp(token, MARKER_2) == 0) ||
					(strcmp(token, MARKER_3) == 0)) {
					marker_column = file_column;
				}
				else if ((strcmp(token, POSITION_1) == 0) ||
						(strcmp(token, POSITION_2) == 0)) {
					position_column =  file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, snps_file);
		}
		else if (marker_column == UINT_MAX) {
			throw AnnotationException(11, snps_file);
		}
		else if (position_column == UINT_MAX) {
			throw AnnotationException(12, snps_file);
		}

		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == marker_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(17, snps_file);
					}
					columns[0] = token;
				}
				else if (file_column == position_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(18, snps_file);
					}
					columns[1] = token;
				}
				else if (file_column == chromosome_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, snps_file);
					}
					columns[2] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL)) {
				position = strtol(columns[1], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(22, snps_file);
				}

				marker = (char*)malloc((strlen(columns[0]) + 1) * sizeof(char));
				if (marker == NULL) {
					throw AnnotationException(29, snps_file);
				}
				strcpy(marker, columns[0]);

				chromosome = columns[2];

				chromosomes_it = chromosomes.find(chromosome);
				if (chromosomes_it != chromosomes.end()) {
					snps = chromosomes_it->second;
				}
				else {
					snps = new map<int, char*>();

					chromosome = (char*)malloc((strlen(columns[2]) + 1) * sizeof(char));
					if (chromosome == NULL) {
						throw AnnotationException(28, snps_file);
					}
					strcpy(chromosome, columns[2]);

					chromosomes.insert(pair<char*, map<int, char*>*>(chromosome, snps));
				}

				snps->insert(pair<int, char*>(position, marker));
			}
			else {
				throw AnnotationException(6, snps_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, snps_file);
		}
	}

	try {
		ifile_stream.close();
		ifile_stream.clear();
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(3, snps_file);
	}

	/****************** PROCESS GENES FILE *******************/
	try {
		ifile_stream.open(genes_file);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, genes_file);
	}

	separator = detect_separator(genes_file, ifile_stream);
	if (separator == -1) {
		throw AnnotationException(30, genes_file);
	}

	if (append) {
		common_separator = separator;
	}
	else {
		common_separator = 1;
	}

	try {
		chromosome_column = UINT_MAX;

		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (append) {
				strcpy(line_copy, line_buffer);
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if (strcmp(token, GENE_START) == 0) {
					gene_start_column = file_column;
				}
				else if (strcmp(token, GENE_END) == 0) {
					gene_end_column =  file_column;
				}
				else if ((strcmp(token, GENE_NAME_1) == 0) ||
						(strcmp(token, GENE_NAME_2) == 0) ||
						(strcmp(token, GENE_NAME_3) == 0)) {
					gene_name_column = file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, genes_file);
		}
		else if (gene_name_column == UINT_MAX) {
			throw AnnotationException(8, genes_file);
		}
		else if (gene_start_column == UINT_MAX) {
			throw AnnotationException(9, genes_file);
		}
		else if (gene_end_column == UINT_MAX) {
			throw AnnotationException(10, genes_file);
		}
	} catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, genes_file);
		}
	}

	if (multicolumn) {
		try {
			while (!ifile_stream.eof()) {
				ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

				length = ifile_stream.gcount() - 2;
				while ((length >= 0) && (line_buffer[length] == '\r')) {
					line_buffer[length] = '\0';
					length -= 1;
				}

				file_column = 0;
				columns[0] = columns[1] = columns[2] = columns[3] = NULL;
				token = strtok(line_buffer, separators_str[separator]);
				while (token != NULL) {
					if (file_column == gene_start_column) {
						if (strlen(token) <= 0) {
							throw AnnotationException(15, genes_file);
						}
						columns[0] = token;
					}
					else if (file_column == gene_end_column) {
						if (strlen(token) <= 0) {
							throw AnnotationException(16, genes_file);
						}
						columns[1] = token;
					}
					else if (file_column == gene_name_column) {
						if (strlen(token) <= 0) {
							throw AnnotationException(14, genes_file);
						}
						columns[2] = token;
					}
					else if (file_column == chromosome_column) {
						if (strlen(token) <= 0) {
							throw AnnotationException(13, genes_file);
						}
						columns[3] = token;
					}

					file_column += 1;
					token = strtok(NULL, separators_str[separator]);
				}

				if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL) && (columns[3] != NULL)) {
					gene_start = strtol(columns[0], &end_ptr, 10);
					if (*end_ptr != '\0') {
						throw AnnotationException(20, genes_file);
					}

					gene_end = strtol(columns[1], &end_ptr, 10);
					if (*end_ptr != '\0') {
						throw AnnotationException(21, genes_file);
					}

					chromosome = columns[3];

					chromosomes_it = chromosomes.find(chromosome);
					if (chromosomes_it != chromosomes.end()) {
						snps = chromosomes_it->second;
						snps_it = snps->lower_bound(gene_start);

						snps_count = 0;
						while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
							snps_count += 1;
							snps_it++;
						}

						if (snps_count > max_snps_count) {
							max_snps_count = snps_count;
						}
					}
				}
				else {
					throw AnnotationException(6, genes_file);
				}
			}
		}
		catch (ios_base::failure &e) {
			if (!ifile_stream.eof()) {
				throw AnnotationException(4, genes_file);
			}
		}

		try {
			ifile_stream.clear();
			ifile_stream.seekg((streampos)0, ifstream::beg);
		}
		catch (ios_base::failure &e) {
			throw AnnotationException(4, genes_file);
		}
	}

	try {
		ofile_stream.open(output_file);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, output_file);
	}

	try {
		if (multicolumn) {
			if (!ifile_stream.eof()) {
				ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

				if (append) {
					length = ifile_stream.gcount() - 2;
					while ((length >= 0) && (line_buffer[length] == '\r')) {
						line_buffer[length] = '\0';
						length -= 1;
					}
					strcpy(line_copy, line_buffer);
				}
			}
		}

		if (append) {
			ofile_stream << line_copy;
		}
		else {
			ofile_stream << "GENE\tCHROMOSOME\tSTART\tEND";
		}

		if (multicolumn) {
			for (int i = 1; i <= max_snps_count; i++) {
				ofile_stream << separators_str[common_separator] <<"SNP" << i << separators_str[common_separator] << "POS" << i;
			}
		}
		else {
			ofile_stream << separators_str[common_separator] << "SNP" << separators_str[common_separator] << "POS";
		}
		ofile_stream << endl;

		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (append) {
				strcpy(line_copy, line_buffer);
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = columns[3] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == gene_start_column) {
					columns[0] = token;
				}
				else if (file_column == gene_end_column) {
					columns[1] = token;
				}
				else if (file_column == gene_name_column) {
					columns[2] = token;
				}
				else if (file_column == chromosome_column) {
					columns[3] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			gene_start = strtol(columns[0], &end_ptr, 10);
			gene_end = strtol(columns[1], &end_ptr, 10);

			chromosome = columns[3];

			chromosomes_it = chromosomes.find(chromosome);
			if (chromosomes_it != chromosomes.end()) {
				snps = chromosomes_it->second;
				snps_it = snps->lower_bound(gene_start);

				if (multicolumn) {
					if (append) {
						ofile_stream << line_copy;
					}
					else {
						ofile_stream << columns[2] << separators_str[common_separator] << chromosome << separators_str[common_separator] << gene_start << separators_str[common_separator] << gene_end;
					}

					snps_count = 0;
					while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
						ofile_stream << separators_str[common_separator] << snps_it->second << separators_str[common_separator] << snps_it->first;
						snps_count += 1;
						snps_it++;
					}

					for (int i = snps_count; i < max_snps_count; i++) {
						ofile_stream << separators_str[common_separator] << separators_str[common_separator];
					}

					ofile_stream << endl;
				}
				else {
					if (append) {
						while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
							ofile_stream << line_copy << separators_str[common_separator] << snps_it->second << separators_str[common_separator] << snps_it->first << endl;
							snps_it++;
						}
					}
					else {
						while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
							ofile_stream << columns[2] << separators_str[common_separator] << chromosome << separators_str[common_separator] << gene_start << separators_str[common_separator] << gene_end;
							ofile_stream << separators_str[common_separator] << snps_it->second << separators_str[common_separator] << snps_it->first << endl;
							snps_it++;
						}
					}
				}
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnnotationException(4, genes_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(5, output_file);
		}
	}

	try {
		ifile_stream.close();
		ifile_stream.clear();
		ofile_stream.close();
	}
	catch (ios_base::failure &e) {
		if (ifile_stream.fail()) {
			throw AnnotationException(3, genes_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(3, output_file);
		}
	}

	/****************** CEAN MEMORY *******************/
	for (chromosomes_it = chromosomes.begin(); chromosomes_it != chromosomes.end(); chromosomes_it++) {
		snps = chromosomes_it->second;

		for (snps_it = snps->begin(); snps_it != snps->end(); snps_it++) {
			free(snps_it->second);
		}
		snps->clear();
		delete snps;

		free(chromosomes_it->first);
	}
	chromosomes.clear();
}

void Annotation::gene_to_snp_by_name(const char* genes_file, const char* genesdb_file, const char* snps_file, const char* output_file, bool append, bool multicolumn) throw (AnnotationException) {
	int separator = 0;
	int common_separator = 0;
	char* token = NULL;
	char* end_ptr = NULL;
	unsigned int file_column = 0;
	char* columns[4] = {NULL, NULL, NULL, NULL};
	int length = 0;

	int gene_start = 0, gene_end = 0;
	char* gene_name = NULL;
	int chromosome = 0;
	char* marker = NULL;
	int position = 0;

	unsigned int gene_start_column = UINT_MAX;
	unsigned int gene_end_column = UINT_MAX;
	unsigned int gene_name_column = UINT_MAX;
	unsigned int chromosome_column = UINT_MAX;
	unsigned int marker_column = UINT_MAX;
	unsigned int position_column = UINT_MAX;

	unsigned int main_gene_name_column = UINT_MAX;
	int main_separator = 0;

	int snps_count = 0;
	int max_snps_count = INT_MIN;

	map<int, map<int, char*>*> chromosomes;
	map<int, map<int, char*>*>::iterator chromosomes_it;
	map<int, char*>* snps = NULL;
	map<int, char*>::iterator snps_it;
	map<char*, vector<int>*, bool(*)(const char*, const char*)> genes(compare);
	map<char*, vector<int>*, bool(*)(const char*, const char*)>::iterator genes_it;

	ifstream isnps_stream;
	ifstream igenes_stream;
	ifstream igenesdb_stream;
	ofstream ofile_stream;

	isnps_stream.exceptions(ios_base::failbit | ios_base::badbit);
	isnps_stream.setf(ios_base::skipws);
	igenes_stream.exceptions(ios_base::failbit | ios_base::badbit);
	igenes_stream.setf(ios_base::skipws);
	igenesdb_stream.exceptions(ios_base::failbit | ios_base::badbit);
	igenesdb_stream.setf(ios_base::skipws);
	ofile_stream.exceptions(ios_base::failbit | ios_base::badbit);

	/****************** PROCESS SNPs FILE *******************/
	try {
		isnps_stream.open(snps_file);
		isnps_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, snps_file);
	}

	separator = detect_separator(snps_file, isnps_stream);
	if (separator == -1) {
		throw AnnotationException(30, snps_file);
	}

	try {
		if (!isnps_stream.eof()) {
			isnps_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = isnps_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if ((strcmp(token, MARKER_1) == 0) ||
					(strcmp(token, MARKER_2) == 0) ||
					(strcmp(token, MARKER_3) == 0)) {
					marker_column = file_column;
				}
				else if ((strcmp(token, POSITION_1) == 0) ||
						(strcmp(token, POSITION_2) == 0)) {
					position_column =  file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, snps_file);
		}
		else if (marker_column == UINT_MAX) {
			throw AnnotationException(11, snps_file);
		}
		else if (position_column == UINT_MAX) {
			throw AnnotationException(12, snps_file);
		}

		while (!isnps_stream.eof()) {
			isnps_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = isnps_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == marker_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(17, snps_file);
					}
					columns[0] = token;
				}
				else if (file_column == position_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(18, snps_file);
					}
					columns[1] = token;
				}
				else if (file_column == chromosome_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, snps_file);
					}
					columns[2] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL)) {
				position = strtol(columns[1], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(22, snps_file);
				}

				marker = (char*)malloc((strlen(columns[0]) + 1) * sizeof(char));
				if (marker == NULL) {
					throw AnnotationException(29, snps_file);
				}
				strcpy(marker, columns[0]);

				chromosome = strtol(columns[2], &end_ptr, 10);
				if (*end_ptr == '\0') {
					chromosomes_it = chromosomes.find(chromosome);
					if (chromosomes_it != chromosomes.end()) {
						snps = chromosomes_it->second;
					}
					else {
						snps = new map<int, char*>();
						chromosomes.insert(pair<int, map<int, char*>*>(chromosome, snps));
					}

					snps->insert(pair<int, char*>(position, marker));
				}
			}
			else {
				throw AnnotationException(6, snps_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!isnps_stream.eof()) {
			throw AnnotationException(4, snps_file);
		}
	}

	try {
		isnps_stream.close();
		isnps_stream.clear();
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(3, snps_file);
	}

	/****************** PROCESSING GENES FILE *******************/
	try {
		igenes_stream.open(genes_file);
		igenes_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, genes_file);
	}

	separator = detect_separator(genes_file, igenes_stream);
	if (separator == -1) {
		throw AnnotationException(30, genes_file);
	}

	if (append) {
		common_separator = separator;
	}
	else {
		common_separator = 1;
	}

	try {
		if (!igenes_stream.eof()) {
			igenes_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = igenes_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if ((strcmp(token, GENE_NAME_1) == 0) ||
						(strcmp(token, GENE_NAME_2) == 0) ||
						(strcmp(token, GENE_NAME_3) == 0)) {
					gene_name_column = file_column;
					break;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (gene_name_column == UINT_MAX) {
			throw AnnotationException(8, genes_file);
		}

		while (!igenes_stream.eof()) {
			igenes_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = igenes_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == gene_name_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(14, genes_file);
					}
					columns[0] = token;
					break;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if (columns[0] != NULL) {
				gene_name = (char*)malloc((strlen(columns[0]) + 1) * sizeof(char));
				if (gene_name == NULL) {
					throw AnnotationException(27, genes_file);
				}
				strcpy(gene_name, columns[0]);

				genes.insert(pair<char*, vector<int>*>(gene_name, new vector<int>()));
			}
			else {
				throw AnnotationException(6, genes_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!igenes_stream.eof()) {
			throw AnnotationException(4, genes_file);
		}
	}

	try {
		igenes_stream.clear();
		igenes_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(4, genes_file);
	}

	/****************** PROCESS GENES DB FILE *******************/
	try {
		igenesdb_stream.open(genesdb_file);
		igenesdb_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, genesdb_file);
	}

	main_separator = separator;
	main_gene_name_column = gene_name_column;

	chromosome_column = UINT_MAX;
	gene_name_column = UINT_MAX;

	separator = detect_separator(genesdb_file, igenesdb_stream);
	if (separator == -1) {
		throw AnnotationException(30, genesdb_file);
	}

	try {
		if (!igenesdb_stream.eof()) {
			igenesdb_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = igenesdb_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				token = lowercase(token);

				if (strcmp(token, GENE_START) == 0) {
					gene_start_column = file_column;
				}
				else if (strcmp(token, GENE_END) == 0) {
					gene_end_column =  file_column;
				}
				else if ((strcmp(token, GENE_NAME_1) == 0) ||
						(strcmp(token, GENE_NAME_2) == 0) ||
						(strcmp(token, GENE_NAME_3) == 0)) {
					gene_name_column = file_column;
				}
				else if ((strcmp(token, CHROMOSOME_1) == 0) ||
						(strcmp(token, CHROMOSOME_2) == 0)) {
					chromosome_column = file_column;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}
		}

		if (chromosome_column == UINT_MAX) {
			throw AnnotationException(7, genesdb_file);
		}
		else if (gene_name_column == UINT_MAX) {
			throw AnnotationException(8, genesdb_file);
		}
		else if (gene_start_column == UINT_MAX) {
			throw AnnotationException(9, genesdb_file);
		}
		else if (gene_end_column == UINT_MAX) {
			throw AnnotationException(10, genesdb_file);
		}

		while (!igenesdb_stream.eof()) {
			igenesdb_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = igenesdb_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			file_column = 0;
			columns[0] = columns[1] = columns[2] = columns[3] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == gene_start_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(15, genesdb_file);
					}
					columns[0] = token;
				}
				else if (file_column == gene_end_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(16, genesdb_file);
					}
					columns[1] = token;
				}
				else if (file_column == gene_name_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(14, genesdb_file);
					}
					columns[2] = token;
				}
				else if (file_column == chromosome_column) {
					if (strlen(token) <= 0) {
						throw AnnotationException(13, genesdb_file);
					}
					columns[3] = token;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			if ((columns[0] != NULL) && (columns[1] != NULL) && (columns[2] != NULL) && (columns[3] != NULL)) {
				gene_start = strtol(columns[0], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(20, genesdb_file);
				}

				gene_end = strtol(columns[1], &end_ptr, 10);
				if (*end_ptr != '\0') {
					throw AnnotationException(21, genesdb_file);
				}

				chromosome = strtol(columns[3], &end_ptr, 10);
				if (*end_ptr == '\0') {
					genes_it = genes.find(columns[2]);
					if (genes_it != genes.end()) {
						genes_it->second->push_back(chromosome);
						genes_it->second->push_back(gene_start);
						genes_it->second->push_back(gene_end);

						if (multicolumn) {
							chromosomes_it = chromosomes.find(chromosome);
							if (chromosomes_it != chromosomes.end()) {
								snps = chromosomes_it->second;
								snps_it = snps->lower_bound(gene_start);

								snps_count = 0;
								while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
									snps_count += 1;
									snps_it++;
								}

								if (snps_count > max_snps_count) {
									max_snps_count = snps_count;
								}
							}
						}
					}
				}
			}
			else {
				throw AnnotationException(6, genesdb_file);
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!igenesdb_stream.eof()) {
			throw AnnotationException(4, genesdb_file);
		}
	}

	try {
		igenesdb_stream.close();
		igenesdb_stream.clear();
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(3, genesdb_file);
	}

	/****************** WRITE RESULTS *******************/
	try {
		ofile_stream.open(output_file);
	}
	catch (ios_base::failure &e) {
		throw AnnotationException(2, output_file);
	}

	separator = main_separator;
	gene_name_column = main_gene_name_column;

	try {
		if (!igenes_stream.eof()) {
			igenes_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			if (append) {
				length = igenes_stream.gcount() - 2;
				while ((length >= 0) && (line_buffer[length] == '\r')) {
					line_buffer[length] = '\0';
					length -= 1;
				}
				strcpy(line_copy, line_buffer);
			}
		}

		if (append) {
			ofile_stream << line_copy;
		}
		else {
			ofile_stream << "GENE";
		}

		if (multicolumn) {
			for (int i = 1; i <= max_snps_count; i++) {
				ofile_stream << separators_str[common_separator] <<"SNP" << i << separators_str[common_separator] << "POS" << i;
			}
		}
		else {
			ofile_stream << separators_str[common_separator] << "SNP" << separators_str[common_separator] << "POS";
		}
		ofile_stream << endl;

		while (!igenes_stream.eof()) {
			igenes_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = igenes_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (append) {
				strcpy(line_copy, line_buffer);
			}

			file_column = 0;
			columns[0] = NULL;
			token = strtok(line_buffer, separators_str[separator]);
			while (token != NULL) {
				if (file_column == gene_name_column) {
					columns[0] = token;
					break;
				}

				file_column += 1;
				token = strtok(NULL, separators_str[separator]);
			}

			genes_it = genes.find(columns[0]);
			if ((genes_it != genes.end()) && (genes_it->second->size() > 0)) {
				chromosome = genes_it->second->at(0);
				gene_start = genes_it->second->at(1);
				gene_end = genes_it->second->at(2);

				chromosomes_it = chromosomes.find(chromosome);
				if (chromosomes_it != chromosomes.end()) {
					snps = chromosomes_it->second;
					snps_it = snps->lower_bound(gene_start);

					if (multicolumn) {
						if (append) {
							ofile_stream << line_copy;
						}
						else {
							ofile_stream << genes_it->first;
						}

						snps_count = 0;
						while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
							ofile_stream << separators_str[common_separator] << snps_it->second << separators_str[common_separator] << snps_it->first;
							snps_count += 1;
							snps_it++;
						}

						for (int i = snps_count; i < max_snps_count; i++) {
							ofile_stream << separators_str[common_separator] << separators_str[common_separator];
						}

						ofile_stream << endl;
					}
					else {
						if (append) {
							while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
								ofile_stream << line_copy << separators_str[common_separator] << snps_it->second << separators_str[common_separator] << snps_it->first << endl;
								snps_it++;
							}
						}
						else {
							while ((snps_it != snps->end()) && (gene_end >= snps_it->first)) {
								ofile_stream << genes_it->first << separators_str[common_separator] << snps_it->second << separators_str[common_separator] << snps_it->first << endl;
								snps_it++;
							}
						}
					}
				}
			}
			else {
				if (multicolumn) {
					if (append) {
						ofile_stream << line_copy;
					}
					else {
						ofile_stream << genes_it->first;
					}

					for (int i = 0; i < max_snps_count; i++) {
						ofile_stream << separators_str[common_separator] << separators_str[common_separator];
					}

					ofile_stream << endl;
				}
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!igenes_stream.eof()) {
			throw AnnotationException(4, genes_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(5, output_file);
		}
	}

	try {
		igenes_stream.close();
		igenes_stream.clear();
		ofile_stream.close();
	}
	catch (ios_base::failure &e) {
		if (igenes_stream.fail()) {
			throw AnnotationException(3, genes_file);
		}

		if (ofile_stream.fail()) {
			throw AnnotationException(3, output_file);
		}
	}

	/****************** CLEAR MEMORY *******************/
	for (chromosomes_it = chromosomes.begin(); chromosomes_it != chromosomes.end(); chromosomes_it++) {
		snps = chromosomes_it->second;

		for (snps_it = snps->begin(); snps_it != snps->end(); snps_it++) {
			free(snps_it->second);
		}
		snps->clear();
		delete snps;
	}
	chromosomes.clear();

	for (genes_it = genes.begin(); genes_it != genes.end(); genes_it++) {
		genes_it->second->clear();
		delete genes_it->second;
		free(genes_it->first);
	}
	genes.clear();
}
