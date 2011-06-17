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

#include "../../include/comparator/Comparator.h"

const int Comparator::LINE_BUFFER_SIZE = 4000;
const int Comparator::HEAP_SIZE = 3000000;
const int Comparator::HEAP_INCREMENT = 100000;

/* Columns */
const char* Comparator::MARKER = "MARKER";
const char* Comparator::ALLELE = "ALLELE";
const char* Comparator::ALLELE1 = "ALLELE1";
const char* Comparator::ALLELE2 = "ALLELE2";
const char* Comparator::CHR = "CHR";
const char* Comparator::POSITION = "POSITION";
const char* Comparator::STRAND = "STRAND";
const char* Comparator::PVALUE = "PVALUE";
const char* Comparator::EFFECT = "EFFECT";
const char* Comparator::STDERR = "STDERR";
const char* Comparator::FREQLABEL = "FREQLABEL";
const char* Comparator::HWE_PVAL = "HWE_PVAL";
const char* Comparator::CALLRATE = "CALLRATE";
const char* Comparator::N_TOTAL = "N_TOTAL";
const char* Comparator::IMPUTED = "IMPUTED";
const char* Comparator::USED_FOR_IMP = "USED_FOR_IMP";
const char* Comparator::OEVAR_IMP = "IMP_QUALITY";

/* Separators */
const char* Comparator::SEPARATOR = "SEPARATOR";
const char* Comparator::HEADER_SEPARATOR = "HEADER_SEPARATOR";
const char* Comparator::DATA_SEPARATOR = "DATA_SEPARATOR";
const char* Comparator::WHITESPACE = "WHITESPACE";
const char* Comparator::COMMA = "COMMA";
const char* Comparator::COMMAS = "COMMAS";
const char* Comparator::TAB = "TAB";
const char* Comparator::SEMICOLON = "SEMICOLON";
const char* Comparator::MISSED = "MISSING";
const int Comparator::CHECK_ROW_COUNT = 10;
const int Comparator::SEPARATORS_SIZE = 4;
const char Comparator::separators[SEPARATORS_SIZE] = {
		',', '\t', ' ', ';'
};

/* Commands */
const char* Comparator::PROCESS = "PROCESS";
const char* Comparator::ANALYZE = "ANALYZE";
const char* Comparator::OUTFILE = "OUTFILE";

/* Metal default values */
const char* Comparator::METAL_OUTPUT = "METAANALYSIS.TBL";
const char* Comparator::METAL_MARKER = "MarkerName";
const char* Comparator::METAL_ALLELE1 = "Allele1";
const char* Comparator::METAL_ALLELE2 = "Allele2";
const char* Comparator::METAL_PVALUE = "P-value";
const char* Comparator::METAL_EFFECT = "Effect";
const char* Comparator::METAL_STDERR = "StdErr";
const char* Comparator::METAL_FREQLABEL = "Freq1";
const char* Comparator::METAL_DIRECTION = "Direction";
const char* Comparator::METAL_ABBREVIATION = "METAANALYSIS";

/* Other default values */
const char* Comparator::CORRECTED_PVALUE = "CORRECTED_PVALUE";
const char* Comparator::CORRECTED_STDERR = "CORRECTED_STDERR";
const char* Comparator::FILTERED_SNPS = "FILTERED_SNPS";

/* Columns to compare */
const int Comparator::COLUMNS_SIZE = 2;
const char* Comparator::columns[COLUMNS_SIZE] = {
		PVALUE, STDERR
};

Comparator::Comparator() throw (ComparatorException) : line_buffer(NULL), cur(NULL), beg(NULL) {
	line_buffer = (char*)malloc(LINE_BUFFER_SIZE * sizeof(char));
	if (line_buffer == NULL) {
		throw ComparatorException(0, "Comparator()", "line_buffer", LINE_BUFFER_SIZE * sizeof(char));
	}
}

Comparator::~Comparator() {
	free(line_buffer);

	line_buffer = NULL;
	cur = NULL;
	beg = NULL;
}

