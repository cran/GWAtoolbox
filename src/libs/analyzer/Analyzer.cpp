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

#include "../../include/analyzer/Analyzer.h"

/* Constants */
const int Analyzer::LINE_BUFFER_SIZE = 4000;

/* Columns */
const char* Analyzer::MARKER = "MARKER";
const char* Analyzer::ALLELE = "ALLELE";
const char* Analyzer::ALLELE1 = "ALLELE1";
const char* Analyzer::ALLELE2 = "ALLELE2";
const char* Analyzer::CHR = "CHR";
const char* Analyzer::POSITION = "POSITION";
const char* Analyzer::STRAND = "STRAND";
const char* Analyzer::PVALUE = "PVALUE";
const char* Analyzer::PVALUE_HQ_1 = "PVALUE_HQ_1";
const char* Analyzer::PVALUE_HQ_2 = "PVALUE_HQ_2";
const char* Analyzer::PVALUE_FROM0TO1 = "PVALUE_FROM0TO1";
const char* Analyzer::PVALUE_MAF_1 = "PVALUE_MAF_1";
const char* Analyzer::PVALUE_MAF_2 = "PVALUE_MAF_2";
const char* Analyzer::PVALUE_IMP_1 = "PVALUE_IMP_1";
const char* Analyzer::PVALUE_IMP_2 = "PVALUE_IMP_2";
const char* Analyzer::EFFECT = "EFFECT";
const char* Analyzer::EFFECT_HQ = "EFFECT_HQ";
const char* Analyzer::STDERR = "STDERR";
const char* Analyzer::FREQLABEL = "FREQLABEL";
const char* Analyzer::HWE_PVAL = "HWE_PVAL";
const char* Analyzer::CALLRATE = "CALLRATE";
const char* Analyzer::N_TOTAL = "N";
const char* Analyzer::IMPUTED = "IMPUTED";
const char* Analyzer::USED_FOR_IMP = "USED_FOR_IMP";
const char* Analyzer::OEVAR_IMP = "IMP_QUALITY";

/* Separators */
const char* Analyzer::SEPARATOR = "SEPARATOR";
const char* Analyzer::HEADER_SEPARATOR = "HEADER_SEPARATOR";
const char* Analyzer::DATA_SEPARATOR = "DATA_SEPARATOR";
const char* Analyzer::WHITESPACE = "WHITESPACE";
const char* Analyzer::COMMA = "COMMA";
const char* Analyzer::TAB = "TAB";
const char* Analyzer::SEMICOLON = "SEMICOLON";
const char* Analyzer::MISSED = "MISSING";
const int Analyzer::CHECK_ROW_COUNT_FOR_SEPARATOR = 10;
const int Analyzer::CHECK_ROW_COUNT_FOR_SIZE = 15;
const int Analyzer::CHECK_SAMPLE_COUNT_FOR_SIZE = 30;
const int Analyzer::SEPARATORS_SIZE = 4;
const char Analyzer::COMMENT = '#';
const char Analyzer::separators[SEPARATORS_SIZE] = {
		',', '\t', ' ', ';'
};

/* Commands */
const char* Analyzer::PROCESS = "PROCESS";
const char* Analyzer::ANALYZE = "ANALYZE";
const char* Analyzer::VERBOSITY = "VERBOSITY";
const char* Analyzer::MAF = "MAF";
const char* Analyzer::IMP = "IMP";
const char*	Analyzer::BOXPLOTWIDTH = "BOXPLOTWIDTH";
const char* Analyzer::PREFIX = "PREFIX";
const char* Analyzer::CASESENSITIVE = "CASESENSITIVE";
const char* Analyzer::SNP_HQ = "HQ_SNP";

/* Verbosity */
const char* Analyzer::VERBOSITY_LOW = "1";
const char* Analyzer::VERBOSITY_HIGH = "2";

/* Mandatory columns */
const int Analyzer::MANDATORY_COLUMNS_SIZE = 11;
const char* Analyzer::mandatory_columns[MANDATORY_COLUMNS_SIZE] = {
		MARKER, ALLELE1, ALLELE2, CHR, POSITION, STRAND,
		PVALUE, EFFECT, STDERR, FREQLABEL, OEVAR_IMP
};

/* Columns with thresholds */
const int Analyzer::THRESHOLD_COLUMNS_SIZE = 6;
const char* Analyzer::threshold_columns[THRESHOLD_COLUMNS_SIZE] = {
		PVALUE, STDERR, FREQLABEL, OEVAR_IMP, HWE_PVAL, CALLRATE
};

/* Mandatory columns for CSV and their order */
const int Analyzer::CSV_COLUMNS_SIZE = 9;
const char* Analyzer::csv_columns[CSV_COLUMNS_SIZE] = {
		N_TOTAL, EFFECT, STDERR, PVALUE, MAF, IMPUTED, OEVAR_IMP, CALLRATE, HWE_PVAL
};

/* Dependencies (for additional warnings) */
const int Analyzer::DEPENDENCIES_SIZE = 15;
const char* Analyzer::dependencies[DEPENDENCIES_SIZE][2] = {
		{EFFECT, MARKER}, {EFFECT, CHR}, {EFFECT, POSITION},
		{STDERR, MARKER}, {STDERR, CHR}, {STDERR, POSITION},
		{PVALUE, MARKER}, {PVALUE, CHR}, {PVALUE, POSITION},
		{EFFECT, STDERR}, {EFFECT, PVALUE},
		{STDERR, EFFECT}, {STDERR, PVALUE},
		{PVALUE, EFFECT}, {PVALUE, STDERR}
};

/* Single column plots */
const int Analyzer::SINGLE_COLUMN_PLOTS_SIZE = 12;
const char* Analyzer::single_column_plots[SINGLE_COLUMN_PLOTS_SIZE][2] = {
		{STDERR, STDERR}, {OEVAR_IMP, OEVAR_IMP}, {PVALUE, PVALUE}, {PVALUE, PVALUE_HQ_1}, {FREQLABEL, FREQLABEL},
		{HWE_PVAL, HWE_PVAL}, {CALLRATE, CALLRATE}, {EFFECT, EFFECT}, {EFFECT, EFFECT_HQ}, {N_TOTAL, N_TOTAL},
		{IMPUTED, IMPUTED}, {USED_FOR_IMP, USED_FOR_IMP}
};

/* Common plots */
const int Analyzer::COMMON_BOXPLOTS_SIZE = 2;
const char* Analyzer::common_boxplots[COMMON_BOXPLOTS_SIZE][2] = {
		{EFFECT, "All Data"}, {EFFECT_HQ, "High Quality Data"}
};

/* QQ plots */
const int Analyzer::COMMON_QQPLOTS_SIZE = 9;
const char* Analyzer::common_qqplots[COMMON_QQPLOTS_SIZE][2] = {
		{"qqplot_MAF", PVALUE_MAF_2}, {"qqplot_MAF", PVALUE_MAF_1}, {"qqplot_MAF", PVALUE_FROM0TO1},
		{"qqplot_IMP", PVALUE_IMP_2}, {"qqplot_IMP", PVALUE_IMP_1}, {"qqplot_IMP", PVALUE_FROM0TO1},
		{"qqplot_MAF_IMP", PVALUE_HQ_2}, {"qqplot_MAF_IMP", PVALUE_HQ_1}, {"qqplot_MAF_IMP", PVALUE_FROM0TO1}
};

/* External resources */
const char* Analyzer::CONTENT_STYLE = "content_style.css";
const char* Analyzer::MENU_STYLE = "menu_style.css";
const char* Analyzer::JSCRIPT = "script.js";

/* Output file names */
const char* Analyzer::MENU_FILE = "menu.html";
const char* Analyzer::MAIN_FILE = "main.html";
const char* Analyzer::BOXPLOTS_FILE = "boxplots.html";
/*const char* Analyzer::SUMMARY_FILE = "summary.txt";*/

/* File extensions */
const char* Analyzer::TXT_EXTENSION = ".txt";
const char* Analyzer::CSV_EXTENSION = ".csv";
const char* Analyzer::HTML_EXTENSION = ".html";
const char* Analyzer::HTM_EXTENSION = ".htm";

Analyzer::Analyzer() throw (AnalyzerException) : line_buffer(NULL), cur(NULL), beg(NULL) {
	line_buffer = (char*)malloc(LINE_BUFFER_SIZE * sizeof(char));
	if (line_buffer == NULL) {
		throw AnalyzerException(0, "Analyzer()", "line_buffer", LINE_BUFFER_SIZE * sizeof(char));
	}
}

Analyzer::~Analyzer() {
	free(line_buffer);

	line_buffer = NULL;
	cur = NULL;
	beg = NULL;
}

/*
 * 	Description:
 *  	Processes METAL-like script file
 * 	Arguments:
 * 		file_name		-- name of the input script file
 * 		file_separator	-- file separator used in full file path (e.g. '/')
 * 	Returns:
 * 		List with files to process or NULL if no files were specified.
 */
