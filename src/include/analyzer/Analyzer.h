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
 * Date: 28-05-2010
 *
 * Description:
 * Class Analyzer is the main class, that determines the steps of processing.
 * It includes functions for reading input scripts, reading file header, processing file data and creating plots.
 *
 */

#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <algorithm>

#include "AnalyzerException.h"
#include "../file/File.h"
#include "../metas/Meta.h"
#include "../metas/MetaFormat.h"
#include "../metas/MetaGroup.h"
#include "../metas/MetaUniqueness.h"
#include "../metas/MetaNumeric.h"
#include "../metas/MetaImplausible.h"
#include "../metas/MetaImplausibleStrict.h"
#include "../metas/MetaImplausibleStrictAdjusted.h"
#include "../metas/MetaFiltered.h"
#include "../metas/MetaCrossTable.h"
#include "../metas/MetaRatio.h"
#include "../expressions/Conjunction.h"
#include "../expressions/Disjunction.h"
#include "../expressions/Get.h"
#include "../expressions/Gt.h"
#include "../expressions/Let.h"
#include "../expressions/Lt.h"
#include "../plots/Plot.h"

#include "R.h"

class Analyzer {
private:
	static const int LINE_BUFFER_SIZE;

	char* line_buffer;
	char* cur;
	char* beg;

	void detect_separators(File& file) throw (AnalyzerException);
	void estimate_lines_count(File& file) throw (AnalyzerException);
	void process_header(File& file, ifstream& ifile_stream) throw (ifstream::failure, AnalyzerException, MetaException);
	void initialize_filters_and_crosstables(File& file) throw (FileException, PlotException, MetaException);
	void initialize_dependencies(File& file);
	Boxplot* create_common_boxplot(vector<File*>& files, const char* plot_name) throw (AnalyzerException, FileException, PlotException);
	Qqplot* create_common_qqplot(File& file, int column_from, int column_to) throw (PlotException);
	void create_common_qqplots(File& file, vector<Plot*>& plots) throw (PlotException);
	char* qc_strtok(const char sep);
	static int compare(const void* first, const void* second);
	int sstrcmp(const char* first, const char* second);
	static bool compare_boxplots(Boxplot* first, Boxplot* second);

	void print_boxplots_html(vector<Boxplot*>& boxplots, const char* file_name, const char* resource_file) throw (AnalyzerException);
	void print_menu_html(vector<File*>& files, const char* boxplotsfile_name,const char* file_name, const char* resource_file) throw (AnalyzerException, FileException);
	void print_frameset_html(const char* file_name, const char* menu_name, File* initial_file) throw (AnalyzerException);

	void check_separators(File& file) throw (AnalyzerException, FileException);
	void check_verbosity_level(File& file) throw (AnalyzerException, FileException);
	void check_missing_value(File& file) throw (AnalyzerException, FileException);
	void check_filters(File& file) throw (AnalyzerException, FileException);
	void check_thresholds(File& file) throw (AnalyzerException, FileException);
	void check_case_sensitivity(File& file) throw (FileException);
	void check_output_prefix(File& file) throw (AnalyzerException, FileException);

	int get_line(ifstream& ifile_stream, char* line, int length) throw (ifstream::failure);

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
	static const char* PVALUE_HQ_1;
	static const char* PVALUE_HQ_2;
	static const char* PVALUE_FROM0TO1;
	static const char* PVALUE_MAF_1;
	static const char* PVALUE_MAF_2;
	static const char* PVALUE_IMP_1;
	static const char* PVALUE_IMP_2;
	static const char* EFFECT;
	static const char* EFFECT_HQ;
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
	static const char* SEMICOLON;
	static const char* TAB;
	static const char* MISSED;
	static const int CHECK_ROW_COUNT_FOR_SEPARATOR;
	static const int CHECK_ROW_COUNT_FOR_SIZE;
	static const int CHECK_SAMPLE_COUNT_FOR_SIZE;
	static const int SEPARATORS_SIZE;
	static const char COMMENT;
	static const char separators[];

	/* Commands */
	static const char* PROCESS;
	static const char* ANALYZE;
	static const char* VERBOSITY;
	static const char* MAF;
	static const char* IMP;
	static const char* BOXPLOTWIDTH;
	static const char* PREFIX;
	static const char* CASESENSITIVE;
	static const char* SNP_HQ;

	/* Verbosity */
	static const char* VERBOSITY_LOW;
	static const char* VERBOSITY_HIGH;

	/* Mandatory columns */
	static const int MANDATORY_COLUMNS_SIZE;
	static const char* mandatory_columns[];

	/* Columns with thresholds */
	static const int THRESHOLD_COLUMNS_SIZE;
	static const char* threshold_columns[];

	/* Mandatory columns and their order for CSV */
	static const int CSV_COLUMNS_SIZE;
	static const char* csv_columns[];

	/* Dependencies (for additional warnings) */
	static const int DEPENDENCIES_SIZE;
	static const char* dependencies[][2];

	/* Single column plots */
	static const int SINGLE_COLUMN_PLOTS_SIZE;
	static const char* single_column_plots[][2];

	/* Common plots */
	static const int COMMON_BOXPLOTS_SIZE;
	static const char* common_boxplots[][2];

	/* QQ plots */
	static const int COMMON_QQPLOTS_SIZE;
	static const char* common_qqplots[][2];

	/* External resources */
	static const char* CONTENT_STYLE;
	static const char* MENU_STYLE;
	static const char* JSCRIPT;

	/* Output file names */
	static const char* MENU_FILE;
	static const char* MAIN_FILE;
	static const char* BOXPLOTS_FILE;
	/*static const char* SUMMARY_FILE;*/

	/* File extentions */
	static const char* TXT_EXTENSION;
	static const char* CSV_EXTENSION;
	static const char* HTML_EXTENSION;
	static const char* HTM_EXTENSION;

	Analyzer() throw (AnalyzerException);
	~Analyzer();

	vector<File*>* process_script(const char* file_name, char file_separator) throw (AnalyzerException, FileException);
	int process_data(File& file) throw (AnalyzerException, FileException, PlotException, MetaException);
	vector<Boxplot*>* create_common_boxplots(vector<File*>& files) throw (AnalyzerException, FileException, PlotException);
	vector<Plot*>* create_plots(File& file) throw (FileException, PlotException);

	void print_result_txt(File& file) throw (AnalyzerException, FileException);
	void print_result_csv(File& file) throw (AnalyzerException, FileException);
	void print_result_html(File& file, vector<Plot*>* plots, const char* resource_path) throw (AnalyzerException, FileException);
	void print_result_html(vector<File*>& files, vector<Boxplot*>* boxplots, const char* resource_path) throw (AnalyzerException, FileException);
	/*void print_summary_txt(vector<File*>& files, const char* o_file_name) throw (AnalyzerException);*/
};

#endif