vector<File*>* Comparator::process_script(const char* file_name) throw (ComparatorException, FileException) {
	vector<File*>* files = NULL;
	vector<File*>::iterator files_it;
	File* new_file = NULL;
	File default_file;

	ifstream ifile_stream;
	const char* blanks = " \t";
	const char* separator = " \t";
	int length = 0;
	char* token = NULL;
	char* metal_file_name = NULL;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	try {
		ifile_stream.open(file_name);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file_name);
	}

	default_file.add_column(MARKER, MARKER);
	default_file.add_column(ALLELE1, ALLELE1);
	default_file.add_column(ALLELE2, ALLELE2);
	default_file.add_column(CHR, CHR);
	default_file.add_column(POSITION, POSITION);
	default_file.add_column(STRAND, STRAND);
	default_file.add_column(PVALUE, PVALUE);
	default_file.add_column(EFFECT, EFFECT);
	default_file.add_column(STDERR, STDERR);
	default_file.add_column(FREQLABEL, FREQLABEL);
	default_file.add_column(HWE_PVAL, HWE_PVAL);
	default_file.add_column(CALLRATE, CALLRATE);
	default_file.add_column(N_TOTAL, N_TOTAL);
	default_file.add_column(IMPUTED, IMPUTED);
	default_file.add_column(USED_FOR_IMP, USED_FOR_IMP);
	default_file.add_column(OEVAR_IMP, OEVAR_IMP);

	try {
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (strspn(line_buffer, blanks) != strlen(line_buffer)) {
				token = strtok(line_buffer, separator);

				if (strcmp(token, OUTFILE) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						metal_file_name = (char*)malloc((strlen(token) + 1) * sizeof(char));
						if (metal_file_name == NULL) {
							throw ComparatorException(0, "process_script( ... )", "token", token);
						}
						strcpy(metal_file_name, token);

						token = strtok(NULL, separator);
						if (token != NULL) {
							metal_file_name = (char*)realloc(metal_file_name, (strlen(metal_file_name) + strlen(token) + 1) * sizeof(char));
							if (metal_file_name == NULL) {
								throw ComparatorException(1, "process_script( ... )", "token", token);
							}
							strcat(metal_file_name, token);
						}
					}
				}
				if (strcmp(token, PROCESS) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						new_file = new File(token, default_file);

						if (files == NULL) {
							files = new vector<File*>();
						}

						files->push_back(new_file);

						token = strtok(NULL, separator);
						if (token != NULL) {
							new_file->set_abbreviation(token);
						}
						else {
							new_file->set_abbreviation(new_file->get_file_name());
						}
					}
				}
				else if (strcmp(token, MARKER) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(MARKER, token);
					}
				}
				else if (strcmp(token, ALLELE) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(ALLELE1, token);

						token = strtok(NULL, separator);
						if (token != NULL) {
							default_file.add_column(ALLELE2, token);
						}
					}
				}
				else if (strcmp(token, CHR) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(CHR, token);
					}
				}
				else if (strcmp(token, POSITION) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(POSITION, token);
					}
				}
				else if (strcmp(token, STRAND) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(STRAND, token);
					}
				}
				else if (strcmp(token, PVALUE) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(PVALUE, token);
					}
				}
				else if (strcmp(token, EFFECT) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(EFFECT, token);
					}
				}
				else if (strcmp(token, STDERR) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(STDERR, token);
					}
				}
				else if (strcmp(token, FREQLABEL) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(FREQLABEL, token);
					}
				}
				else if (strcmp(token, HWE_PVAL) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(HWE_PVAL, token);
					}
				}
				else if (strcmp(token, CALLRATE) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(CALLRATE, token);
					}
				}
				else if (strcmp(token, N_TOTAL) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(N_TOTAL, token);
					}
				}
				else if (strcmp(token, IMPUTED) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(IMPUTED, token);
					}
				}
				else if (strcmp(token, USED_FOR_IMP) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(USED_FOR_IMP, token);
					}
				}
				else if (strcmp(token, OEVAR_IMP) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(OEVAR_IMP, token);
					}
				}
				else if (strcmp(token, SEPARATOR) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_property(HEADER_SEPARATOR, token);
						default_file.add_property(DATA_SEPARATOR, token);
					}
				}
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file_name);
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file_name);
	}

	if (files != NULL) {
		if (metal_file_name == NULL) {
			metal_file_name = (char*)malloc((strlen(METAL_OUTPUT) + 1) * sizeof(char));
			if (metal_file_name == NULL) {
				throw ComparatorException(0, "process_script( ... )", "METAL_OUTPUT", METAL_OUTPUT);
			}
			strcpy(metal_file_name, METAL_OUTPUT);
		}

		new_file = new File(metal_file_name);
		new_file->add_column(MARKER, METAL_MARKER);
		new_file->add_column(ALLELE1, METAL_ALLELE1);
		new_file->add_column(ALLELE2, METAL_ALLELE2);
		new_file->add_column(PVALUE, METAL_PVALUE);
		new_file->add_column(EFFECT, METAL_EFFECT);
		new_file->add_column(STDERR, METAL_STDERR);
		new_file->add_column(FREQLABEL, METAL_FREQLABEL);
		new_file->add_column(METAL_DIRECTION, METAL_DIRECTION);
		new_file->add_property(HEADER_SEPARATOR, TAB);
		new_file->add_property(DATA_SEPARATOR, TAB);
		new_file->set_abbreviation(METAL_ABBREVIATION);

		files->insert(files->begin(), new_file);

		for (files_it = files->begin(); files_it != files->end(); files_it++) {
			if (((*files_it)->get_property(HEADER_SEPARATOR) == NULL) || ((*files_it)->get_property(DATA_SEPARATOR) == NULL)) {
				detect_separators(*files_it);
			}
			check_separators(*files_it);
		}
	}

	return files;
}