vector<File*>* Analyzer::process_script(const char* file_name, char file_separator) throw (AnalyzerException, FileException) {
	vector<File*>* files = NULL;
	vector<File*>::iterator file_it;
	File* new_file = NULL;
	File default_file;

	ifstream ifile_stream;
	const char* blanks = " \t";
	const char* separator = " \t";
	char* token = NULL;
	const char* file_extension = NULL;
	char* end_ptr = NULL;
	double d_value = 0.0;
	int length = 0;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	try {
		ifile_stream.open(file_name);
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw AnalyzerException(1, file_name);
	}

	/* Initially set default file columns. */
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

	/* Initially set default file properties. */
	default_file.add_property(MISSED, "NA");
	default_file.add_property(VERBOSITY, VERBOSITY_LOW);
	default_file.add_property(CASESENSITIVE, "0");

	/* Initially set default thresholds. */
	default_file.add_threshold(STDERR, 0);
	default_file.add_threshold(STDERR, 100000);
	default_file.add_threshold(OEVAR_IMP, 0);
	default_file.add_threshold(OEVAR_IMP, 1.5);
	default_file.add_threshold(PVALUE, 0);
	default_file.add_threshold(PVALUE, 1);
	default_file.add_threshold(FREQLABEL, 0);
	default_file.add_threshold(FREQLABEL, 1);
	default_file.add_threshold(HWE_PVAL, 0);
	default_file.add_threshold(HWE_PVAL, 1);
	default_file.add_threshold(CALLRATE, 0);
	default_file.add_threshold(CALLRATE, 1);
	default_file.add_threshold(MAF, 0.01);
	default_file.add_threshold(MAF, 0.05);
	default_file.add_threshold(IMP, 0.3);
	default_file.add_threshold(IMP, 0.6);
	default_file.add_threshold(SNP_HQ, 0.01);
	default_file.add_threshold(SNP_HQ, 0.3);

	try {
		while (!ifile_stream.eof()) {
			length = get_line(ifile_stream, line_buffer, LINE_BUFFER_SIZE);

			if ((length > 0) && (strspn(line_buffer, blanks) != strlen(line_buffer))) {
				token = strtok(line_buffer, separator);

				if (strcmp(token, PROCESS) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						new_file = new File(token, default_file, file_separator);

						token = strtok(NULL, separator);
						if ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
							new_file->set_abbreviation(token);
							new_file->set_output_name(token);
						}
						else {
							file_extension = new_file->get_file_extension();

							if ((file_extension != NULL) && ((sstrcmp(file_extension, TXT_EXTENSION) == 0) || (sstrcmp(file_extension, CSV_EXTENSION) == 0) ||
									(sstrcmp(file_extension, HTML_EXTENSION) == 0) || (sstrcmp(file_extension, HTM_EXTENSION) == 0))) {
								new_file->set_output_name(new_file->transform_file_name(NULL, NULL, false));
							}
							else {
								new_file->set_output_name(new_file->transform_file_name(NULL, NULL, true));
							}
						}

						if (files == NULL) {
							files = new vector<File*>();
						}

						files->push_back(new_file);
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

						token = strtok(NULL, separator);
						if ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
							default_file.remove_threshold(PVALUE);
							do {
								d_value = strtod(token, &end_ptr);
								if (*end_ptr != '\0') {
									throw AnalyzerException(27, token);
								}
								default_file.add_threshold(PVALUE, d_value);
								token = strtok(NULL, separator);
							} while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT));
						}
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

						token = strtok(NULL, separator);
						if ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
							default_file.remove_threshold(STDERR);
							do {
								d_value = strtod(token, &end_ptr);
								if (*end_ptr != '\0') {
									throw AnalyzerException(27, token);
								}
								default_file.add_threshold(STDERR, d_value);
								token = strtok(NULL, separator);
							} while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT));
						}
					}
				}
				else if (strcmp(token, FREQLABEL) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(FREQLABEL, token);

						token = strtok(NULL, separator);
						if ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
							default_file.remove_threshold(FREQLABEL);
							do {
								d_value = strtod(token, &end_ptr);
								if (*end_ptr != '\0') {
									throw AnalyzerException(27, token);
								}
								default_file.add_threshold(FREQLABEL, d_value);
								token = strtok(NULL, separator);
							} while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT));
						}
					}
				}
				else if (strcmp(token, HWE_PVAL) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(HWE_PVAL, token);

						token = strtok(NULL, separator);
						if ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
							default_file.remove_threshold(HWE_PVAL);
							do {
								d_value = strtod(token, &end_ptr);
								if (*end_ptr != '\0') {
									throw AnalyzerException(27, token);
								}
								default_file.add_threshold(HWE_PVAL, d_value);
								token = strtok(NULL, separator);
							} while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT));
						}
					}
				}
				else if (strcmp(token, CALLRATE) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_column(CALLRATE, token);

						token = strtok(NULL, separator);
						if ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
							default_file.remove_threshold(CALLRATE);
							do {
								d_value = strtod(token, &end_ptr);
								if (*end_ptr != '\0') {
									throw AnalyzerException(27, token);
								}
								default_file.add_threshold(CALLRATE, d_value);
								token = strtok(NULL, separator);
							} while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT));
						}
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

						token = strtok(NULL, separator);
						if ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
							default_file.remove_threshold(OEVAR_IMP);
							do {
								d_value = strtod(token, &end_ptr);
								if (*end_ptr != '\0') {
									throw AnalyzerException(27, token);
								}
								default_file.add_threshold(OEVAR_IMP, d_value);
								token = strtok(NULL, separator);
							} while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT));
						}
					}
				}
				else if (strcmp(token, MISSED) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_property(MISSED, token);
					}
				}
				else if (strcmp(token, SEPARATOR) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_property(HEADER_SEPARATOR, token);
						default_file.add_property(DATA_SEPARATOR, token);
					}
				}
				else if (strcmp(token, VERBOSITY) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_property(VERBOSITY, token);
					}
				}
				else if (strcmp(token, MAF) == 0) {
					default_file.remove_threshold(MAF);

					token = strtok(NULL, separator);
					while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT))  {
						d_value = strtod(token, &end_ptr);
						if (*end_ptr != '\0') {
							throw AnalyzerException(27, token);
						}
						default_file.add_threshold(MAF, d_value);
						token = strtok(NULL, separator);
					}
				}
				else if (strcmp(token, IMP) == 0) {
					default_file.remove_threshold(IMP);

					token = strtok(NULL, separator);
					while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT))  {
						d_value = strtod(token, &end_ptr);
						if (*end_ptr != '\0') {
							throw AnalyzerException(27, token);
						}
						default_file.add_threshold(IMP, d_value);
						token = strtok(NULL, separator);
					}
				}
				else if (strcmp(token, BOXPLOTWIDTH) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_property(BOXPLOTWIDTH, token);
					}
				}
				else if (strcmp(token, CASESENSITIVE) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_property(CASESENSITIVE, token);
					}
				}
				else if (strcmp(token, PREFIX) == 0) {
					token = strtok(NULL, separator);

					if (token != NULL) {
						default_file.add_property(PREFIX, token);
					}
				}
				else if (strcmp(token, SNP_HQ) == 0) {
					default_file.remove_threshold(SNP_HQ);

					token = strtok(NULL, separator);
					while ((token != NULL) && (strlen(token) > 0) && (token[0] != COMMENT)) {
						d_value = strtod(token, &end_ptr);
						if (*end_ptr != '\0') {
							throw AnalyzerException(27, token);
						}
						default_file.add_threshold(SNP_HQ, d_value);
						token = strtok(NULL, separator);
					}
				}
			}
		}
	} catch(ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnalyzerException(3, file_name);
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw AnalyzerException(2, file_name);
	}

	/* Automatic column separator detection, estimation of file size, and final checks */
	if (files != NULL) {
		for (file_it = files->begin(); file_it != files->end(); file_it++) {
			if (((*file_it)->get_property(HEADER_SEPARATOR) == NULL) ||
					((*file_it)->get_property(DATA_SEPARATOR) == NULL)) {
				detect_separators(**file_it);
			}

/*			clock_t start_time = 0;
			clock_t end_time = 0;
			double execution_time = 0.0;

			start_time = clock();*/

			estimate_lines_count(**file_it);

/*			end_time = clock();
			execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;

			cout << (*file_it)->get_file_name() << " " << (*file_it)->get_estimated_lines_count();
			cout << " (" << execution_time << " sec )" << endl;*/

			check_separators(**file_it);
			check_missing_value(**file_it);
			check_verbosity_level(**file_it);
			check_filters(**file_it);
			check_thresholds(**file_it);
			check_case_sensitivity(**file_it);
			check_output_prefix(**file_it);
		}
	}

	return files;
}

/*
 *	Description:
 *		Analyzes data in a single file specified in the METAL-like script.
 *	Arguments:
 *		file	-- file to analyze.
 */
int Analyzer::process_data(File& file) throw (AnalyzerException, FileException, PlotException, MetaException) {
	vector<Meta*>* metas = NULL;
	vector<Meta*>::iterator meta_it;

	vector<MetaFiltered*>* filtered_metas = NULL;
	vector<MetaFiltered*>::iterator filtered_metas_it;

	vector<MetaRatio*>* ratio_metas = NULL;
	vector<MetaRatio*>::iterator ratio_metas_it;

	vector<MetaCrossTable*>* cross_table_metas = NULL;
	vector<MetaCrossTable*>::iterator cross_table_metas_it;

	vector< pair<pair<Meta*, Meta*>, int> >* dependencies = NULL;
	vector< pair<pair<Meta*, Meta*>, int> >::iterator dependency_it;

	ifstream ifile_stream;
	char separator = separators[2];
	char *token = NULL;
	Meta* meta = NULL;
	int columns_count = 0;
	int column_id = 0;
	int line_id = 1;
	int length = 0;
	int memory_used = 0;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	metas = file.get_metas();
	filtered_metas = file.get_filtered_metas();
	ratio_metas = file.get_ratio_metas();
	cross_table_metas = file.get_cross_table_metas();
	dependencies = file.get_dependencies();

	try {
		ifile_stream.open(file.get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw AnalyzerException(1, file.get_full_path());
	}

	try {
		process_header(file, ifile_stream);
		initialize_filters_and_crosstables(file);
		initialize_dependencies(file);

		if ((columns_count = metas->size()) > 0) {
			separator = file.get_data_separator();

			while (!ifile_stream.eof()) {
				line_id += 1;
				length = get_line(ifile_stream, line_buffer, LINE_BUFFER_SIZE);
				if (length <= 0) {
					if (!ifile_stream.eof()) {
						throw AnalyzerException(30, line_id);
					} else {
						break;
					}
				}

				column_id = 0;
				cur = line_buffer;
				token = qc_strtok(separator);
				while (token != NULL) {
					if ((meta = metas->at(column_id)) != NULL) {
						meta->put(token);
					}

					column_id += 1;
					token = qc_strtok(separator);
				}

				if (column_id != columns_count) {
					throw AnalyzerException(28, line_id);
				}

				for (filtered_metas_it = filtered_metas->begin(); filtered_metas_it != filtered_metas->end(); filtered_metas_it++) {
					(*filtered_metas_it)->put(NULL);
				}

				for (ratio_metas_it = ratio_metas->begin(); ratio_metas_it != ratio_metas->end(); ratio_metas_it++) {
					(*ratio_metas_it)->put(NULL);
				}

				for (cross_table_metas_it = cross_table_metas->begin(); cross_table_metas_it != cross_table_metas->end(); cross_table_metas_it++) {
					(*cross_table_metas_it)->put(NULL);
				}

				for (dependency_it = dependencies->begin(); dependency_it != dependencies->end(); dependency_it++) {
					if (!dependency_it->first.first->is_na() && dependency_it->first.second->is_na()) {
						dependency_it->second += 1;
					}
				}
			}
		}
	} catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw AnalyzerException(3, file.get_full_path());
		}
	} catch (std::out_of_range &e) {
		throw AnalyzerException(29, line_id);
	}

	memory_used = file.get_memory_usage();

	for (meta_it = metas->begin(); meta_it != metas->end(); meta_it++) {
		if ((*meta_it) != NULL) {
			(*meta_it)->finalize();
		}
	}

	for (filtered_metas_it = filtered_metas->begin(); filtered_metas_it != filtered_metas->end(); filtered_metas_it++) {
		(*filtered_metas_it)->finalize();
	}

	for (ratio_metas_it = ratio_metas->begin(); ratio_metas_it != ratio_metas->end(); ratio_metas_it++) {
		(*ratio_metas_it)->finalize();
	}

	for (cross_table_metas_it = cross_table_metas->begin(); cross_table_metas_it != cross_table_metas->end(); cross_table_metas_it++) {
		(*cross_table_metas_it)->finalize();
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw AnalyzerException(2, file.get_full_path());
	}

	return memory_used;
}

