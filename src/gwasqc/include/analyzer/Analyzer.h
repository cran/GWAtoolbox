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

#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <stdexcept>

#include "../../../GWAtoolbox.h"
#include "../../../auxiliary/include/auxiliary.h"
#include "../../../reader/include/TextReader.h"
#include "../../../gwafile/include/GwaFile.h"
#include "../metas/MetaCrossTable.h"
#include "../metas/MetaFiltered.h"
#include "../metas/MetaFormat.h"
#include "../metas/MetaGroup.h"
#include "../metas/MetaImplausible.h"
#include "../metas/MetaImplausibleStrictAdjusted.h"
#include "../metas/MetaRatio.h"
#include "../metas/MetaUniqueness.h"
#include "../expressions/Conjunction.h"
#include "../expressions/Disjunction.h"
#include "../expressions/Get.h"
#include "../expressions/Gt.h"
#include "../expressions/Let.h"
#include "../expressions/Lt.h"
#include "AnalyzerException.h"

#include <R.h>

class Analyzer {
private:
//	TextReader reader;
	Reader* reader;
	GwaFile* gwafile;

	vector<Meta*> metas;
	vector<MetaNumeric*> numeric_metas;
	vector<Plottable*> plots;
	vector<char*> missing_columns;
	vector<char*> not_required_columns;
	vector< pair<pair<Meta*, Meta*>, int> > dependencies;
	vector<MetaFiltered*> filtered_metas;
	vector<MetaRatio*> ratio_metas;
	vector<MetaCrossTable*> cross_table_metas;

	Qqplot* create_combined_qqplot(int column_from, int column_to) throw (PlotException);

	static void print_boxplots_html(vector<const char*>& boxplot_names, const char* html_output_name, const char* resource_path) throw (AnalyzerException);
	static void print_menu_html(vector<const char*>& gwa_file_names, vector<const char*>& html_report_paths, const char* boxplots_html, const char* html_output_name, const char* resource_path, char path_separator) throw (AnalyzerException);
	static void print_frameset_html(const char* menu_html, const char* html_output_name, const char* first_frame_html, char path_separator) throw (AnalyzerException);

public:
	static const int MANDATORY_COLUMNS_SIZE;
	static const char* mandatory_columns[];

	static const int COLUMN_DEPENDENCIES_SIZE;
	static const char* column_dependencies[][2];

	static const int CSV_COLUMNS_SIZE;
	static const char* csv_columns[];

	static const int SINGLE_COLUMN_PLOTS_SIZE;
	static const char* single_column_plots[][2];

	static const int COMBINED_BOXPLOTS_SIZE;
	static const char* combined_boxplots[][2];

	static const int COMBINED_QQPLOTS_SIZE;
	static const char* combined_qqplots[][2];

	static const char* PVALUE_HQ_1;
	static const char* PVALUE_HQ_2;
	static const char* PVALUE_FROM0TO1;
	static const char* PVALUE_MAF_1;
	static const char* PVALUE_MAF_2;
	static const char* PVALUE_IMP_1;
	static const char* PVALUE_IMP_2;
	static const char* EFFECT_HQ;

	static const char* TXT_EXTENSION;
	static const char* CSV_EXTENSION;
	static const char* HTML_EXTENSION;
	static const char* HTM_EXTENSION;

	static const char* CONTENT_STYLE;
	static const char* MENU_STYLE;
	static const char* JSCRIPT;

	static const char* MENU_FILE;
	static const char* MAIN_FILE;
	static const char* BOXPLOTS_FILE;

	Analyzer();
	virtual ~Analyzer();

	void open_gwafile(GwaFile* gwafile) throw (AnalyzerException);
	void close_gwafile() throw (AnalyzerException);

	void process_header() throw (AnalyzerException);
	void initialize_column_dependencies();
	void initialize_filtered_columns() throw (AnalyzerException);
	void initialize_columns_ratios() throw (AnalyzerException);
	void process_data() throw (AnalyzerException);
	void finalize_processing() throw (AnalyzerException);
	double get_memory_usage();

	void print_txt_report() throw (AnalyzerException);
	void print_csv_report() throw (AnalyzerException);
	void print_html_report(char** html_report_path, vector<Plot*>& plots, const char* resource_path) throw (AnalyzerException);

	void create_plots(vector<Plot*>& plots) throw (AnalyzerException);
	void create_combined_qqplots(vector<Plot*>& plots) throw (AnalyzerException);
	void create_combined_boxplots(vector<Plot*>& plots) throw (AnalyzerException);

	static void print_main_html(vector<const char*>& gwa_file_names, vector<const char*>& html_report_paths, vector<const char*>& boxplot_names, const char* resource_path, char path_separator) throw (AnalyzerException);
};

#endif