double Comparator::get_lambda(File* file) throw (ComparatorException, FileException) {
	const char* column_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	int length = 0;
	char* end_ptr;
	char* token = NULL;

	char* column_value = NULL;
	int column_position = -1, position = 0;

	int n = 0;
	double* data = NULL;
	double* new_data = NULL;
	int current_heap_size = 0;

	double d_value = 0.0;
	double lambda = 0;

	int line_id = 0;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	current_heap_size = HEAP_SIZE;
	data = (double*)malloc(current_heap_size * sizeof(double));
	if (data == NULL) {
		throw ComparatorException(0, "get_lambda( ... )", "data", current_heap_size * sizeof(double));
	}

	column_name = file->get_column(PVALUE);
	if (column_name == NULL) {
		throw ComparatorException(11, PVALUE);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		free(data);
		data = NULL;
		throw ComparatorException(2, file->get_full_path());
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, column_name) == 0) {
					column_position = position;
					break;
				}

				token = lstrtok(separator);
				position += 1;
			}
		}

		if (column_position < 0) {
			free(data);
			data = NULL;
			throw ComparatorException(11, column_name);
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			line_id += 1;
			cur = line_buffer;
			position = 0;
			column_value = NULL;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == column_position) {
					column_value = token;
					break;
				}
				token = lstrtok(separator);
				position += 1;
			}

			if (column_value != NULL) {
				d_value = R_strtod(column_value, &end_ptr);

				if ((*end_ptr != '\0') || (isnan(d_value))) {
					continue;
				}

				n += 1;
				if (n > current_heap_size) {
					current_heap_size += HEAP_INCREMENT;

					new_data = (double*)realloc(data, current_heap_size * sizeof(double));
					if (new_data == NULL) {
						free(data);
						data = NULL;
						throw ComparatorException(1, "get_lambda( ... )", "data", current_heap_size * sizeof(double));
					}
					data = new_data;
				}

				data[n - 1] = d_value;
			}
			else {
				free(data);
				data = NULL;
				throw ComparatorException(12, line_id);
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			free(data);
			data = NULL;
			throw ComparatorException(4, file->get_full_path());
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		free(data);
		data = NULL;
		throw ComparatorException(3, file->get_full_path());
	}

	if (n > 0) {
		for (int i = 0; i < n; i++) {
			data[i] = Rf_qchisq(data[i], 1, 0, 0);
		}

		qsort(data, n, sizeof(double), compare_double);

		lambda = gsl_stats_median_from_sorted_data(data, 1, n) / Rf_qchisq(0.5, 1, 0, 0);
	}
	else {
		lambda = numeric_limits<double>::quiet_NaN();
	}

	free(data);
	data = NULL;

	return lambda;
}