void Analyzer::print_result_txt(File& file) throw (AnalyzerException, FileException) {
	const char* file_extension = NULL;
	const char* output_prefix = NULL;
	char* o_textfile_name = NULL;

	ofstream o_textfile_stream;

	output_prefix = file.get_output_prefix();
	file_extension = file.get_file_extension();

	o_textfile_name = file.transform_output_name(output_prefix, TXT_EXTENSION);

//	o_textfile_name = file.transform_file_abbreviation(output_prefix, TXT_EXTENSION);
//	if (o_textfile_name == NULL) {
//		if ((file_extension != NULL) && ((sstrcmp(file_extension, TXT_EXTENSION) == 0) || (sstrcmp(file_extension, CSV_EXTENSION) == 0) ||
//				(sstrcmp(file_extension, HTML_EXTENSION) == 0) || (sstrcmp(file_extension, HTM_EXTENSION) == 0))) {
//			o_textfile_name = file.transform_file_name(output_prefix, TXT_EXTENSION, false);
//		}
//		else {
//			o_textfile_name = file.transform_file_name(output_prefix, TXT_EXTENSION, true);
//		}
//	}

/*	* OLD *
	if ((file_extension != NULL) && ((sstrcmp(file_extension, TXT_EXTENSION) == 0) || (sstrcmp(file_extension, CSV_EXTENSION) == 0) ||
			(sstrcmp(file_extension, HTML_EXTENSION) == 0) || (sstrcmp(file_extension, HTM_EXTENSION) == 0))) {
		o_textfile_name = file.transform_file_name(output_prefix, TXT_EXTENSION, false);
	}
	else {
		o_textfile_name = file.transform_file_name(output_prefix, TXT_EXTENSION, true);
	}*/

	o_textfile_stream.exceptions(ofstream::failbit | ofstream::badbit);

	try {
		o_textfile_stream.open(o_textfile_name);
	} catch (ofstream::failure &e) {
		throw AnalyzerException(1, o_textfile_name);
	}

	try {
		time_t system_time = time(NULL);

		const char* version = "GWASQC(c) v1.0";
		const char* scanned = "File scanned:\t";
		const char* date = "Date        :\t";
		char* current_time = strtok(asctime(localtime(&system_time)), "\n");

		unsigned int section = 0, subsection = 0;
		bool print = true;

		vector<Meta*>* metas = file.get_metas();
		vector<Meta*>::iterator metas_it;
		vector<MetaNumeric*>* numeric_metas = file.get_numeric_metas();
		vector<MetaCrossTable*>* cross_table_metas = file.get_cross_table_metas();
		vector< pair<pair<Meta*, Meta*>, int> >* dependencies = file.get_dependencies();
		vector<char*>* names = NULL;

		vector<char*>::iterator names_it;
		vector<MetaNumeric*>::iterator numeric_metas_it;
		vector<MetaCrossTable*>::iterator cross_table_metas_it;
		vector< pair<pair<Meta*, Meta*>, int> >::iterator dependency_it;

		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << setfill(' ') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << version << setfill(' ') << setw(76 - strlen(version)) << " #" << endl;
		o_textfile_stream << "# " << setfill(' ') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << scanned << file.get_file_name() << setfill(' ') << setw(76 - strlen(version) - strlen(file.get_file_name())) << " #" << endl;
		o_textfile_stream << "# " << date << current_time << setfill(' ') << setw(76 - strlen(date) - strlen(current_time)) << " #" << endl;
		o_textfile_stream << "# " << setfill(' ') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;

		o_textfile_stream << endl;

		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		names = file.get_missed_columns();
		if (names->empty()) {
			o_textfile_stream << " All the required columns are present" << endl;
		}
		else {
			o_textfile_stream << " The following required columns are missing or have different cases:" << endl;
			o_textfile_stream << endl;

			names_it = names->begin();
			while (names_it != names->end()) {
				o_textfile_stream << " " << *names_it << endl;
				names_it++;
			}
		}

		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		names = file.get_not_required_columns();
		if (names->empty()) {
			o_textfile_stream << " No additional columns were present" << endl;
		}
		else {
			o_textfile_stream << " The following columns do not match required column names or were not required:" << endl;
			o_textfile_stream << endl;

			names_it = names->begin();
			while (names_it != names->end()) {
				o_textfile_stream << " " << *names_it << endl;
				names_it++;
			}
		}
		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		o_textfile_stream << " Duplicates among SNPIDs:" << endl;
		o_textfile_stream << endl;

		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::MARKER) == 0) {
					(*metas_it)->print(o_textfile_stream);
					o_textfile_stream << endl;
					print = false;
					break;
				}
			}
		}

		if (print == true) {
			o_textfile_stream << " Not available" << endl << endl;
		}

		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		o_textfile_stream << " SNPs per chromosome:" << endl;
		o_textfile_stream << endl;

		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::CHR) == 0) {
					(*metas_it)->print(o_textfile_stream);
					o_textfile_stream << endl;
					print = false;
					break;
				}
			}
		}

		if (print == true) {
			o_textfile_stream << " Not available" << endl << endl;
		}

		section += 1;
		o_textfile_stream << section << ")";
		o_textfile_stream << " Allele coding" << endl;
		o_textfile_stream << endl;

		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if ((strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE1) == 0) ||
					(strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE2) == 0)) {
					o_textfile_stream << (*metas_it)->get_actual_name() << ":" << endl;
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);
					o_textfile_stream << endl;
					print = false;
				}
			}
		}

		if (print == true) {
			o_textfile_stream << " Not available" << endl << endl;
		}

		section += 1;
		o_textfile_stream << section << ")";
		o_textfile_stream << " Strand:" << endl;
		o_textfile_stream << endl;

		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::STRAND) == 0) {
					(*metas_it)->print(o_textfile_stream);
					o_textfile_stream << endl;
					print = false;
					break;
				}
			}
		}

		if (print == true) {
			o_textfile_stream << " Not available" << endl << endl;
		}

		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		o_textfile_stream << " Distribution of statistics" << endl;
		o_textfile_stream << endl;

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::N_TOTAL) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					}
					else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::FREQLABEL) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					}
					else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::OEVAR_IMP) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					}
					else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::EFFECT) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					}
					else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::STDERR) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					}
					else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::PVALUE) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					}
					else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if 	((strcmp((*metas_it)->get_common_name(), Analyzer::MARKER) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::CHR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE1) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE2) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::STRAND) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::N_TOTAL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::FREQLABEL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::OEVAR_IMP) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::EFFECT) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::STDERR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::PVALUE) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::IMPUTED) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::USED_FOR_IMP) != 0)) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					}
					else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
				}
			}
		}

		for (cross_table_metas_it = cross_table_metas->begin(); cross_table_metas_it != cross_table_metas->end(); cross_table_metas_it++) {
			subsection += 1;
			o_textfile_stream << section << "." << subsection << ")";
			o_textfile_stream << " " << (*cross_table_metas_it)->get_full_name() << endl;
			o_textfile_stream << endl;
			(*cross_table_metas_it)->print(o_textfile_stream);
			o_textfile_stream << endl;
		}

		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		o_textfile_stream << " ADDITIONAL WARNINGS:" << endl;
		o_textfile_stream << endl;

		o_textfile_stream << resetiosflags(o_textfile_stream.flags());
		o_textfile_stream << setfill(' ');

		print = true;
		for (dependency_it = dependencies->begin(); dependency_it != dependencies->end(); dependency_it++) {
			for (numeric_metas_it = numeric_metas->begin(); numeric_metas_it != numeric_metas->end(); numeric_metas_it++) {
				if (((*numeric_metas_it == dependency_it->first.first) && (!(*numeric_metas_it)->is_numeric())) ||
						((*numeric_metas_it == dependency_it->first.second) && (!(*numeric_metas_it)->is_numeric())) ){
					print = false;
					break;
				}
			}

			if (print) {
				o_textfile_stream << " -- No. of non missing " << dependency_it->first.first->get_actual_name();
				o_textfile_stream << " corresponding to missing " << setw(12) << left << dependency_it->first.second->get_actual_name();
				o_textfile_stream << " : " << dependency_it->second << endl;
			}
			else {
				print = true;
			}
		}
		o_textfile_stream << endl;

		o_textfile_stream << resetiosflags(o_textfile_stream.flags());
		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << endl;
		o_textfile_stream << "THE QUALITY CONTROL IS FINISHED" << endl;
	} catch (ofstream::failure &e) {
		throw AnalyzerException(4, o_textfile_name);
	}

	try {
		o_textfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException(2, o_textfile_name);
	}
}

void Analyzer::print_result_csv(File& file) throw (AnalyzerException, FileException) {
	const char* output_prefix = NULL;
	const char* file_extension = NULL;
	char* o_csvfile_name = NULL;

	ofstream o_csvfile_stream;

	output_prefix = file.get_output_prefix();
	file_extension = file.get_file_extension();

	/*****/

	o_csvfile_name = file.transform_output_name(output_prefix, CSV_EXTENSION);

//	o_csvfile_name = file.transform_file_abbreviation(output_prefix, CSV_EXTENSION);
//	if (o_csvfile_name == NULL) {
//		if ((file_extension != NULL) && ((sstrcmp(file_extension, TXT_EXTENSION) == 0) || (sstrcmp(file_extension, CSV_EXTENSION) == 0) ||
//				(sstrcmp(file_extension, HTML_EXTENSION) == 0) || (sstrcmp(file_extension, HTM_EXTENSION) == 0))) {
//			o_csvfile_name = file.transform_file_name(output_prefix, CSV_EXTENSION, false);
//		}
//		else {
//			o_csvfile_name = file.transform_file_name(output_prefix, CSV_EXTENSION, true);
//		}
//	}

	/*****/

/*
	* OLD *
	if ((file_extension != NULL) && ((sstrcmp(file_extension, TXT_EXTENSION) == 0) || (sstrcmp(file_extension, CSV_EXTENSION) == 0) ||
			(sstrcmp(file_extension, HTML_EXTENSION) == 0) || (sstrcmp(file_extension, HTM_EXTENSION) == 0))) {
		o_csvfile_name = file.transform_file_name(output_prefix, CSV_EXTENSION, false);
	}
	else {
		o_csvfile_name = file.transform_file_name(output_prefix, CSV_EXTENSION, true);
	}
*/

	o_csvfile_stream.exceptions(ofstream::failbit | ofstream::badbit);

	try {
		o_csvfile_stream.open(o_csvfile_name);
	} catch (ofstream::failure &e) {
		throw AnalyzerException(1, o_csvfile_name);
	}

	try {
		vector<MetaFiltered*>* filtered_metas = file.get_filtered_metas();
		vector<MetaFiltered*>::iterator filtered_metas_it;

		vector<MetaRatio*>* ratio_metas = file.get_ratio_metas();
		vector<MetaRatio*>::iterator ratio_metas_it;

		const char* separator = ";";
		bool no_column = true;

		o_csvfile_stream << setprecision(7);

		/*****************************************************************************/
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			o_csvfile_stream << separator << csv_columns[i];
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "STD_EFFECT_" << MetaRatio::slices[j];
		}
		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "N";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_source()->get_n();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "N_HQ";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_n();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "N_NAs";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_na();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "Mean";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_mean();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "StdDev";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_sd();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "Min";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_min();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}

		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "Max";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_max();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "Median";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_median();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
			o_csvfile_stream << separator << "NA";
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "Skewness";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_skew();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		ratio_metas_it = ratio_metas->begin();
		if ( (ratio_metas_it != ratio_metas->end()) &&
				( (*ratio_metas_it)->get_source_a()->is_numeric() ) &&
				( (*ratio_metas_it)->get_source_b()->is_numeric() ) ) {
			for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
				o_csvfile_stream << separator << (*ratio_metas_it)->get_skewness()[j];
			}
		}
		else {
			for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
				o_csvfile_stream << separator << "NA";
			}
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/
		o_csvfile_stream << "Kurtosis";
		for (int i = 0; i < CSV_COLUMNS_SIZE; i++) {
			no_column = true;
			filtered_metas_it = filtered_metas->begin();
			while (filtered_metas_it != filtered_metas->end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_kurtosis();
					}
					else {
						o_csvfile_stream << separator << "NA";
					}
					no_column = false;
					break;
				}
				filtered_metas_it++;
			}

			if (no_column) {
				o_csvfile_stream << separator << "NA";
			}
		}

		ratio_metas_it = ratio_metas->begin();
		if ( (ratio_metas_it != ratio_metas->end()) &&
				( (*ratio_metas_it)->get_source_a()->is_numeric() ) &&
				( (*ratio_metas_it)->get_source_b()->is_numeric() ) ) {
			for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
				o_csvfile_stream << separator << (*ratio_metas_it)->get_kurtosis()[j];
			}
		}
		else {
			for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
				o_csvfile_stream << separator << "NA";
			}
		}

		o_csvfile_stream << endl;

		/*****************************************************************************/

	} catch (ofstream::failure &e) {
		throw AnalyzerException(4, o_csvfile_name);
	}

	try {
		o_csvfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException(2, o_csvfile_name);
	}
}