map<char*, Data*, bool(*)(const char*, const char*)>* Comparator::get_filtered_snps(File* file, double lambda, double pval_threshold, double maf_threshold) throw (ComparatorException, FileException) {
	const char* snp_column_name = NULL;
	const char* pval_column_name = NULL;
	const char* maf_column_name = NULL;
	const char* dir_column_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	int length = 0;
	char* token = NULL;
	int position = 0;

	int snp_column = -1;
	int pval_column = -1;
	int maf_column = -1;
	int dir_column = -1;

	char* end_ptr = NULL;
	char* new_snp = NULL;

	char* snp_value = NULL;
	char* pval_value = NULL;
	char* maf_value = NULL;
	char* dir_value = NULL;

	double d_pval_value = 0.0;
	double d_maf_value = 0.0;

	int line_id = 0;

	map<char*, Data*, bool(*)(const char*, const char*)>* snps = NULL;
	Data* data = NULL;

	snps = new map<char*, Data*, bool(*)(const char*, const char*)>(compare_chars);

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	snp_column_name = file->get_column(MARKER);
	if (snp_column_name == NULL) {
		throw ComparatorException(11, MARKER);
	}

	pval_column_name = file->get_column(PVALUE);
	if (pval_column_name == NULL) {
		throw ComparatorException(11, PVALUE);
	}

	maf_column_name = file->get_column(FREQLABEL);
	if (maf_column_name == NULL) {
		throw ComparatorException(11, FREQLABEL);
	}

	dir_column_name = file->get_column(METAL_DIRECTION);
	if (dir_column_name == NULL) {
		throw ComparatorException(11, METAL_DIRECTION);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file->get_full_path());
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, snp_column_name) == 0) {
					snp_column = position;
				}
				else if (strcmp(token, pval_column_name) == 0) {
					pval_column = position;
				}
				else if (strcmp(token, maf_column_name) == 0) {
					maf_column = position;
				}
				else if (strcmp(token, dir_column_name) == 0) {
					dir_column = position;
				}
				token = lstrtok(separator);
				position += 1;
			}
		}

		if (snp_column < 0) {
			throw ComparatorException(11, snp_column_name);
		}
		if (pval_column < 0) {
			throw ComparatorException(11, pval_column_name);
		}
		if (maf_column < 0) {
			throw ComparatorException(11, maf_column_name);
		}
		if (dir_column < 0) {
			throw ComparatorException(11, dir_column_name);
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			line_id += 1;
			cur = line_buffer;
			position = 0;
			snp_value = NULL;
			pval_value = NULL;
			maf_value = NULL;
			dir_value = NULL;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == snp_column) {
					snp_value = token;
				}
				else if (position == pval_column) {
					pval_value = token;
				}
				else if (position == maf_column) {
					maf_value = token;
				}
				else if (position == dir_column) {
					dir_value = token;
				}

				token = lstrtok(separator);
				position += 1;
			}

			if (snp_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (pval_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (maf_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (dir_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			d_pval_value = R_strtod(pval_value, &end_ptr);
			if ((*end_ptr != '\0') || (isnan(d_pval_value))) {
				continue;
			}

			/* Correct p-value according to lambda */
			d_pval_value = Rf_pchisq((Rf_qchisq(d_pval_value, 1, 0, 0) / lambda), 1, 0, 0);
			/************************************/

			/* Filter for p-value */
			if (gsl_fcmp(d_pval_value, pval_threshold, EPSILON) == 1) {
				continue;
			}
			/*********************/

			d_maf_value = R_strtod(maf_value, &end_ptr);
			if ((*end_ptr != '\0') || (isnan(d_maf_value))) {
				continue;
			}

			/* Adjust allele frequency */
			if (d_maf_value > 0.5) {
				d_maf_value = 1.0 - d_maf_value;
			}
			/***************************/

			/* Filter for MAF */
			if (gsl_fcmp(d_maf_value, maf_threshold, EPSILON) < 0) {
				continue;
			}
			/*****************/


			/* Insert filtered SNP name and corrected p-value */
			new_snp = (char*)malloc((strlen(snp_value) + 1) * sizeof(char));
			if (new_snp == NULL) {
				throw ComparatorException(0, "get_filtered_snps( ... )", "snp_value", snp_value);
			}
			strcpy(new_snp, snp_value);

			data = new Data();
			data->numeric.push_back(d_pval_value);
			for (unsigned int j = 0; j < strlen(dir_value); j++) {
				if (dir_value[j] == '+') {
					data->direction.push_back(1);
				}
				else if (dir_value[j] == '-') {
					data->direction.push_back(-1);
				}
				else {
					data->direction.push_back(0);
				}
			}
			snps->insert(pair<char*, Data*>(new_snp, data));
			/**************************************************/
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file->get_full_path());
	}

	return snps;
}

void Comparator::add_corrected_p_values(File* file, double lambda, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException) {
	const char* snp_column_name = NULL;
	const char* pval_column_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	int length = 0;
	char* token = NULL;
	int position = 0;

	int snp_column = -1;
	int pval_column = -1;

	char* end_ptr = NULL;

	char* snp_value = NULL;
	char* pval_value = NULL;

	double d_pval_value = 0.0;

	int line_id = 0;

	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	snp_column_name = file->get_column(MARKER);
	if (snp_column_name == NULL) {
		throw ComparatorException(11, MARKER);
	}

	pval_column_name = file->get_column(PVALUE);
	if (pval_column_name == NULL) {
		throw ComparatorException(11, PVALUE);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file->get_full_path());
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, snp_column_name) == 0) {
					snp_column = position;
				}
				else if (strcmp(token, pval_column_name) == 0) {
					pval_column = position;
				}
				token = lstrtok(separator);
				position += 1;
			}
		}

		if (snp_column < 0) {
			ComparatorException(11, snp_column_name);
		}
		if (pval_column < 0) {
			ComparatorException(11, pval_column_name);
		}

		filtered_snps_it = filtered_snps->begin();
		while (filtered_snps_it != filtered_snps->end()) {
			filtered_snps_it->second->numeric.push_back(numeric_limits<double>::quiet_NaN());
			filtered_snps_it++;
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			line_id += 1;
			cur = line_buffer;
			position = 0;
			snp_value = NULL;
			pval_value = NULL;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == snp_column) {
					snp_value = token;
				}
				else if (position == pval_column) {
					pval_value = token;
				}

				token = lstrtok(separator);
				position += 1;
			}

			if (snp_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (pval_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			filtered_snps_it = filtered_snps->find(snp_value);
			if (filtered_snps_it != filtered_snps->end()) {

				d_pval_value = R_strtod(pval_value, &end_ptr);
				if ((*end_ptr != '\0') || (isnan(d_pval_value))) {
					continue;
				}

				/* Correct p-value according to lambda */
				d_pval_value = Rf_pchisq((Rf_qchisq(d_pval_value, 1, 0, 0) / lambda), 1, 0, 0);
				/************************************/

				filtered_snps_it->second->numeric.pop_back();
				filtered_snps_it->second->numeric.push_back(d_pval_value);
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file->get_full_path());
	}
}

void Comparator::add_corrected_stderr(File* file, double lambda, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException) {
	const char* snp_column_name = NULL;
	const char* stderr_column_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	int length = 0;
	char* token = NULL;
	int position = 0;

	int snp_column = -1;
	int stderr_column = -1;

	char* end_ptr = NULL;

	char* snp_value = NULL;
	char* stderr_value = NULL;

	double d_stderr_value = 0.0;

	int line_id = 0;

	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	snp_column_name = file->get_column(MARKER);
	if (snp_column_name == NULL) {
		throw ComparatorException(11, MARKER);
	}

	stderr_column_name = file->get_column(STDERR);
	if (stderr_column_name == NULL) {
		throw ComparatorException(11, STDERR);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file->get_full_path());
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, snp_column_name) == 0) {
					snp_column = position;
				}
				else if (strcmp(token, stderr_column_name) == 0) {
					stderr_column = position;
				}
				token = lstrtok(separator);
				position += 1;
			}
		}

		if (snp_column < 0) {
			throw ComparatorException(11, snp_column_name);
		}
		if (stderr_column < 0) {
			throw ComparatorException(11, stderr_column_name);
		}

		filtered_snps_it = filtered_snps->begin();
		while (filtered_snps_it != filtered_snps->end()) {
			filtered_snps_it->second->numeric.push_back(numeric_limits<double>::quiet_NaN());
			filtered_snps_it++;
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			line_id += 1;
			cur = line_buffer;
			position = 0;
			snp_value = NULL;
			stderr_value = NULL;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == snp_column) {
					snp_value = token;
				}
				else if (position == stderr_column) {
					stderr_value = token;
				}

				token = lstrtok(separator);
				position += 1;
			}

			if (snp_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (stderr_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			filtered_snps_it = filtered_snps->find(snp_value);
			if (filtered_snps_it != filtered_snps->end()) {

				d_stderr_value = R_strtod(stderr_value, &end_ptr);
				if ((*end_ptr != '\0') || (isnan(d_stderr_value))) {
					continue;
				}

				/* Correct StdErr according to lambda */
				d_stderr_value = d_stderr_value * sqrt(lambda);
				/************************************/

				filtered_snps_it->second->numeric.pop_back();
				filtered_snps_it->second->numeric.push_back(d_stderr_value);
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file->get_full_path());
	}
}