void Analyzer::print_result_html(File& file, vector<Plot*>* plots, const char* resource_path) throw (AnalyzerException, FileException) {
	const char* output_prefix = NULL;
	const char* file_extension = NULL;
	char* jscript = NULL;
	char* style = NULL;
	char* o_htmlfile_name = NULL;

	ofstream o_htmlfile_stream;
	ifstream i_resource_stream;

	output_prefix = file.get_output_prefix();
	file_extension = file.get_file_extension();

	o_htmlfile_name = file.transform_output_name(output_prefix, HTML_EXTENSION);

//	o_htmlfile_name = file.transform_file_abbreviation(output_prefix, HTML_EXTENSION);
//	if (o_htmlfile_name == NULL) {
//		if ((file_extension != NULL) && ((sstrcmp(file_extension, TXT_EXTENSION) == 0) || (sstrcmp(file_extension, CSV_EXTENSION) == 0) ||
//				(sstrcmp(file_extension, HTML_EXTENSION) == 0) || (sstrcmp(file_extension, HTM_EXTENSION) == 0))) {
//			o_htmlfile_name = file.transform_file_name(output_prefix, HTML_EXTENSION, false);
//		}
//		else {
//			o_htmlfile_name = file.transform_file_name(output_prefix, HTML_EXTENSION, true);
//		}
//	}

/*	* OLD *
	if ((file_extension != NULL) && ((sstrcmp(file_extension, TXT_EXTENSION) == 0) || (sstrcmp(file_extension, CSV_EXTENSION) == 0) ||
			(sstrcmp(file_extension, HTML_EXTENSION) == 0) || (sstrcmp(file_extension, HTM_EXTENSION) == 0))) {
		o_htmlfile_name = file.transform_file_name(output_prefix, HTML_EXTENSION, false);
	}
	else {
		o_htmlfile_name = file.transform_file_name(output_prefix, HTML_EXTENSION, true);
	}*/

	jscript = (char*)malloc((strlen(resource_path) + strlen(JSCRIPT) + 1) * sizeof(char));
	if (jscript == NULL) {
		throw AnalyzerException(0, "print_result_html(File& file, ... )", "jscript", JSCRIPT);
	}
	strcpy(jscript, resource_path);
	strcat(jscript, JSCRIPT);

	style = (char*)malloc((strlen(resource_path) + strlen(CONTENT_STYLE) + 1) * sizeof(char));
	if (style == NULL) {
		throw AnalyzerException(0, "print_result_html(File& file, ... )", "style", CONTENT_STYLE);
	}
	strcpy(style, resource_path);
	strcat(style, CONTENT_STYLE);

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);
	i_resource_stream.exceptions(ifstream::failbit | ifstream::badbit);
	i_resource_stream.setf(ifstream::skipws);

	try {
		o_htmlfile_stream.open(o_htmlfile_name);
		i_resource_stream.open(jscript);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(1, o_htmlfile_name);
		}
		else {
			throw AnalyzerException(1, jscript);
		}
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">";
		o_htmlfile_stream << "<head>";
		o_htmlfile_stream << "<title>GWASQC v1.0</title>";
		o_htmlfile_stream << "<script type = \"text/javascript\">" << endl;

		while(!i_resource_stream.eof()) {
			i_resource_stream.getline(line_buffer, LINE_BUFFER_SIZE);
			o_htmlfile_stream << line_buffer << endl;
		}

		o_htmlfile_stream << "</script>";
	}
	catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(4, o_htmlfile_name);
		}
		else if (!i_resource_stream.eof()) {
			throw AnalyzerException(3, jscript);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(2, jscript);
	}

	try {
		i_resource_stream.open(style);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ifstream::failure &e) {
		throw AnalyzerException(1, style);
	}

	try {
		o_htmlfile_stream << "<style type = \"text/css\">" << endl;

		while (!i_resource_stream.eof()) {
			i_resource_stream.getline(line_buffer, LINE_BUFFER_SIZE);
			o_htmlfile_stream << line_buffer << endl;
		}

		o_htmlfile_stream << "</style>";
		o_htmlfile_stream << "</head>";
	}
	catch (fstream::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(4, o_htmlfile_name);
		}
		else if (!i_resource_stream.eof()) {
			throw AnalyzerException(3, style);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(2, style);
	}

	try {
		time_t system_time = time(NULL);
		char* current_time = asctime(localtime(&system_time));
		unsigned int section = 0, subsection = 0;
		bool print = true;

		vector<Meta*>* metas = file.get_metas();
		vector<Meta*>::iterator metas_it;
		vector<MetaNumeric*>* numeric_metas = file.get_numeric_metas();
		vector<MetaCrossTable*>* cross_table_metas = file.get_cross_table_metas();
		vector< pair<pair<Meta*, Meta*>, int> >* dependencies = file.get_dependencies();
		vector<char*>* names = NULL;

		vector<char*>::iterator names_it;
		vector<MetaNumeric*>::iterator numeric_metas_it;
		vector<MetaCrossTable*>::iterator cross_table_metas_it;
		vector< pair<pair<Meta*, Meta*>, int> >::iterator dependency_it;

		stringstream string_stream;

		o_htmlfile_stream << "<body>";

		o_htmlfile_stream << "<div class = \"header\"><div class = \"headertext\">";
		o_htmlfile_stream << "<h1>GWASQC v1.0</h1><hr />";
		o_htmlfile_stream << "<p>" << file.get_file_name() << "<br />" << current_time << "</p>";
		o_htmlfile_stream << "</div></div>";

		o_htmlfile_stream << "<div class = \"section\">";
		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";

		names = file.get_missed_columns();
		if (names->empty()) {
			o_htmlfile_stream << " All the required columns are present";
			o_htmlfile_stream << "</p>";
		}
		else {
			o_htmlfile_stream << " The following required columns are missing or have different cases:";
			o_htmlfile_stream << "</p>";
			o_htmlfile_stream << "<div class = \"groups\">";
			o_htmlfile_stream << "<table class = \"groups\">";
			o_htmlfile_stream << "<tr><th>Column Name</th></tr>";

			names_it = names->begin();
			while (names_it != names->end()) {
				o_htmlfile_stream << "<tr><td>" << *names_it << "</td></tr>";
				names_it++;
			}

			o_htmlfile_stream << "</table>";
			o_htmlfile_stream << "</div>";
		}

		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";

		names = file.get_not_required_columns();
		if (names->empty()) {
			o_htmlfile_stream << " No additional columns were present";
			o_htmlfile_stream << "</p>";
		}
		else {
			o_htmlfile_stream << " The following columns do not match required column names or were not required:";
			o_htmlfile_stream << "</p>";
			o_htmlfile_stream << "<div class = \"groups\">";
			o_htmlfile_stream << "<table class = \"groups\">";
			o_htmlfile_stream << "<tr><th>Column Name</th></tr>";

			names_it = names->begin();
			while (names_it != names->end()) {
				o_htmlfile_stream << "<tr><td>" << *names_it << "</td></tr>";
				names_it++;
			}

			o_htmlfile_stream << "</table>";
			o_htmlfile_stream << "</div>";
		}

		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";
		o_htmlfile_stream << " Duplicates among SNPIDs:";
		o_htmlfile_stream << "</p>";

		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::MARKER) == 0) {
					(*metas_it)->print_html(o_htmlfile_stream, NULL);
					print = false;
					break;
				}
			}
		}

		if (print == true) {
			o_htmlfile_stream << "<div class = \"result\">";
			o_htmlfile_stream << "Not available";
			o_htmlfile_stream << "</div>";
		}

		o_htmlfile_stream << "</div><div class = \"section\">";

		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";
		o_htmlfile_stream << " SNPs per chromosome:";
		o_htmlfile_stream << "</p>";

		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::CHR) == 0) {
					(*metas_it)->print_html(o_htmlfile_stream, NULL);
					print = false;
					break;
				}
			}
		}

		if (print == true) {
			o_htmlfile_stream << "<div class = \"result\">";
			o_htmlfile_stream << "Not available";
			o_htmlfile_stream << "</div>";
		}

		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";
		o_htmlfile_stream << " Allele coding";
		o_htmlfile_stream << "</p>";


		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if ((strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE1) == 0) ||
					(strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE2) == 0)) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ") " << (*metas_it)->get_actual_name();
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, NULL);
					o_htmlfile_stream << "</div>";
					print = false;
				}
			}
		}

		if (print == true) {
			o_htmlfile_stream << "<div class = \"result\">";
			o_htmlfile_stream << "Not available";
			o_htmlfile_stream << "</div>";
		}

		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";
		o_htmlfile_stream << " Strand:";
		o_htmlfile_stream << "</p>";

		print = true;
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::STRAND) == 0) {
					(*metas_it)->print_html(o_htmlfile_stream, NULL);
					print = false;
					break;
				}
			}
		}

		if (print == true) {
			o_htmlfile_stream << "<div class = \"result\">";
			o_htmlfile_stream << "Not available";
			o_htmlfile_stream << "</div>";
		}

		o_htmlfile_stream << "</div><div class = \"section\">";

		section += 1;
		subsection = 0;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";
		o_htmlfile_stream << " Distribution of statistics";
		o_htmlfile_stream << "</p>";

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::N_TOTAL) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					}
					else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, output_prefix);
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::FREQLABEL) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					}
					else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, output_prefix);
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::OEVAR_IMP) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					}
					else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, output_prefix);
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::EFFECT) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					}
					else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, output_prefix);
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::STDERR) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					}
					else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, output_prefix);
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Analyzer::PVALUE) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					}
					else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, output_prefix);

					print = false;
					vector<Plot*>::iterator plots_it;
					if (plots != NULL) {
						for (plots_it = plots->begin(); plots_it != plots->end(); plots_it++) {
							if (((*plots_it)->get_qqplots() != NULL) && ((*plots_it)->get_qqplots()->size() > 0)) {
								print = true;
								break;
							}
						}
					}

					if (print) {
						o_htmlfile_stream << "<div class = \"container\">";
						o_htmlfile_stream << "<div class = \"spacer\">&nbsp;</div>";

						for (plots_it = plots->begin(); plots_it != plots->end(); plots_it++) {
							if (((*plots_it)->get_qqplots() != NULL) && ((*plots_it)->get_qqplots()->size() > 0)) {
								o_htmlfile_stream << "<div class = \"float\">";
								o_htmlfile_stream << "<a href = \"" << (*plots_it)->get_name() << ".png\">";
								o_htmlfile_stream << "<img src = \"" << (*plots_it)->get_name() << ".png\" title = \"Click to enlarge the figure\" width = \"200\" height = \"200\" />";
								o_htmlfile_stream << "</a>";
								o_htmlfile_stream << "</div>";
							}
						}

						o_htmlfile_stream << "</div>";
						o_htmlfile_stream << "<div class = \"spacer\">&nbsp;</div>";
					}

					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if (*metas_it != NULL) {
				if 	((strcmp((*metas_it)->get_common_name(), Analyzer::MARKER) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::CHR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE1) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::ALLELE2) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::STRAND) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::N_TOTAL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::FREQLABEL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::OEVAR_IMP) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::EFFECT) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::STDERR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::PVALUE) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::IMPUTED) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Analyzer::USED_FOR_IMP) != 0)) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					}
					else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					(*metas_it)->print_html(o_htmlfile_stream, output_prefix);
					o_htmlfile_stream << "</div>";
				}
			}
		}

		for (cross_table_metas_it = cross_table_metas->begin(); cross_table_metas_it != cross_table_metas->end(); cross_table_metas_it++) {
			subsection += 1;
			o_htmlfile_stream << "<div class = \"subsection\">";
			o_htmlfile_stream << "<p class = \"subsectionname\">";
			o_htmlfile_stream << section << "." << subsection << ") " << (*cross_table_metas_it)->get_full_name();
			o_htmlfile_stream << "</p>";
			(*cross_table_metas_it)->print_html(o_htmlfile_stream, output_prefix);
			o_htmlfile_stream << "</div>";
		}

		o_htmlfile_stream << "</div>";

		section += 1;
		o_htmlfile_stream << "<div class = \"section\">";
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";
		o_htmlfile_stream << " ADDITIONAL WARNINGS:";
		o_htmlfile_stream << "</p>";

		print = false;
		for (dependency_it = dependencies->begin(); dependency_it != dependencies->end(); dependency_it++) {
			print = true;

			for (numeric_metas_it = numeric_metas->begin(); numeric_metas_it != numeric_metas->end(); numeric_metas_it++) {
				if (((*numeric_metas_it == dependency_it->first.first) && (!(*numeric_metas_it)->is_numeric())) ||
						((*numeric_metas_it == dependency_it->first.second) && (!(*numeric_metas_it)->is_numeric())) ){
					print = false;
					break;
				}
			}

			if (print) {
				break;
			}
		}

		if (print) {
			o_htmlfile_stream << "<div class = \"groups\">";
			o_htmlfile_stream << "<table class = \"groups\">";

			for (dependency_it = dependencies->begin(); dependency_it != dependencies->end(); dependency_it++) {
				print = true;

				for (numeric_metas_it = numeric_metas->begin(); numeric_metas_it != numeric_metas->end(); numeric_metas_it++) {
					if (((*numeric_metas_it == dependency_it->first.first) && (!(*numeric_metas_it)->is_numeric())) ||
							((*numeric_metas_it == dependency_it->first.second) && (!(*numeric_metas_it)->is_numeric())) ){
						print = false;
						break;
					}
				}

				if (print) {
					o_htmlfile_stream << "<tr>";
					o_htmlfile_stream << "<td>";
					o_htmlfile_stream << "No. of non missing " << dependency_it->first.first->get_actual_name();
					o_htmlfile_stream << " corresponding to missing " << dependency_it->first.second->get_actual_name();
					o_htmlfile_stream << "</td><td>";
					o_htmlfile_stream << dependency_it->second;
					o_htmlfile_stream << "</td>";
					o_htmlfile_stream << "</tr>";
				}
			}
			o_htmlfile_stream << "</table>";
			o_htmlfile_stream << "</div>";
		}
		else {
			o_htmlfile_stream << "<div class = \"result\">";
			o_htmlfile_stream << "None";
			o_htmlfile_stream << "</div>";
		}
		o_htmlfile_stream << "</div>";

		o_htmlfile_stream << "</body>";
		o_htmlfile_stream << "</html>";
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(4, o_htmlfile_name);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException(2, o_htmlfile_name);
	}
}

/*void Analyzer::print_summary_txt(vector<File*>& files, const char* o_file_name) throw (AnalyzerException) {
	vector<File*>::iterator files_it;
	const char* output_prefix = NULL;
	const char* file_extension = NULL;
	char* o_csvfile_name = NULL;

	ofstream o_summaryfile_stream;

	o_summaryfile_stream.exceptions(ofstream::failbit | ofstream::badbit);

	try {
		o_summaryfile_stream.open(o_file_name);
	} catch (ofstream::failure &e) {
		throw AnalyzerException(1, o_file_name);
	}

	try {
		for (files_it = files.begin(); files_it != files.end(); files_it++) {
			output_prefix = (*files_it)->get_output_prefix();
			file_extension = (*files_it)->get_file_extension();

			o_csvfile_name = (*files_it)->transform_output_name(output_prefix, CSV_EXTENSION);

			o_summaryfile_stream << o_csvfile_name << endl;
		}
	} catch (ofstream::failure &e) {
		throw AnalyzerException(4, o_file_name);
	}

	try {
		o_summaryfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException(2, o_file_name);
	}
}*/