void Comparator::add_effect_values(File* file, unsigned int seq_pos, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException) {
	const char* snp_column_name = NULL;
	const char* effect_column_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	int length = 0;
	char* token = NULL;
	int position = 0;

	int snp_column = -1;
	int effect_column = -1;

	char* end_ptr = NULL;

	char* snp_value = NULL;
	char* effect_value = NULL;

	double d_value = 0.0;
	int metal_sign = 0;

	int line_id = 0;

	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	snp_column_name = file->get_column(MARKER);
	if (snp_column_name == NULL) {
		throw ComparatorException(11, MARKER);
	}

	effect_column_name = file->get_column(EFFECT);
	if (effect_column_name == NULL) {
		throw ComparatorException(11, EFFECT);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file->get_full_path());
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, snp_column_name) == 0) {
					snp_column = position;
				}
				else if (strcmp(token, effect_column_name) == 0) {
					effect_column = position;
				}
				token = lstrtok(separator);
				position += 1;
			}
		}

		if (snp_column < 0) {
			throw ComparatorException(11, snp_column_name);
		}
		if (effect_column < 0) {
			throw ComparatorException(11, effect_column_name);
		}

		filtered_snps_it = filtered_snps->begin();
		while (filtered_snps_it != filtered_snps->end()) {
			filtered_snps_it->second->numeric.push_back(numeric_limits<double>::quiet_NaN());
			filtered_snps_it++;
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			line_id += 1;
			cur = line_buffer;
			position = 0;
			snp_value = NULL;
			effect_value = NULL;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == snp_column) {
					snp_value = token;
				}
				else if (position == effect_column) {
					effect_value = token;
				}

				token = lstrtok(separator);
				position += 1;
			}

			if (snp_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (effect_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			filtered_snps_it = filtered_snps->find(snp_value);
			if (filtered_snps_it != filtered_snps->end()) {
				d_value = R_strtod(effect_value, &end_ptr);
				if ((*end_ptr != '\0') || (isnan(d_value))) {
					continue;
				}

				if (seq_pos > filtered_snps_it->second->direction.size()) {
					throw ComparatorException(13, snp_value);
				}

				metal_sign = filtered_snps_it->second->direction.at(seq_pos);

				if (((metal_sign > 0) && (d_value < 0)) ||
					((metal_sign < 0) && (d_value > 0))) {
					d_value = -d_value;
				}

				filtered_snps_it->second->numeric.pop_back();
				filtered_snps_it->second->numeric.push_back(d_value);
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file->get_full_path());
	}
}