void Analyzer::print_boxplots_html(vector<Boxplot*>& boxplots, const char* file_name, const char* resource_path) throw (AnalyzerException) {
	char* jscript = NULL;
	char* style = NULL;
	ofstream o_htmlfile_stream;
	ifstream i_resource_stream;

	jscript = (char*)malloc((strlen(resource_path) + strlen(JSCRIPT) + 1) * sizeof(char));
	if (jscript == NULL) {
		throw AnalyzerException(0, "print_boxplots_html( ... )", "jscript", JSCRIPT);
	}
	strcpy(jscript, resource_path);
	strcat(jscript, JSCRIPT);

	style = (char*)malloc((strlen(resource_path) + strlen(CONTENT_STYLE) + 1) * sizeof(char));
	if (style == NULL) {
		throw AnalyzerException(0, "print_boxplots_html( ... )", "style", CONTENT_STYLE);
	}
	strcpy(style, resource_path);
	strcat(style, CONTENT_STYLE);

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);
	i_resource_stream.exceptions(ifstream::failbit | ifstream::badbit);
	i_resource_stream.setf(ifstream::skipws);

	try {
		o_htmlfile_stream.open(file_name);
		i_resource_stream.open(jscript);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(1, file_name);
		}
		else {
			throw AnalyzerException(1, jscript);
		}
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">";
		o_htmlfile_stream << "<head>";
		o_htmlfile_stream << "<title>GWASQC v1.0</title>";
		o_htmlfile_stream << "<script type = \"text/javascript\">" << endl;

		while(!i_resource_stream.eof()) {
			i_resource_stream.getline(line_buffer, LINE_BUFFER_SIZE);
			o_htmlfile_stream << line_buffer << endl;
		}

		o_htmlfile_stream << "</script>";
	}
	catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(4, file_name);
		}
		else if (!i_resource_stream.eof()) {
			throw AnalyzerException(3, jscript);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(2, jscript);
	}

	try {
		i_resource_stream.open(style);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ifstream::failure &e) {
		throw AnalyzerException(1, style);
	}

	try {
		o_htmlfile_stream << "<style type = \"text/css\">" << endl;

		while (!i_resource_stream.eof()) {
			i_resource_stream.getline(line_buffer, LINE_BUFFER_SIZE);
			o_htmlfile_stream << line_buffer << endl;
		}

		o_htmlfile_stream << "</style>";
		o_htmlfile_stream << "</head>";
	}
	catch (fstream::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(4, file_name);
		}
		else if (!i_resource_stream.eof()) {
			throw AnalyzerException(3, style);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(2, style);
	}

	try {
		vector<Boxplot*>::iterator boxplots_it;
		time_t system_time = time(NULL);
		char* current_time = asctime(localtime(&system_time));

		int section = 1;

		o_htmlfile_stream << "<body>";

		o_htmlfile_stream << "<div class = \"header\">";
		o_htmlfile_stream << "<div class = \"headertext\">";
		o_htmlfile_stream << "<h1>GWASQC v1.0</h1>";
		o_htmlfile_stream << "<hr />";
		/*o_htmlfile_stream << "<p>Comparison Of Effect Distribution<br />";*/
		o_htmlfile_stream << "<p>Between-Study Comparison<br />";
		o_htmlfile_stream << current_time << "</p>";

		o_htmlfile_stream << "</div>";
		o_htmlfile_stream << "</div>";

		o_htmlfile_stream << "<div class = \"section\">";
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";
		o_htmlfile_stream << " Comparison Of Effect Distribution";
		o_htmlfile_stream << "</p>";
		o_htmlfile_stream << "<div class = \"container\">";
		o_htmlfile_stream << "<div class = \"spacer\">&nbsp;</div>";

		for (boxplots_it = boxplots.begin(); boxplots_it != boxplots.end(); boxplots_it++) {
			o_htmlfile_stream << "<div class = \"float\">";
			o_htmlfile_stream << "<a href = \"" << "boxplots_" << (*boxplots_it)->get_common_name() << ".png\">";
			o_htmlfile_stream << "<img src = \"" << "boxplots_" << (*boxplots_it)->get_common_name() << ".png\" title = \"Click to enlarge the figure\" width = \"400\" height = \"200\" />";
			o_htmlfile_stream << "</a>";
			o_htmlfile_stream << "</div>";
		}

		o_htmlfile_stream << "<div class = \"spacer\">&nbsp;</div>";
		o_htmlfile_stream << "</div>";
		o_htmlfile_stream << "</div>";

		o_htmlfile_stream << "</body>";
		o_htmlfile_stream << "</html>";
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(4, file_name);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException(2, file_name);
	}
}

void Analyzer::print_menu_html(vector<File*>& files, const char* boxplotsfile_name, const char* file_name, const char* resource_path) throw (AnalyzerException, FileException) {
	char* style = NULL;
	ofstream o_htmlfile_stream;
	ifstream i_resource_stream;

	style = (char*)malloc((strlen(resource_path) + strlen(MENU_STYLE) + 1) * sizeof(char));
	if (style == NULL) {
		throw AnalyzerException(0, "print_menu_html( ... )", "style", MENU_STYLE);
	}
	strcpy(style, resource_path);
	strcat(style, MENU_STYLE);

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);
	i_resource_stream.exceptions(ifstream::failbit | ifstream::badbit);
	i_resource_stream.setf(ifstream::skipws);

	try {
		o_htmlfile_stream.open(file_name);
		i_resource_stream.open(style);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	}
	catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(1, file_name);
		}
		else {
			throw AnalyzerException(1, style);
		}
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">";
		o_htmlfile_stream << "<head>";
		o_htmlfile_stream << "<title>GWASQC v1.0</title>";

		o_htmlfile_stream << "<style type = \"text/css\">" << endl;

		while (!i_resource_stream.eof()) {
			i_resource_stream.getline(line_buffer, LINE_BUFFER_SIZE);
			o_htmlfile_stream << line_buffer << endl;
		}

		o_htmlfile_stream << "</style>";
		o_htmlfile_stream << "</head>";
	}
	catch (fstream::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException(4, file_name);
		}
		else if (!i_resource_stream.eof()) {
			throw AnalyzerException(3, style);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	}
	catch (ifstream::failure &e) {
		throw AnalyzerException(2, style);
	}

	try {
		vector<File*>::iterator files_it;

		o_htmlfile_stream << "<body>";
		o_htmlfile_stream << "<div class = \"panel\">";
		o_htmlfile_stream << "<div class = \"menu\">";
		o_htmlfile_stream << "<h1>File</h1>";
		o_htmlfile_stream << "<hr />";

		for (files_it = files.begin(); files_it != files.end(); files_it++) {
			o_htmlfile_stream << "<a href = \"" << (*files_it)->transform_output_name((*files_it)->get_output_prefix(), HTML_EXTENSION) << "\" target = \"trait\">";
			if (((*files_it)->get_abbreviation() != NULL) && (strlen((*files_it)->get_abbreviation()) > 0)) {
				o_htmlfile_stream << (*files_it)->get_abbreviation();
			}
			else {
				o_htmlfile_stream << (*files_it)->get_file_name();
			}
			o_htmlfile_stream << "</a><br />";
//			o_htmlfile_stream << "<a href = \"" << (*files_it)->get_output_prefix() << (*files_it)->get_output_name() << HTML_EXTENSION << "\" target = \"trait\">";
//			o_htmlfile_stream << (*files_it)->get_file_name();
//			o_htmlfile_stream << "</a><br />";
		}

		o_htmlfile_stream << "<hr />";

		if (boxplotsfile_name != NULL) {
			o_htmlfile_stream << "<a href = \"" << boxplotsfile_name << "\" target = \"trait\">";
			o_htmlfile_stream << "Between-Study Comparisons";
			o_htmlfile_stream << "</a><br />";
		}

		o_htmlfile_stream << "</div>";
		o_htmlfile_stream << "</div>";
		o_htmlfile_stream << "</body>";
		o_htmlfile_stream << "</html>";
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(4, file_name);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException(2, file_name);
	}
}

void Analyzer::print_frameset_html(const char* file_name, const char* menu_name, File* initial_file) throw (AnalyzerException) {
	ofstream o_htmlfile_stream;

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);

	try {
		o_htmlfile_stream.open(file_name);
	}
	catch (ios_base::failure &e) {
		throw AnalyzerException(1, file_name);
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">";
		o_htmlfile_stream << "<head>";
		o_htmlfile_stream << "<title>GWASQC v1.0</title>";
		o_htmlfile_stream << "</head>";
		o_htmlfile_stream << "<frameset cols = \"240, *\" frameborder = \"no\" framespacing = \"0\">";

		o_htmlfile_stream << "<frame src=\"" << menu_name << "\" noresize=\"noresize\" scrolling=\"no\" frameborder=\"0\" marginwidth=\"0\" />";
		o_htmlfile_stream << "<frame src=\"";
		if (initial_file != NULL) {
			o_htmlfile_stream << initial_file->get_output_prefix() << initial_file->get_output_name() << HTML_EXTENSION;
		}
		o_htmlfile_stream << "\" name=\"trait\" noresize=\"noresize\" scrolling=\"auto\" frameborder=\"0\" marginwidth=\"0px\" />";

		o_htmlfile_stream << "</frameset>";
		o_htmlfile_stream << "</html>";
	}
	catch (ofstream::failure &e) {
		throw AnalyzerException(4, file_name);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException(2, file_name);
	}

}

void Analyzer::print_result_html(vector<File*>& files, vector<Boxplot*>* boxplots, const char* resource_path) throw (AnalyzerException, FileException) {
	char* o_mainfile_name = NULL;
	char* o_menufile_name = NULL;
	char* o_boxplotsfile_name = NULL;
	File* initial_file = NULL;

	o_mainfile_name = (char*)malloc((strlen(MAIN_FILE) + 1) * sizeof(char));
	if (o_mainfile_name == NULL) {
		throw AnalyzerException(0, "print_result_html(vector<File*>& files, ... )", "o_mainfile_name", MAIN_FILE);
	}
	strcpy(o_mainfile_name, MAIN_FILE);

	if ((boxplots != NULL) && (boxplots->size() > 0)) {
		o_boxplotsfile_name = (char*)malloc((strlen(BOXPLOTS_FILE) + 1) * sizeof(char));
		if (o_boxplotsfile_name == NULL) {
			throw AnalyzerException(0, "print_result_html(vector<File*>& files, ... )", "o_boxplotsfile_name", BOXPLOTS_FILE);
		}
		strcpy(o_boxplotsfile_name, BOXPLOTS_FILE);
		print_boxplots_html(*boxplots, o_boxplotsfile_name, resource_path);
	}

	o_menufile_name = (char*)malloc((strlen(MENU_FILE) + 1) * sizeof(char));
	if (o_menufile_name == NULL) {
		throw AnalyzerException(0, "print_result_html(vector<File*>& files, ... )", "o_menufile_name", MENU_FILE);
	}
	strcpy(o_menufile_name, MENU_FILE);

	if (files.size() > 0) {
		initial_file = files.front();
	}

	print_menu_html(files, o_boxplotsfile_name, o_menufile_name, resource_path);
	print_frameset_html(o_mainfile_name, o_menufile_name, initial_file);
}

vector<Boxplot*>* Analyzer::create_common_boxplots(vector<File*>& files) throw (AnalyzerException, FileException, PlotException) {
	vector<Boxplot*>* boxplots = new vector<Boxplot*>();
	Boxplot* boxplot = NULL;

	for (int i = 0; i < COMMON_BOXPLOTS_SIZE; i++) {
		boxplot = create_common_boxplot(files, common_boxplots[i][0]);

		if (boxplot != NULL) {
			boxplot->set_common_name(common_boxplots[i][0]);
			boxplot->set_title(common_boxplots[i][1]);
			boxplots->push_back(boxplot);
		}
	}

	return boxplots;
}

void Analyzer::create_common_qqplots(File& file, vector<Plot*>& plots) throw (PlotException) {
	Plot* plot = NULL;
	Qqplot* qqplot = NULL;
	const char* plot_name = NULL;
	int column_from = 0, column_to = 0;
	stringstream string_stream;

	plot_name = Analyzer::common_qqplots[column_to][0];
	while (column_to < Analyzer::COMMON_QQPLOTS_SIZE) {
		if (strcmp(plot_name, Analyzer::common_qqplots[column_to][0]) != 0) {
			qqplot = create_common_qqplot(file, column_from, column_to);

			if (qqplot != NULL) {
				plot = new Plot();
				plots.push_back(plot);
				string_stream.str("");
				string_stream << file.get_output_prefix() << file.get_output_name() << "_" << plot_name;
				plot->set_name(string_stream.str().c_str());
				plot->set_title(plot_name);
				plot->add_qqplot(qqplot);
			}

			column_from = column_to;
			plot_name = Analyzer::common_qqplots[column_to][0];
		}

		column_to++;
	}

	qqplot = create_common_qqplot(file, column_from, column_to);

	if (qqplot != NULL) {
		plot = new Plot();
		plots.push_back(plot);
		string_stream.str("");
		string_stream << file.get_output_prefix() << file.get_output_name() << "_" << plot_name;
		plot->set_name(string_stream.str().c_str());
		plot->set_title(plot_name);
		plot->add_qqplot(qqplot);
	}
}

vector<Plot*>* Analyzer::create_plots(File& file) throw (FileException, PlotException) {
	vector<Plot*>* plots = new vector<Plot*>();
	Plot* plot = NULL;
	const char* current_plot = NULL;
	const char* previous_plot = NULL;
	const char* column_name = NULL;
	vector<Plottable*>* columns = file.get_plots();
	vector<Plottable*>::iterator columns_it;
	Plottable* column = NULL;
	stringstream string_stream;

	for (int i = 0; i < SINGLE_COLUMN_PLOTS_SIZE; i++) {
		current_plot = single_column_plots[i][0];
		column_name = single_column_plots[i][1];

		if ((previous_plot == NULL) || (strcmp(current_plot, previous_plot) != 0)) {
			previous_plot = current_plot;

			plot = NULL;
			for (columns_it = columns->begin(); columns_it != columns->end(); columns_it++) {
				column = *columns_it;

				if (strcmp(column->get_plot_name(), column_name) == 0) {
					if ((column->is_plottable()) && ((column->get_boxplot() != NULL) || (column->get_histogram() != NULL))) {
						plot = new Plot();
						plots->push_back(plot);
					}

					break;
				}
			}

			if (plot != NULL) {
				string_stream.str("");

				string_stream << file.get_output_prefix() << file.get_output_name() << "_" << file.get_column(column_name);
				plot->set_name(string_stream.str().c_str());

				string_stream.str("");
				string_stream << "Distribution of " << file.get_column(column_name);
				plot->set_title(string_stream.str().c_str());

				if(column->get_histogram() != NULL) {
					plot->add_histogram(column->get_histogram());
				}

				if (column->get_boxplot() != NULL) {
					plot->add_boxplot(column->get_boxplot());
				}

				column->set_plot(plot);
			}
		}
		else if (plot != NULL) {
			for (columns_it = columns->begin(); columns_it != columns->end(); columns_it++) {
				column = *columns_it;

				if (strcmp(column->get_plot_name(), column_name) == 0) {
					if (column->is_plottable()) {
						if (column->get_histogram() != NULL) {
							plot->add_histogram(column->get_histogram());
						}

						if (column->get_boxplot() != NULL) {
							plot->add_boxplot(column->get_boxplot());
						}
					}
					break;
				}
			}
		}
	}

	create_common_qqplots(file, *plots);

	return plots;
}

Boxplot* Analyzer::create_common_boxplot(vector<File*>& files, const char* plot_name) throw (AnalyzerException, FileException, PlotException) {
	vector<Meta*>* metas = NULL;
	vector<File*>::iterator files_it;
	vector<Meta*>::iterator metas_it;
	vector<Plottable*>* plots = NULL;
	vector<Plottable*>::iterator plots_it;
	Plottable* plot = NULL;

	const char* width_column_name = NULL;
	Meta* width_column = NULL;

	Boxplot* boxplot = NULL;
	Boxplot* common_boxplot = NULL;
	vector<Boxplot*> boxplots;
	vector<Boxplot*>::iterator boxplots_it;

	double median = numeric_limits<double>::quiet_NaN();
	vector<double> medians;
	bool default_width = false;

	stringstream string_stream;
	int groups = 0;
	const char* const* names = NULL;

	for (files_it = files.begin(); files_it != files.end(); files_it++) {
		metas = (*files_it)->get_metas();

		width_column_name = (*files_it)->get_property(BOXPLOTWIDTH);
		if ((width_column_name == NULL) || (strlen(width_column_name) <= 0)) {
			default_width = true;
		}

		if (!default_width) {
			for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
				if ((*metas_it != NULL) && (strcmp((*metas_it)->get_common_name(), width_column_name) == 0)) {
					width_column = *metas_it;
					break;
				}
			}

			if ((width_column == NULL) || (isnan(median = ((MetaNumeric*)width_column)->get_median()))) {
				default_width = true;
				median = 1;
			}

			if ((default_width) && (common_boxplot != NULL)) {
				int groups = common_boxplot->get_groups_cnt();

				for (int i = 1; i <= groups; i++) {
					common_boxplot->set_width(i, 1);
				}
			}

			width_column = NULL;
		}
		else {
			median = 1;
		}

		plots = (*files_it)->get_plots();

		for (plots_it = plots->begin(); plots_it != plots->end(); plots_it++) {
			if (strcmp((*plots_it)->get_plot_name(), plot_name) == 0) {
				plot = *plots_it;
				break;
			}
		}

		if ((plot != NULL) && ((boxplot = plot->get_boxplot()) != NULL)) {
			boxplot->set_width(1, median);

			if (((*files_it)->get_abbreviation() != NULL) && (strlen((*files_it)->get_abbreviation()) > 0)) {
				boxplot->set_name(1, (*files_it)->get_abbreviation());
			}
			else {
				int length = 8;
				char* short_file_name = (char*)malloc((length + 1) * sizeof(char));
				if (short_file_name == NULL) {
					throw AnalyzerException(0, "create_common_boxplot( ... )", "short_file_name", (length + 1) * sizeof(char));
				}
				strncpy(short_file_name, (*files_it)->get_file_name(), length);
				short_file_name[length] = '\0';
				boxplot->set_name(1, short_file_name);
			}

			/* *OLD*
			boxplot->set_name(1, (*files_it)->get_abbreviation());
			*/

			boxplots.push_back(boxplot);
			medians.push_back(median);

			plot = NULL;
		}
	}

	stable_sort(boxplots.begin(), boxplots.end(), Analyzer::compare_boxplots);
	stable_sort(medians.begin(), medians.end());
	for (boxplots_it = boxplots.begin(); boxplots_it != boxplots.end(); boxplots_it++) {
		common_boxplot = Boxplot::merge(common_boxplot, *boxplots_it);
	}

	if ((!default_width) && (common_boxplot != NULL)) {
		groups = common_boxplot->get_groups_cnt();
		names = common_boxplot->get_names();

		for (int i = 1; i <= groups; i++) {
			common_boxplot->set_width(i, medians.at(i - 1));
			string_stream << names[i - 1] << " " << medians.at(i - 1);
			common_boxplot->set_name(i, string_stream.str().c_str());
			string_stream.str("");
		}
	}

	boxplots.clear();

	return common_boxplot;
}

Qqplot* Analyzer::create_common_qqplot(File& file, int column_from, int column_to) throw (PlotException) {
	Qqplot* qqplot = NULL;
	Qqplot* common_qqplot = NULL;
	vector<Plottable*>* plots = file.get_plots();
	vector<Plottable*>::iterator plots_it = plots->begin();

	int i = column_from;

	while (i < column_to) {
		while (plots_it != plots->end()) {
			if (strcmp((*plots_it)->get_plot_name() , Analyzer::common_qqplots[i][1]) == 0) {
				if ((qqplot = (*plots_it)->get_qqplot()) != NULL) {
					common_qqplot = Qqplot::merge(common_qqplot, qqplot);
				}

				break;
			}

			plots_it++;
		}

		i += 1;
		plots_it = plots->begin();
	}

	return common_qqplot;
}

int Analyzer::get_line(ifstream& ifile_stream, char* line, int length) throw (ifstream::failure) {
	int i = 0;
	char c = '\0';

	try {
		while (!ifile_stream.eof() && (i < length)) {
			ifile_stream.get(line[i]);

			if (line[i] == '\n') {
				break;
			}
			else if (line[i] == '\r') {
				if (!ifile_stream.eof()) {
					c = ifile_stream.peek();
					if (c == '\n') {
						ifile_stream.ignore();
					}
				}
				break;
			}

			i += 1;
		}
	}
	catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw &e;
		}
	}

	line[i] = '\0';

	return i;
}

/*
 *	Description:
 *		Automatically detects column separators in the specified file. For the
 *		detection uses header and next 10 file rows (CHECK_ROW_COUNT constant).
 *	Arguments:
 *		file	-- file to process.
 */
void Analyzer::detect_separators(File& file) throw (AnalyzerException) {
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

	try {
		ifile_stream.open(file.get_full_path());
		ifile_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw AnalyzerException(1, file.get_full_path());
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
			length = get_line(ifile_stream, line_buffer, LINE_BUFFER_SIZE);

			if (length > 0) {
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
			length = get_line(ifile_stream, line_buffer, LINE_BUFFER_SIZE);

			if (length > 0) {
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
		while ((!ifile_stream.eof()) && (row < CHECK_ROW_COUNT_FOR_SEPARATOR)) {
			length = get_line(ifile_stream, line_buffer, LINE_BUFFER_SIZE);

			if (length > 0) {
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
			throw AnalyzerException(3, file.get_full_path());
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
					throw AnalyzerException(6);
				}
				i += 1;
			}
		}
		else if (matches > 1) {
			throw AnalyzerException(6);
		}
	}

	switch (header_separator) {
		case 0:
			file.add_property(HEADER_SEPARATOR, Analyzer::COMMA);
			break;
		case 1:
			file.add_property(HEADER_SEPARATOR, Analyzer::TAB);
			break;
		case 2:
			file.add_property(HEADER_SEPARATOR, Analyzer::WHITESPACE);
			break;
		default:
			file.add_property(HEADER_SEPARATOR, Analyzer::SEMICOLON);
			break;
	}

	switch (data_separator) {
		case 0:
			file.add_property(DATA_SEPARATOR, Analyzer::COMMA);
			break;
		case 1:
			file.add_property(DATA_SEPARATOR, Analyzer::TAB);
			break;
		case 2:
			file.add_property(DATA_SEPARATOR, Analyzer::WHITESPACE);
			break;
		default:
			file.add_property(DATA_SEPARATOR, Analyzer::SEMICOLON);
			break;
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw AnalyzerException(2, file.get_full_path());
	}
}

void Analyzer::process_header(File& file, ifstream& ifile_stream) throw (ifstream::failure, AnalyzerException, MetaException) {
	vector<Meta*>* metas = file.get_metas();
	vector<MetaNumeric*>* numeric_metas = file.get_numeric_metas();
	vector<Plottable*>* plots = file.get_plots();
	vector<char*>* missed_columns = file.get_missed_columns();
	vector<char*>* not_required_columns = file.get_not_required_columns();
	vector<char*>::iterator name_it;

	char* token = NULL;
	const char* common_name = NULL;
	const char* actual_name = NULL;
	char* name = NULL;
	Meta* meta = NULL;
	int column_position = 0;
	int length = 0;

	const char* missing_value = NULL;
	const char* verbosity = NULL;
	char separator = separators[2];

	vector<double>* thresholds = NULL;

	if (!ifile_stream.eof()) {
		separator = file.get_header_separator();
		missing_value = file.get_property(MISSED);
		verbosity = file.get_property(VERBOSITY);

		for (int i = 0; i < MANDATORY_COLUMNS_SIZE; i++) {
			actual_name = file.get_column(mandatory_columns[i]);
			if (actual_name != NULL) {
				name = (char*)malloc((strlen(actual_name) + 1) * sizeof(char));
				if (name == NULL) {
					throw AnalyzerException(0, "process_header( ... )", "actual_name", actual_name);
				}
				strcpy(name, actual_name);
				missed_columns->push_back(name);
			}
		}

/*		* OLD *
		ifile_stream.getline(line_buffer, LINE_BUFFER_SIZE);

		length = ifile_stream.gcount() - 2;
		while ((length >= 0) && (line_buffer[length] == '\r')) {
			line_buffer[length] = '\0';
			length -= 1;
		}*/

		length = get_line(ifile_stream, line_buffer, LINE_BUFFER_SIZE);
		if (length <= 0) {
			cout << "Error: empty header!" << endl;
		}

		cur = line_buffer;
		token = qc_strtok(separator);
		while (token != NULL) {
			if ((common_name = file.get_common_column(token)) != NULL) {
				if (strcmp(common_name, MARKER) == 0) {
					meta = new MetaUniqueness(file.get_estimated_lines_count());
				}
				else if ((strcmp(common_name, ALLELE1) == 0) || (strcmp(common_name, ALLELE2) == 0) ||
						(strcmp(common_name, CHR) == 0) || strcmp(common_name, STRAND) == 0) {
					meta = new MetaGroup();
				}
				else if (strcmp(common_name, STDERR) == 0) {
					thresholds = file.get_threshold(STDERR);

					meta = new MetaImplausible(thresholds->at(0), thresholds->at(1), file.get_estimated_lines_count());

					numeric_metas->push_back((MetaNumeric*)meta);
					((MetaImplausible*)meta)->set_description("All data");
					((MetaImplausible*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Standard error");
					plots->push_back((MetaImplausible*)meta);
				}
				else if (strcmp(common_name, OEVAR_IMP) == 0) {
					thresholds = file.get_threshold(OEVAR_IMP);

					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), file.get_estimated_lines_count());

					numeric_metas->push_back((MetaNumeric*)meta);
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Imputation quality");
					plots->push_back((MetaImplausible*)meta);
				}
				else if (strcmp(common_name, PVALUE) == 0) {
					thresholds = file.get_threshold(PVALUE);

					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), file.get_estimated_lines_count());

					numeric_metas->push_back((MetaNumeric*)meta);
					((MetaImplausibleStrict*)meta)->set_plots(true, false, false);
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("P-value");

					plots->push_back((MetaImplausibleStrict*)meta);
				}
				else if (strcmp(common_name, FREQLABEL) == 0) {
					thresholds = file.get_threshold(FREQLABEL);

					meta = new MetaImplausibleStrictAdjusted(thresholds->at(0), thresholds->at(1), 0.5, file.get_estimated_lines_count());

					numeric_metas->push_back((MetaNumeric*)meta);
					((MetaImplausibleStrictAdjusted*)meta)->set_description(token);
					((MetaImplausibleStrictAdjusted*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Minor allele frequency");
					plots->push_back((MetaImplausibleStrictAdjusted*)meta);
				}
				else if	(strcmp(common_name, HWE_PVAL) == 0) {
					thresholds = file.get_threshold(HWE_PVAL);

					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), file.get_estimated_lines_count());

					if (strcmp(verbosity, VERBOSITY_LOW) == 0) {
						((MetaImplausibleStrict*)meta)->set_plots(false, false, false);
					}
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Hardy-Weinberg equilibrium p-value");

					numeric_metas->push_back((MetaNumeric*)meta);
					plots->push_back((MetaImplausibleStrict*)meta);

				}
				else if (strcmp(common_name, CALLRATE) == 0) {
					thresholds = file.get_threshold(CALLRATE);

					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), file.get_estimated_lines_count());

					if (strcmp(verbosity, VERBOSITY_LOW) == 0) {
						((MetaImplausibleStrict*)meta)->set_plots(false, false, false);
					}
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Genotyping callrate");

					numeric_metas->push_back((MetaNumeric*)meta);
					plots->push_back((MetaImplausibleStrict*)meta);
				}
				else if ((strcmp(common_name, EFFECT) == 0)) {
					meta = new MetaNumeric(file.get_estimated_lines_count());
					numeric_metas->push_back((MetaNumeric*)meta);
					((MetaNumeric*)meta)->set_description("All data");
					((MetaNumeric*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Effect size");
					plots->push_back((MetaNumeric*)meta);
				}
				else if ((strcmp(common_name, N_TOTAL) == 0)) {
					meta = new MetaNumeric(file.get_estimated_lines_count());

					if (strcmp(verbosity, VERBOSITY_LOW) == 0) {
						((MetaNumeric*)meta)->set_plots(true, false, false);
					}
					((MetaNumeric*)meta)->set_description("All data");
					((MetaNumeric*)meta)->set_ouptut(true, true, true, false, false, false);
					meta->set_full_name("Sample size");

					numeric_metas->push_back((MetaNumeric*)meta);
					plots->push_back((MetaNumeric*)meta);
				}
				else if  (strcmp(common_name, IMPUTED) == 0) {
					meta = new MetaNumeric(file.get_estimated_lines_count());

					if (strcmp(verbosity, VERBOSITY_LOW) == 0) {
						((MetaNumeric*)meta)->set_plots(false, false, false);
					}
					((MetaNumeric*)meta)->set_description("All data");

					numeric_metas->push_back((MetaNumeric*)meta);
					plots->push_back((MetaNumeric*)meta);
				}
				else if (strcmp(common_name, USED_FOR_IMP) == 0) {
					meta = new MetaNumeric(file.get_estimated_lines_count());

					if (strcmp(verbosity, VERBOSITY_LOW) == 0) {
						((MetaNumeric*)meta)->set_plots(false, false, false);
					}
					((MetaNumeric*)meta)->set_description("All data");

					numeric_metas->push_back((MetaNumeric*)meta);
					plots->push_back((MetaNumeric*)meta);
				}
				else if ((strcmp(common_name, POSITION) == 0)) {
					meta = new MetaFormat();
					meta->set_full_name("Position");
				}
				else {
					throw AnalyzerException(5, token);
				}

				if (file.is_case_sensitive()) {
					for (name_it = missed_columns->begin(); name_it != missed_columns->end(); name_it++) {
						if (strcmp(*name_it, token) == 0) {
							free(*name_it);
							missed_columns->erase(name_it);
							break;
						}
					}
				}
				else {
					for (name_it = missed_columns->begin(); name_it != missed_columns->end(); name_it++) {
						if (sstrcmp(*name_it, token) == 0) {
							free(*name_it);
							missed_columns->erase(name_it);
							break;
						}
					}
				}

				meta->set_common_name(common_name);
				meta->set_actual_name(token);
				meta->set_id(column_position);
				meta->set_na_marker(missing_value);
				metas->push_back(meta);
			}
			else {
				name = (char*)malloc((strlen(token) + 1) * sizeof(char));
				if (name == NULL) {
					throw AnalyzerException(0, "process_header( ... )", "token", token);
				}
				strcpy(name, token);
				not_required_columns->push_back(name);
				metas->push_back(NULL);
			}

			token = qc_strtok(separator);
			column_position++;
		}
	}
}