void Comparator::add_freq_values(File* file, unsigned int seq_pos, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException) {
	const char* snp_column_name = NULL;
	const char* effect_column_name = NULL;
	const char* freq_column_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	int length = 0;
	char* token = NULL;
	int position = 0;

	int snp_column = -1;
	int effect_column = -1;
	int freq_column = -1;

	char* end_ptr = NULL;

	char* snp_value = NULL;
	char* effect_value = NULL;
	char* freq_value = NULL;

	double d_effect_value = 0.0;
	double d_freq_value = 0.0;
	int metal_sign = 0;

	int line_id = 0;

	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	snp_column_name = file->get_column(MARKER);
	if (snp_column_name == NULL) {
		throw ComparatorException(11, MARKER);
	}

	effect_column_name = file->get_column(EFFECT);
	if (effect_column_name == NULL) {
		throw ComparatorException(11, EFFECT);
	}

	freq_column_name = file->get_column(FREQLABEL);
	if (freq_column_name == NULL) {
		throw ComparatorException(11, FREQLABEL);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file->get_full_path());
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, snp_column_name) == 0) {
					snp_column = position;
				}
				else if (strcmp(token, effect_column_name) == 0) {
					effect_column = position;
				}
				else if (strcmp(token, freq_column_name) == 0) {
					freq_column = position;
				}
				token = lstrtok(separator);
				position += 1;
			}
		}

		if (snp_column < 0) {
			throw ComparatorException(11, snp_column_name);
		}
		if (effect_column < 0) {
			throw ComparatorException(11, effect_column_name);
		}
		if (freq_column < 0) {
			throw ComparatorException(11, freq_column_name);
		}

		filtered_snps_it = filtered_snps->begin();
		while (filtered_snps_it != filtered_snps->end()) {
			filtered_snps_it->second->numeric.push_back(numeric_limits<double>::quiet_NaN());
			filtered_snps_it++;
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			line_id += 1;
			cur = line_buffer;
			position = 0;
			snp_value = NULL;
			effect_value = NULL;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == snp_column) {
					snp_value = token;
				}
				else if (position == effect_column) {
					effect_value = token;
				}
				else if (position == freq_column) {
					freq_value = token;
				}

				token = lstrtok(separator);
				position += 1;
			}

			if (snp_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (effect_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (freq_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			filtered_snps_it = filtered_snps->find(snp_value);
			if (filtered_snps_it != filtered_snps->end()) {
				d_freq_value = R_strtod(freq_value, &end_ptr);
				if ((*end_ptr != '\0') || (isnan(d_freq_value))) {
					continue;
				}

				d_effect_value = R_strtod(effect_value, &end_ptr);
				if ((*end_ptr == '\0') && (!isnan(d_effect_value))) {

					if (seq_pos >= filtered_snps_it->second->direction.size()) {
						throw ComparatorException(13, snp_value);
					}

					metal_sign = filtered_snps_it->second->direction.at(seq_pos);

					if (((metal_sign > 0) && (d_effect_value < 0)) ||
						((metal_sign < 0) && (d_effect_value > 0))) {
						d_freq_value = 1 - d_freq_value;
					}
				}


				filtered_snps_it->second->numeric.pop_back();
				filtered_snps_it->second->numeric.push_back(d_freq_value);
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file->get_full_path());
	}
}

void Comparator::add_values(File* file, const char* column_name, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException) {
	const char* snp_column_name = NULL;
	const char* data_column_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	int length = 0;
	char* token = NULL;
	int position = 0;

	int snp_column = -1;
	int data_column = -1;

	char* end_ptr = NULL;

	char* snp_value = NULL;
	char* data_value = NULL;

	double d_value = 0.0;

	int line_id = 0;

	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	snp_column_name = file->get_column(MARKER);
	if (snp_column_name == NULL) {
		throw ComparatorException(11, MARKER);
	}

	data_column_name = file->get_column(column_name);
	if (data_column_name == NULL) {
		throw ComparatorException(11, column_name);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file->get_full_path());
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, snp_column_name) == 0) {
					snp_column = position;
				}
				else if (strcmp(token, data_column_name) == 0) {
					data_column = position;
				}
				token = lstrtok(separator);
				position += 1;
			}
		}

		if (snp_column < 0) {
			throw ComparatorException(11, snp_column_name);
		}
		if (data_column < 0) {
			throw ComparatorException(11, data_column_name);
		}

		filtered_snps_it = filtered_snps->begin();
		while (filtered_snps_it != filtered_snps->end()) {
			filtered_snps_it->second->numeric.push_back(numeric_limits<double>::quiet_NaN());
			filtered_snps_it++;
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			line_id += 1;
			cur = line_buffer;
			position = 0;
			snp_value = NULL;
			data_value = NULL;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == snp_column) {
					snp_value = token;
				}
				else if (position == data_column) {
					data_value = token;
				}

				token = lstrtok(separator);
				position += 1;
			}

			if (snp_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			if (data_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			filtered_snps_it = filtered_snps->find(snp_value);
			if (filtered_snps_it != filtered_snps->end()) {

				d_value = R_strtod(data_value, &end_ptr);
				if ((*end_ptr != '\0') || (isnan(d_value))) {
					continue;
				}

				filtered_snps_it->second->numeric.pop_back();
				filtered_snps_it->second->numeric.push_back(d_value);
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file->get_full_path());
	}
}