void Analyzer::initialize_filters_and_crosstables(File& file) throw (FileException, PlotException, MetaException) {
	vector<MetaNumeric*>* numeric_metas = file.get_numeric_metas();
	vector<MetaNumeric*>::iterator numeric_metas_it;

	vector<MetaFiltered*>* filtered_metas = file.get_filtered_metas();
	MetaFiltered* filtered_meta = NULL;

	vector<MetaRatio*>* ratio_metas = file.get_ratio_metas();
	MetaRatio* ratio_meta = NULL;

	vector<MetaCrossTable*>* cross_table_metas = file.get_cross_table_metas();
	MetaCrossTable* cross_table_meta = NULL;

	vector<Plottable*>* plots = file.get_plots();

	MetaNumeric* effect = NULL;
	MetaNumeric* freqlabel = NULL;
	MetaNumeric* oevar_imp = NULL;
	MetaNumeric* pvalue = NULL;
	MetaNumeric* se = NULL;
	MetaNumeric* imputed = NULL;
	MetaNumeric* used_for_imp = NULL;
	MetaFiltered* effect_hq = NULL;
	MetaFiltered* se_hq = NULL;

	vector<double>* maf_levels = file.get_threshold(MAF);
	vector<double>* imp_levels = file.get_threshold(IMP);
	vector<double>* snp_hq = file.get_threshold(SNP_HQ);

	stringstream string_stream;

	numeric_metas_it = numeric_metas->begin();
	while (numeric_metas_it != numeric_metas->end()) {
		if (*numeric_metas_it != NULL) {
			if (strcmp((*numeric_metas_it)->get_common_name(), Analyzer::EFFECT) == 0) {
				effect = *numeric_metas_it;
			}
			else if (strcmp((*numeric_metas_it)->get_common_name(), Analyzer::FREQLABEL) == 0) {
				freqlabel = *numeric_metas_it;
			}
			else if (strcmp((*numeric_metas_it)->get_common_name(), Analyzer::OEVAR_IMP) == 0) {
				oevar_imp = *numeric_metas_it;
			}
			else if (strcmp((*numeric_metas_it)->get_common_name(), Analyzer::PVALUE) == 0) {
				pvalue = *numeric_metas_it;
			}
			else if (strcmp((*numeric_metas_it)->get_common_name(), Analyzer::STDERR) == 0) {
				se = *numeric_metas_it;
			}
			else if (strcmp((*numeric_metas_it)->get_common_name(), Analyzer::IMPUTED) == 0) {
				imputed = *numeric_metas_it;
			}
			else if (strcmp((*numeric_metas_it)->get_common_name(), Analyzer::USED_FOR_IMP) == 0) {
				used_for_imp = *numeric_metas_it;
			}
		}

		numeric_metas_it++;
	}

	if ((freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
		numeric_metas_it = numeric_metas->begin();
		while (numeric_metas_it != numeric_metas->end()) {
			if (*numeric_metas_it != NULL) {
				filtered_meta = new MetaFiltered(*numeric_metas_it, file.get_estimated_lines_count());
				filtered_meta->set_common_name((*numeric_metas_it)->get_common_name());
				if (strcmp((*numeric_metas_it)->get_common_name(), FREQLABEL) == 0) {
					filtered_meta->set_actual_name("MAF");
				}
				else {
					filtered_meta->set_actual_name((*numeric_metas_it)->get_common_name());
				}
				filtered_meta->set_plots(false, false, false);

				filtered_meta->add_dependency(*freqlabel, *(new Gt(snp_hq->at(0))));
				filtered_meta->add_dependency(*oevar_imp, *(new Gt(snp_hq->at(1))));
				filtered_meta->add_dependency(*se, *(new Gt(0)));

				filtered_metas->push_back(filtered_meta);

				if (*numeric_metas_it == se) {
					se_hq = filtered_meta;
				}
				else if (*numeric_metas_it == effect) {
					effect_hq = filtered_meta;
				}
			}

			numeric_metas_it++;
		}
	}

	if ((se_hq != NULL) && (effect_hq != NULL)) {
		ratio_meta = new MetaRatio(effect_hq, se_hq);
		ratio_metas->push_back(ratio_meta);
	}

	if ((effect != NULL) && (freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
		filtered_meta = new MetaFiltered(effect, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::EFFECT_HQ);
		filtered_meta->set_actual_name(Analyzer::EFFECT_HQ);
		filtered_meta->set_plots(false, true, false);

		string_stream.str("");
		string_stream << "MAF > " << maf_levels->at(0) << " and IMP > " << imp_levels->at(0);
		filtered_meta->set_description(string_stream.str().c_str());

		filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(0))));
		filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(0))));
		filtered_meta->add_dependency(*se, *(new Gt(0)));

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if (pvalue != NULL) {
		filtered_meta = new MetaFiltered(pvalue, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::PVALUE_FROM0TO1);
		filtered_meta->set_actual_name(Analyzer::PVALUE_FROM0TO1);
		filtered_meta->set_plots(false, false, true);
		filtered_meta->set_description("Unfiltered");
		filtered_meta->set_color("black");

		filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if ((pvalue != NULL) && (freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
		filtered_meta = new MetaFiltered(pvalue, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::PVALUE_HQ_1);
		filtered_meta->set_actual_name(Analyzer::PVALUE_HQ_1);
		filtered_meta->set_plots(true, false, true);

		string_stream.str("");
		string_stream << "MAF > " << maf_levels->at(0) << " and IMP > " << imp_levels->at(0);
		filtered_meta->set_description(string_stream.str().c_str());
		filtered_meta->set_color("red");

		filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(0))));
		filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(0))));
		filtered_meta->add_dependency(*se, *(new Gt(0)));

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if ((pvalue != NULL) && (freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
		filtered_meta = new MetaFiltered(pvalue, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::PVALUE_HQ_2);
		filtered_meta->set_actual_name(Analyzer::PVALUE_HQ_2);
		filtered_meta->set_plots(false, false, true);

		string_stream.str("");
		string_stream << "MAF > " << maf_levels->at(1) << " and IMP > " << imp_levels->at(1);
		filtered_meta->set_description(string_stream.str().c_str());
		filtered_meta->set_color("blue");

		filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(1))));
		filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(1))));
		filtered_meta->add_dependency(*se, *(new Gt(0)));

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if ((pvalue != NULL) && (freqlabel != NULL) && (se != NULL) && (effect != NULL)) {
		filtered_meta = new MetaFiltered(pvalue, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::PVALUE_MAF_1);
		filtered_meta->set_actual_name(Analyzer::PVALUE_MAF_1);
		filtered_meta->set_plots(false, false, true);

		string_stream.str("");
		string_stream << "MAF > " << maf_levels->at(0);
		filtered_meta->set_description(string_stream.str().c_str());
		filtered_meta->set_color("red");

		filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
		filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(0))));
		filtered_meta->add_dependency(*se, *(new Gt(0)));
		filtered_meta->add_dependency(*effect);

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if ((pvalue != NULL) && (freqlabel != NULL) && (se != NULL) && (effect != NULL)) {
		filtered_meta = new MetaFiltered(pvalue, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::PVALUE_MAF_2);
		filtered_meta->set_actual_name(Analyzer::PVALUE_MAF_2);
		filtered_meta->set_plots(false, false, true);

		string_stream.str("");
		string_stream << "MAF > " << maf_levels->at(1);
		filtered_meta->set_description(string_stream.str().c_str());
		filtered_meta->set_color("blue");

		filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
		filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(1))));
		filtered_meta->add_dependency(*se, *(new Gt(0)));
		filtered_meta->add_dependency(*effect);

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if ((pvalue != NULL) && (oevar_imp != NULL) && (se != NULL) && (effect != NULL)) {
		filtered_meta = new MetaFiltered(pvalue, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::PVALUE_IMP_1);
		filtered_meta->set_actual_name(Analyzer::PVALUE_IMP_1);
		filtered_meta->set_plots(false, false, true);

		string_stream.str("");
		string_stream << "IMP > " << imp_levels->at(0);
		filtered_meta->set_description(string_stream.str().c_str());
		filtered_meta->set_color("red");

		filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
		filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(0))));
		filtered_meta->add_dependency(*se, *(new Gt(0)));
		filtered_meta->add_dependency(*effect);

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if ((pvalue != NULL) && (oevar_imp != NULL) && (se != NULL) && (effect != NULL)) {
		filtered_meta = new MetaFiltered(pvalue, file.get_estimated_lines_count());
		filtered_meta->set_common_name(Analyzer::PVALUE_IMP_2);
		filtered_meta->set_actual_name(Analyzer::PVALUE_IMP_2);
		filtered_meta->set_plots(false, false, true);

		string_stream.str("");
		string_stream << "IMP > " << imp_levels->at(1);
		filtered_meta->set_description(string_stream.str().c_str());
		filtered_meta->set_color("blue");

		filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
		filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(1))));
		filtered_meta->add_dependency(*se, *(new Gt(0)));
		filtered_meta->add_dependency(*effect);

		filtered_metas->push_back(filtered_meta);
		plots->push_back(filtered_meta);
	}

	if ((imputed != NULL) && (used_for_imp != NULL)) {
		cross_table_meta = new MetaCrossTable(imputed, used_for_imp);
		cross_table_meta->set_full_name("SNPs used for imputation vs. imputed SNPs");
		cross_table_metas->push_back(cross_table_meta);
	}
}