void Comparator::print_all_columns(File* file, const char* output_file_name, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException, FileException) {
	const char* snp_column_name = NULL;
	char* full_output_file_name = NULL;
	char separator = '\0';

	ifstream ifile_stream;
	ofstream ofile_stream;

	char* line_copy = NULL;
	int length = 0;
	char* token = NULL;
	int position = 0;

	int snp_column = -1;

	char* snp_value = NULL;

	int line_id = 0;

	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;

	ifile_stream.exceptions(ios_base::failbit | ios_base::badbit);
	ifile_stream.setf(ios_base::skipws);
	ofile_stream.exceptions(ios_base::failbit | ios_base::badbit);

	line_copy = (char*)malloc(LINE_BUFFER_SIZE * sizeof(char));
	if (line_copy == NULL) {
		throw ComparatorException(0, "print_all_columns( ... )", "line_copy", LINE_BUFFER_SIZE * sizeof(char));
	}

	full_output_file_name = (char*)malloc((strlen(output_file_name) + strlen(".txt") + 1) * sizeof(char));
	if (full_output_file_name == NULL) {
		throw ComparatorException(0, "print_all_columns( ... )", "output_file_name", output_file_name);
	}
	strcpy(full_output_file_name, output_file_name);
	strcat(full_output_file_name, ".txt");

	snp_column_name = file->get_column(MARKER);
	if (snp_column_name == NULL) {
		throw ComparatorException(11, MARKER);
	}

	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
		ofile_stream.open(full_output_file_name);
	}
	catch (ios_base::failure &e) {
		if (ifile_stream.fail()) {
			throw ComparatorException(2, file->get_full_path());
		}

		if (ofile_stream.fail()) {
			throw ComparatorException(2, full_output_file_name);
		}
	}

	try {
		separator = file->get_header_separator();
		if (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			ofile_stream << line_buffer << endl;

			cur = line_buffer;
			token = lstrtok(separator);
			while (token != NULL) {
				if (strcmp(token, snp_column_name) == 0) {
					snp_column = position;
					break;
				}

				token = lstrtok(separator);
				position += 1;
			}
		}

		if (snp_column < 0) {
			throw ComparatorException(11, snp_column_name);
		}

		separator = file->get_data_separator();
		while (!ifile_stream.eof()) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;

			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			strcpy(line_copy, line_buffer);

			line_id += 1;
			cur = line_buffer;
			position = 0;
			token = lstrtok(separator);
			while (token != NULL) {
				if (position == snp_column) {
					snp_value = token;
					break;
				}

				token = lstrtok(separator);
				position += 1;
			}

			if (snp_value == NULL) {
				throw ComparatorException(12, line_id);
			}

			filtered_snps_it = filtered_snps->find(snp_value);
			if (filtered_snps_it != filtered_snps->end()) {
				ofile_stream << line_copy << endl;
			}
		}
	}
	catch (ios_base::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}

		if (ofile_stream.fail()) {
			throw ComparatorException(5, full_output_file_name);
		}
	}

	try {
		ifile_stream.close();
		ofile_stream.close();
	}
	catch (ios_base::failure &e) {
		if (ifile_stream.fail()) {
			throw ComparatorException(3, file->get_full_path());
		}

		if (ofile_stream.fail()) {
			throw ComparatorException(3, full_output_file_name);
		}
	}
}

void Comparator::print_single_column(vector<File*>* files, const char* column_name, bool skip_metal, map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) throw (ComparatorException) {
	ofstream ofile_stream;

	char* output_file_name = NULL;

	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;
	vector<File*>::iterator files_it;
	vector<double>::iterator values_it;

	ofile_stream.exceptions(ios_base::failbit | ios_base::badbit);

	output_file_name = (char*)malloc((strlen(column_name) + strlen(".txt") + 1) * sizeof(char));
	if (output_file_name == NULL) {
		throw ComparatorException(0, "print_single_column( ... )", "column_name", column_name);
	}
	strcpy(output_file_name, column_name);
	strcat(output_file_name, ".txt");

	try {
		ofile_stream.open(output_file_name);
	}
	catch (ios_base::failure &e) {
		throw ComparatorException(2, output_file_name);
	}

	try {
		ofile_stream << MARKER;
		files_it = files->begin();
		if (skip_metal == true) {
			files_it++;
		}
		while (files_it != files->end()) {
			if ((*files_it)->get_abbreviation() != NULL) {
				ofile_stream << "\t" << (*files_it)->get_abbreviation();
			}
			else {
				ofile_stream << "\t" << (*files_it)->get_file_name();
			}
			files_it++;
		}

		ofile_stream << endl;

		filtered_snps_it = filtered_snps->begin();
		while (filtered_snps_it != filtered_snps->end()) {
			ofile_stream << filtered_snps_it->first;

			values_it = filtered_snps_it->second->numeric.begin();
			while (values_it != filtered_snps_it->second->numeric.end()) {
				ofile_stream << "\t" << *values_it;
				values_it++;
			}

			ofile_stream << endl;
			filtered_snps_it++;
		}
	}
	catch (ifstream::failure &e) {
		throw ComparatorException(5, output_file_name);
	}

	try {
		ofile_stream.close();
	}
	catch (ios_base::failure &e) {
		throw ComparatorException(3, output_file_name);
	}
}

void Comparator::clear_values(map<char*, Data*, bool(*)(const char*, const char*)>* filtered_snps) {
	map<char*, Data*, bool(*)(const char*, const char*)>::iterator filtered_snps_it;

	filtered_snps_it = filtered_snps->begin();
	while (filtered_snps_it != filtered_snps->end()) {
		filtered_snps_it->second->clear();
		filtered_snps_it++;
	}
}