void Analyzer::initialize_dependencies(File& file) {
	vector<Meta*>* metas = file.get_metas();
	vector< pair<pair<Meta*, Meta*>, int> >* dependencies = file.get_dependencies();
	vector<Meta*>::iterator metas_it;
	Meta* left_dependant = NULL;
	Meta* right_dependant = NULL;

	for (int i = 0; i < DEPENDENCIES_SIZE; i++) {
		for (metas_it = metas->begin(); metas_it != metas->end(); metas_it++) {
			if ((*metas_it) != NULL) {
				if ((left_dependant == NULL) && (strcmp((*metas_it)->get_common_name(), Analyzer::dependencies[i][0]) == 0)) {
					left_dependant = *metas_it;
				}

				if ((right_dependant == NULL) && (strcmp((*metas_it)->get_common_name(), Analyzer::dependencies[i][1]) == 0)) {
					right_dependant = *metas_it;
				}

				if ((left_dependant != NULL) && (right_dependant != NULL)) {
					dependencies->push_back(pair<pair<Meta*, Meta*>, int>(pair<Meta*, Meta*>(left_dependant, right_dependant), 0));
					break;
				}
			}
		}

		left_dependant = NULL;
		right_dependant = NULL;
	}
}

char* Analyzer::qc_strtok(const char sep) {
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

int Analyzer::sstrcmp(const char* first, const char* second) {
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

int Analyzer::compare (const void* first, const void* second) {
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

bool Analyzer::compare_boxplots(Boxplot* first, Boxplot* second) {
	const double* first_widths = first->get_widths();
	const double* second_widths = second->get_widths();

	if ((first_widths != NULL) && (second_widths != NULL) && (first_widths[0] < second_widths[0])) {
		return true;
	}

	return false;
}

/*
 *	Description:
 *		Checks if column separators were specified (SEPARATOR command) for the
 *		file in the METAL-like script.
 *	Arguments:
 *		file	-- file to check.
 */
void Analyzer::check_separators(File& file) throw (AnalyzerException, FileException) {
	const char* separator_name = NULL;

	separator_name = file.get_property(HEADER_SEPARATOR);
	if (separator_name == NULL) {
		throw AnalyzerException(7);
	}
	else if (sstrcmp(separator_name, COMMA) == 0) {
		file.set_header_separator(separators[0]);
	}
	else if (sstrcmp(separator_name, TAB) == 0) {
		file.set_header_separator(separators[1]);
	}
	else if (sstrcmp(separator_name, WHITESPACE) == 0) {
		file.set_header_separator(separators[2]);
	}
	else if (sstrcmp(separator_name, SEMICOLON) == 0) {
		file.set_header_separator(separators[3]);
	}
	else {
		throw AnalyzerException(8, separator_name);
	}

	separator_name = file.get_property(DATA_SEPARATOR);
	if (separator_name == NULL) {
		throw AnalyzerException(9);
	}
	if (sstrcmp(separator_name, COMMA) == 0) {
		file.set_data_separator(separators[0]);
	}
	else if (sstrcmp(separator_name, TAB) == 0) {
		file.set_data_separator(separators[1]);
	}
	else if (sstrcmp(separator_name, WHITESPACE) == 0) {
		file.set_data_separator(separators[2]);
	}
	else if (sstrcmp(separator_name, SEMICOLON) == 0) {
		file.set_data_separator(separators[3]);
	}
	else {
		throw AnalyzerException(10, separator_name);
	}
}

/*
 *	Description:
 *		Checks if MISSED command was specified for the file in the
 *		METAL-like script.
 *	Arguments:
 *		file	-- file to check.
 */
void Analyzer::check_missing_value(File& file) throw (AnalyzerException, FileException) {
	const char* missing_value = file.get_property(MISSED);

	if (missing_value == NULL) {
		throw AnalyzerException(11);
	}
	else if (strchr(missing_value, file.get_data_separator()) != NULL) {
		throw AnalyzerException(12);
	}
}

/*
 *	Description:
 *		Checks if VERBOSITY command was specified for the file in the
 *		METAL-like script.
 *	Arguments:
 *		file	-- file to check.
 */
void Analyzer::check_verbosity_level(File& file) throw (AnalyzerException, FileException) {
	const char* verbosity_name = file.get_property(VERBOSITY);

	if (verbosity_name == NULL) {
		file.add_property(VERBOSITY, VERBOSITY_LOW);
	}
	else if ((strcmp(verbosity_name, VERBOSITY_LOW) != 0) && (strcmp(verbosity_name, VERBOSITY_HIGH) != 0)) {
		throw AnalyzerException(13);
	}
}

/*
 * 	Description:
 * 		Checks if MAF (minor allele frequency), IMP (imputation quality) and
 * 		SNP_HQ (high quality SNPs) were specified for the file in METAL-like
 * 		script.
 *	Arguments:
 *		file	-- file to check.
 */
void Analyzer::check_filters(File& file) throw (AnalyzerException, FileException) {
	vector<double>* maf_levels = NULL;
	vector<double>* imp_levels = NULL;
	vector<double>* snp_hq = NULL;
	double swap_value = 0.0;

	maf_levels = file.get_threshold(MAF);
	if ((maf_levels == NULL) || (maf_levels->size() <= 0)) {
		throw AnalyzerException(14);
	}

	if (maf_levels->size() < 2) {
		throw AnalyzerException(15);
	}

	if (maf_levels->size() > 2) {
		throw AnalyzerException(16);
	}

	imp_levels = file.get_threshold(IMP);
	if ((imp_levels == NULL) || (imp_levels->size() <= 0)) {
		throw AnalyzerException(17);
	}

	if (imp_levels->size() < 2) {
		throw AnalyzerException(18);
	}

	if (imp_levels->size() > 2) {
		throw AnalyzerException(19);
	}

	if (maf_levels->at(0) > maf_levels->at(1)) {
		swap_value = maf_levels->at(0);
		maf_levels->at(0) = maf_levels->at(1);
		maf_levels->at(1) = swap_value;
	}

	if (imp_levels->at(0) > imp_levels->at(1)) {
		swap_value = imp_levels->at(0);
		imp_levels->at(0) = imp_levels->at(1);
		imp_levels->at(1) = swap_value;
	}

	snp_hq = file.get_threshold(SNP_HQ);
	if ((snp_hq == NULL) || (snp_hq->size() <= 0)) {
		throw AnalyzerException(20);
	}

	if (snp_hq->size() < 2) {
		throw AnalyzerException(21);
	}

	if (snp_hq->size() > 2) {
		throw AnalyzerException(22);
	}
}

/*
 *	Description:
 * 		Checks if columns' threshold for the file were specified in the
 * 		METAL-like script.
 * 	Arguments:
 * 		file	-- file to check.
 */
void Analyzer::check_thresholds(File& file) throw (AnalyzerException, FileException) {
	vector<double>* thresholds = NULL;
	double swap_value = 0.0;

	for (int i = 0; i < THRESHOLD_COLUMNS_SIZE; i++) {
		thresholds = file.get_threshold(threshold_columns[i]);

		if ((thresholds == NULL) || (thresholds->size() <= 0)) {
			throw AnalyzerException(23, threshold_columns[i]);
		}
		else if (thresholds->size() < 2) {
			throw AnalyzerException(24, threshold_columns[i]);
		}
		else if (thresholds->size() > 2) {
			throw AnalyzerException(25, threshold_columns[i]);
		}

		if (thresholds->at(0) > thresholds->at(1)) {
			swap_value = thresholds->at(0);
			thresholds->at(0) = thresholds->at(1);
			thresholds->at(1) = swap_value;
		}
	}
}

/*
 * 	Description:
 *		Checks if command CASESENSITIVE was set for the file in the
 *		METAL-like input script.
 *	Arguments:
 *		file 	-- file to check.
 */
void Analyzer::check_case_sensitivity(File& file) throw (FileException) {
	const char* case_sensitive = file.get_property(CASESENSITIVE);

	if ((case_sensitive != NULL) && (strcmp(case_sensitive, "1") == 0)) {
		file.set_case_sensitive(true);
	}
	else {
		file.set_case_sensitive(false);
	}
}

/*
 * 	Description:
 *		Checks if outputs' prefix was specified for the file in the
 *		METAL-like input script.
 *	Arguments:
 *		file	-- file to check.
 */
void Analyzer::check_output_prefix(File& file) throw (AnalyzerException, FileException) {
	const char* prefix = file.get_property(PREFIX);

	if ((prefix != NULL) && (strlen(prefix) > 0)) {
		file.set_output_prefix(prefix);
	}
	else {
		throw AnalyzerException(26, file.get_full_path());
	}
}

/*
 * 	Description:
 * 		Estimates the number of lines in the input file. For the estimation uses
 * 		CHECK_SAMPLE_COUNT_FOR_SIZE samples consisting of CHECK_ROW_COUNT_FOR_SIZE
 * 		random file lines.
 *	Arguments:
 *		file 	-- file for estimation.
 */
void Analyzer::estimate_lines_count(File& file) throw (AnalyzerException) {
	ifstream ifile_stream;
	int file_length = 0;
	int current_pos = 0;
	int header_length = 0;
	char character = '\0';
	int chars_count = 0;
	double location = 0.0;
	vector<double> medians;
	int lines_count = 0;
	double mean = 0.0;
	double median = 0.0;
	int estimated_lines_count = 0;
	int denominator = CHECK_ROW_COUNT_FOR_SIZE * 10;

	multiset<int> line_lengths;
	multiset<int>::iterator it;
	int pos = 0;

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	try {
		ifile_stream.open(file.get_full_path(), ios::binary);
	} catch (ifstream::failure &e) {
		throw AnalyzerException(1, file.get_full_path());
	}

	try {
		ifile_stream.seekg((streampos)0, ifstream::end);
		file_length = ifile_stream.tellg();

		ifile_stream.seekg((streampos)0, ifstream::beg);
		current_pos = ifile_stream.tellg();

		while (!ifile_stream.eof()) {
			ifile_stream.get(character);
			chars_count += 1;
			if (character == '\n') {
				header_length = chars_count;
				chars_count = 0;
				break;
			}
		}
	} catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw &e;
		} else {
			ifile_stream.clear();
			header_length = chars_count;
		}
	}

	if (header_length < file_length) {
		srand(time(NULL));

		for (int j = 0; j < CHECK_SAMPLE_COUNT_FOR_SIZE; j++) {
			for (int i = 0; i < CHECK_ROW_COUNT_FOR_SIZE; i++) {
				location = (rand() % denominator) / (double)denominator;
				current_pos = (int)(file_length * location);

				try {
					ifile_stream.seekg((streampos)current_pos, ifstream::beg);

					while (!ifile_stream.eof()) {
						ifile_stream.get(character);
						if (character == '\n') {
							break;
						}
					}

					while (!ifile_stream.eof()) {
						ifile_stream.get(character);
						chars_count += 1;
						if (character == '\n') {
							line_lengths.insert(chars_count);
							chars_count = 0;
							break;
						}
					}

				}
				catch (ifstream::failure &e) {
					if (!ifile_stream.eof()) {
						throw &e;
					} else {
						ifile_stream.clear();

						if (chars_count > 0) {
							line_lengths.insert(chars_count);
							chars_count = 0;
						}
					}
				}
			}

			lines_count = line_lengths.size();

			if (lines_count > 0) {
				it = line_lengths.begin();

				while (pos < (lines_count / 2 - 1)) {
					pos += 1;
					it++;
				}

				if (lines_count % 2 == 0) {
					median = (*it + *(++it)) / 2.0;
				} else {
					median = *(++it);
				}

				medians.push_back(median);

				median = 0.0;
				pos = 0;
				line_lengths.clear();
			}
		}

		try {
			ifile_stream.close();
		} catch (ifstream::failure &e) {
			throw AnalyzerException(2, file.get_full_path());
		}

		for (unsigned int i = 0; i < medians.size(); i++) {
			mean += medians.at(i);
		}
		mean = mean / medians.size();

		estimated_lines_count =  (int)ceil((file_length - header_length) / mean);
	}

	file.set_estimated_lines_count(estimated_lines_count);
}