void Comparator::detect_separators(File* file) throw (ComparatorException) {
	ifstream ifile_stream;

	int header_tokens[SEPARATORS_SIZE];
	int row_tokens[SEPARATORS_SIZE];

	bool header = false;
	bool rows = false;

	int length = 0;
	int i = 0, j = 0;
	int tokens = 0;
	int row = 0;
	unsigned int char_pos = 0;
	int matches = 0;
	int header_separator = 0;
	int data_separator = 0;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	/* Open input file */
	try {
		ifile_stream.open(file->get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw ComparatorException(2, file->get_full_path());
	}

	/* Initialize arrays for the number of tokens. */
	i = 0;
	while (i < SEPARATORS_SIZE) {
		header_tokens[i] = 0;
		row_tokens[i] = 0;
		i += 1;
	}

	try {
		/* Read header and calculate number of tokens for each separator. */
		if ((!ifile_stream.eof()) && (header != true)) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (length >= 0) {
				header = true;

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
					header_tokens[i] = tokens;
					i += 1;
				}
			}
		}

		/* Read first row and initialize array with number of tokens for each separator. */
		while ((!ifile_stream.eof()) && (rows != true)) {
			ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

			length = ifile_stream.gcount() - 2;
			while ((length >= 0) && (line_buffer[length] == '\r')) {
				line_buffer[length] = '\0';
				length -= 1;
			}

			if (length >= 0) {
				rows = true;

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

		/* Read next N rows and calculate number of tokens for each separator. */
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

	} catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw ComparatorException(4, file->get_full_path());
		}
	}

	/* Determine the common separators. */
	if (header == true) {
		if (rows == false) {
			i = 0;
			while (i < SEPARATORS_SIZE) {
				if (header_tokens[i] > 0) {
					header_separator = i;
					data_separator = i;
					matches += 1;
				}
				i += 1;
			}
		}
		else {
			i = 0;
			while (i < SEPARATORS_SIZE) {
				j = 0;
				while (j < SEPARATORS_SIZE) {
					if ((header_tokens[i] == row_tokens[j]) && (header_tokens[i] > 0)) {
						matches += 1;
						header_separator = i;
						data_separator = j;
					}
					j += 1;
				}
				i += 1;
			}
		}

		if (matches < 1) {
			i = 0;
			while (i < SEPARATORS_SIZE) {
				if ((header_tokens[i] != 0) || (row_tokens[i] != 0)) {
					throw ComparatorException(6, file->get_full_path());
				}
				i += 1;
			}
		}
		else if (matches > 1) {
			throw ComparatorException(6, file->get_full_path());
		}
	}

	switch (header_separator) {
		case 0:
			file->add_property(HEADER_SEPARATOR, COMMA);
			break;
		case 1:
			file->add_property(HEADER_SEPARATOR, TAB);
			break;
		case 2:
			file->add_property(HEADER_SEPARATOR, WHITESPACE);
			break;
		default:
			file->add_property(HEADER_SEPARATOR, SEMICOLON);
			break;
	}

	switch (data_separator) {
		case 0:
			file->add_property(DATA_SEPARATOR, COMMA);
			break;
		case 1:
			file->add_property(DATA_SEPARATOR, TAB);
			break;
		case 2:
			file->add_property(DATA_SEPARATOR, WHITESPACE);
			break;
		default:
			file->add_property(DATA_SEPARATOR, SEMICOLON);
			break;
	}

	/* Close input file */
	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw ComparatorException(3, file->get_full_path());
	}
}

void Comparator::check_separators(File* file) throw (ComparatorException, FileException) {
	const char* separator_name = NULL;

	separator_name = file->get_property(HEADER_SEPARATOR);
	if (separator_name == NULL) {
		throw ComparatorException(7, file->get_full_path());
	}
	else if ((sstrcmp(separator_name, COMMA) == 0) || (sstrcmp(separator_name, COMMAS) == 0)) {
		file->set_header_separator(separators[0]);
	}
	else if (sstrcmp(separator_name, TAB) == 0) {
		file->set_header_separator(separators[1]);
	}
	else if (sstrcmp(separator_name, WHITESPACE) == 0) {
		file->set_header_separator(separators[2]);
	}
	else if (sstrcmp(separator_name, SEMICOLON) == 0) {
		file->set_header_separator(separators[3]);
	}
	else {
		throw ComparatorException(8, file->get_full_path());
	}

	separator_name = file->get_property(DATA_SEPARATOR);
	if (separator_name == NULL) {
		throw ComparatorException(9, file->get_full_path());
	}
	if ((sstrcmp(separator_name, COMMA) == 0) || (sstrcmp(separator_name, COMMAS) == 0)) {
		file->set_data_separator(separators[0]);
	}
	else if (sstrcmp(separator_name, TAB) == 0) {
		file->set_data_separator(separators[1]);
	}
	else if (sstrcmp(separator_name, WHITESPACE) == 0) {
		file->set_data_separator(separators[2]);
	}
	else if (sstrcmp(separator_name, SEMICOLON) == 0) {
		file->set_data_separator(separators[3]);
	}
	else {
		throw ComparatorException(10, file->get_full_path());
	}
}

int Comparator::sstrcmp(const char* first, const char* second) {
	int i = 0;

	while ((first[i] != '\0') && (second[i] != '\0')) {
		if (tolower(first[i]) < tolower(second[i])) {
			return -1;
		}
		else if (tolower(first[i]) > tolower(second[i])) {
			return 1;
		}
		i++;
	}

	if (first[i] == '\0') {
		if (second[i] == '\0') {
			return 0;
		}
		return -1;
	}
	return 1;
}

char* Comparator::lstrtok(const char sep) {
	if (cur == NULL) {
		return NULL;
	}

	beg = cur;

	while (*cur != '\0') {
		if (*cur == sep) {
			*cur = '\0';
			cur++;
			return beg;
		}

		cur++;
	}

	cur = NULL;

	return beg;
}

bool Comparator::compare_chars (const char* first, const char* second) {
	return strcmp(first, second) < 0;
}

int Comparator::compare_double (const void* first, const void* second) {
	double d_first = *(double*)first;
	double d_second = *(double*)second;

	if (d_first < d_second) {
		return -1;
	}
	else if (d_first > d_second) {
		return 1;
	}

	return 0;
}
