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

#include "../../include/analyzer/Analyzer.h"

const int Analyzer::MANDATORY_COLUMNS_SIZE = 11;
const char* Analyzer::mandatory_columns[MANDATORY_COLUMNS_SIZE] = {
		Descriptor::MARKER, Descriptor::ALLELE1, Descriptor::ALLELE2, Descriptor::CHR,
		Descriptor::POSITION, Descriptor::STRAND, Descriptor::PVALUE, Descriptor::EFFECT,
		Descriptor::STDERR, Descriptor::FREQLABEL, Descriptor::OEVAR_IMP
};

const int Analyzer::COLUMN_DEPENDENCIES_SIZE = 15;
const char* Analyzer::column_dependencies[COLUMN_DEPENDENCIES_SIZE][2] = {
		{Descriptor::EFFECT, Descriptor::MARKER}, {Descriptor::EFFECT, Descriptor::CHR}, {Descriptor::EFFECT, Descriptor::POSITION},
		{Descriptor::STDERR, Descriptor::MARKER}, {Descriptor::STDERR, Descriptor::CHR}, {Descriptor::STDERR, Descriptor::POSITION},
		{Descriptor::PVALUE, Descriptor::MARKER}, {Descriptor::PVALUE, Descriptor::CHR}, {Descriptor::PVALUE, Descriptor::POSITION},
		{Descriptor::EFFECT, Descriptor::STDERR}, {Descriptor::EFFECT, Descriptor::PVALUE},
		{Descriptor::STDERR, Descriptor::EFFECT}, {Descriptor::STDERR, Descriptor::PVALUE},
		{Descriptor::PVALUE, Descriptor::EFFECT}, {Descriptor::PVALUE, Descriptor::STDERR}
};

const int Analyzer::CSV_COLUMNS_SIZE = 9;
const char* Analyzer::csv_columns[CSV_COLUMNS_SIZE] = {
		Descriptor::N_TOTAL, Descriptor::EFFECT, Descriptor::STDERR,
		Descriptor::PVALUE, Descriptor::MAF, Descriptor::IMPUTED,
		Descriptor::OEVAR_IMP, Descriptor::CALLRATE, Descriptor::HWE_PVAL
};

const int Analyzer::SINGLE_COLUMN_PLOTS_SIZE = 12;
const char* Analyzer::single_column_plots[SINGLE_COLUMN_PLOTS_SIZE][2] = {
		{Descriptor::STDERR, Descriptor::STDERR}, {Descriptor::OEVAR_IMP, Descriptor::OEVAR_IMP},
		{Descriptor::PVALUE, Descriptor::PVALUE}, {Descriptor::PVALUE, PVALUE_HQ_1},
		{Descriptor::FREQLABEL, Descriptor::FREQLABEL},	{Descriptor::HWE_PVAL, Descriptor::HWE_PVAL},
		{Descriptor::CALLRATE, Descriptor::CALLRATE}, {Descriptor::EFFECT, Descriptor::EFFECT},
		{Descriptor::EFFECT, EFFECT_HQ}, {Descriptor::N_TOTAL, Descriptor::N_TOTAL},
		{Descriptor::IMPUTED, Descriptor::IMPUTED}, {Descriptor::USED_FOR_IMP, Descriptor::USED_FOR_IMP}
};

const int Analyzer::COMBINED_BOXPLOTS_SIZE = 2;
const char* Analyzer::combined_boxplots[COMBINED_BOXPLOTS_SIZE][2] = {
		{Descriptor::EFFECT, "All Data"}, {EFFECT_HQ, "High Quality Data"}
};

const int Analyzer::COMBINED_QQPLOTS_SIZE = 9;
const char* Analyzer::combined_qqplots[COMBINED_QQPLOTS_SIZE][2] = {
		{"qqplot_MAF", PVALUE_MAF_2}, {"qqplot_MAF", PVALUE_MAF_1}, {"qqplot_MAF", PVALUE_FROM0TO1},
		{"qqplot_IMP", PVALUE_IMP_2}, {"qqplot_IMP", PVALUE_IMP_1}, {"qqplot_IMP", PVALUE_FROM0TO1},
		{"qqplot_MAF_IMP", PVALUE_HQ_2}, {"qqplot_MAF_IMP", PVALUE_HQ_1}, {"qqplot_MAF_IMP", PVALUE_FROM0TO1}
};

const char* Analyzer::PVALUE_HQ_1 = "PVALUE_HQ_1";
const char* Analyzer::PVALUE_HQ_2 = "PVALUE_HQ_2";
const char* Analyzer::PVALUE_FROM0TO1 = "PVALUE_FROM0TO1";
const char* Analyzer::PVALUE_MAF_1 = "PVALUE_MAF_1";
const char* Analyzer::PVALUE_MAF_2 = "PVALUE_MAF_2";
const char* Analyzer::PVALUE_IMP_1 = "PVALUE_IMP_1";
const char* Analyzer::PVALUE_IMP_2 = "PVALUE_IMP_2";
const char* Analyzer::EFFECT_HQ = "EFFECT_HQ";

const char* Analyzer::TXT_EXTENSION = ".txt";
const char* Analyzer::CSV_EXTENSION = ".csv";
const char* Analyzer::HTML_EXTENSION = ".html";
const char* Analyzer::HTM_EXTENSION = ".htm";

const char* Analyzer::CONTENT_STYLE = "content_style.css";
const char* Analyzer::MENU_STYLE = "menu_style.css";
const char* Analyzer::JSCRIPT = "script.js";

const char* Analyzer::MENU_FILE = "menu.html";
const char* Analyzer::MAIN_FILE = "main.html";
const char* Analyzer::BOXPLOTS_FILE = "boxplots.html";

Analyzer::Analyzer() : reader(NULL), gwafile(NULL) {

}

Analyzer::~Analyzer() {
	vector<char*>::iterator vector_char_it;
	vector<Meta*>::iterator metas_it;
	vector<MetaFiltered*>::iterator filtered_metas_it;
	vector<MetaRatio*>::iterator ratio_metas_it;
	vector<MetaCrossTable*>::iterator cross_table_metas_it;

	for (vector_char_it = missing_columns.begin(); vector_char_it != missing_columns.end(); vector_char_it++) {
		free(*vector_char_it);
	}

	for (vector_char_it = not_required_columns.begin(); vector_char_it != not_required_columns.end(); vector_char_it++) {
		free(*vector_char_it);
	}

	for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
		if ((*metas_it) != NULL) {
			delete *metas_it;
		}
	}

	for (filtered_metas_it = filtered_metas.begin(); filtered_metas_it != filtered_metas.end(); filtered_metas_it++) {
		delete *filtered_metas_it;
	}

	for (ratio_metas_it = ratio_metas.begin(); ratio_metas_it != ratio_metas.end(); ratio_metas_it++) {
		delete *ratio_metas_it;
	}

	for (cross_table_metas_it = cross_table_metas.begin(); cross_table_metas_it != cross_table_metas.end(); cross_table_metas_it++) {
		delete *cross_table_metas_it;
	}

	missing_columns.clear();
	not_required_columns.clear();
	metas.clear();
	plots.clear();
	dependencies.clear();
	filtered_metas.clear();
	ratio_metas.clear();
	cross_table_metas.clear();
	numeric_metas.clear();

	gwafile = NULL;
}

void Analyzer::open_gwafile(GwaFile* gwafile) throw (AnalyzerException) {
	if (gwafile == NULL) {
		throw AnalyzerException("Analyzer", "open_gwafile( GwaFile* )", __LINE__, 0, "gwafile");
	}

	try {
		close_gwafile();

		reader = ReaderFactory::create(gwafile->get_descriptor()->get_full_path());
		reader->set_file_name(gwafile->get_descriptor()->get_full_path());
		reader->open();

		this->gwafile = gwafile;
	} catch (ReaderException& e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "open_gwafile( GwaFile* )", __LINE__, 3, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (AnalyzerException& e) {
		e.add_message("Analyzer", "open_gwafile( GwaFile* )", __LINE__, 3, gwafile->get_descriptor()->get_full_path());
		throw;
	}
}

void Analyzer::close_gwafile() throw (AnalyzerException) {
	vector<char*>::iterator vector_char_it;
	vector<Meta*>::iterator metas_it;
	vector<MetaFiltered*>::iterator filtered_metas_it;
	vector<MetaRatio*>::iterator ratio_metas_it;
	vector<MetaCrossTable*>::iterator cross_table_metas_it;

	try {
		if (reader != NULL) {
			reader->close();
			delete reader;
			reader = NULL;
		}
	} catch (ReaderException &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "close_gwafile()", __LINE__, 4, gwafile != NULL ? gwafile->get_descriptor()->get_full_path() : "NULL");
		throw new_e;
	}

	for (vector_char_it = missing_columns.begin(); vector_char_it != missing_columns.end(); vector_char_it++) {
		free(*vector_char_it);
	}

	for (vector_char_it = not_required_columns.begin(); vector_char_it != not_required_columns.end(); vector_char_it++) {
		free(*vector_char_it);
	}

	for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
		if ((*metas_it) != NULL) {
			delete *metas_it;
		}
	}

	for (filtered_metas_it = filtered_metas.begin(); filtered_metas_it != filtered_metas.end(); filtered_metas_it++) {
		delete *filtered_metas_it;
	}

	for (ratio_metas_it = ratio_metas.begin(); ratio_metas_it != ratio_metas.end(); ratio_metas_it++) {
		delete *ratio_metas_it;
	}

	for (cross_table_metas_it = cross_table_metas.begin(); cross_table_metas_it != cross_table_metas.end(); cross_table_metas_it++) {
		delete *cross_table_metas_it;
	}

	missing_columns.clear();
	not_required_columns.clear();
	metas.clear();
	plots.clear();
	dependencies.clear();
	filtered_metas.clear();
	ratio_metas.clear();
	cross_table_metas.clear();
	numeric_metas.clear();

	gwafile = NULL;
}

void Analyzer::process_header() throw (AnalyzerException) {
	Descriptor* descriptor = NULL;
	char header_separator = '\0';
	char* header = NULL;
	char* token = NULL;
	const char* column_name = NULL;
	char* column_name_copy = NULL;
	int column_position = 0;
	Meta* meta = NULL;
	vector<char*>::iterator vector_char_it;
	vector<double>* thresholds;
	unsigned int heap_size = 0;

	if (gwafile == NULL) {
		return;
	}

	try {
		if (reader->read_line() <= 0) {
			throw AnalyzerException("Analyzer", "process_header()", __LINE__, 5, gwafile->get_descriptor()->get_full_path());
		}

		descriptor = gwafile->get_descriptor();
		header_separator = gwafile->get_header_separator();
		header = *(reader->line);

		if ((gwafile->get_estimated_size() > numeric_limits<unsigned int>::max()) ||
				((heap_size = (unsigned int)gwafile->get_estimated_size()) == 0)) {
			heap_size = Meta::HEAP_SIZE;
		}

		for (int i = 0; i < MANDATORY_COLUMNS_SIZE; i++) {
			column_name = descriptor->get_column(mandatory_columns[i]);
			if (column_name != NULL) {
				column_name_copy = (char*)malloc((strlen(column_name) + 1) * sizeof(char));
				if (column_name_copy == NULL) {
					throw AnalyzerException("Analyzer", "process_header()", __LINE__, 2, (strlen(column_name) + 1) * sizeof(char));
				}
				strcpy(column_name_copy, column_name);
				missing_columns.push_back(column_name_copy);
			}
		}

		token = auxiliary::strtok(&header, header_separator);
		while (token != NULL) {
			column_name = descriptor->get_default_column(token, gwafile->is_case_sensitive());
			if (column_name != NULL) {
				if (strcmp(column_name, Descriptor::MARKER) == 0) {
					meta = new MetaUniqueness(heap_size);
				} else if (strcmp(column_name, Descriptor::ALLELE1) == 0) {
					meta = new MetaGroup(true);
				} else if (strcmp(column_name, Descriptor::ALLELE2) == 0) {
					meta = new MetaGroup(true);
				} else if (strcmp(column_name, Descriptor::CHR) == 0) {
					meta = new MetaGroup();
				} else if (strcmp(column_name, Descriptor::POSITION) == 0) {
					meta = new MetaFormat();
					meta->set_full_name("Position");
				} else if (strcmp(column_name, Descriptor::STRAND) == 0) {
					meta = new MetaGroup();
				} else if (strcmp(column_name, Descriptor::PVALUE) == 0) {
					thresholds = descriptor->get_threshold(Descriptor::PVALUE);
					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), heap_size);
					numeric_metas.push_back((MetaNumeric*)meta);
					((MetaImplausibleStrict*)meta)->set_plots(true, false, false);
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("P-value");
					plots.push_back((MetaImplausibleStrict*)meta);
				} else if (strcmp(column_name, Descriptor::EFFECT) == 0) {
					meta = new MetaNumeric(heap_size);
					numeric_metas.push_back((MetaNumeric*)meta);
					((MetaNumeric*)meta)->set_description("All data");
					((MetaNumeric*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Effect size");
					plots.push_back((MetaNumeric*)meta);
				} else if (strcmp(column_name, Descriptor::STDERR) == 0) {
					thresholds = descriptor->get_threshold(Descriptor::STDERR);
					meta = new MetaImplausible(thresholds->at(0), thresholds->at(1), heap_size);
					numeric_metas.push_back((MetaNumeric*)meta);
					((MetaImplausible*)meta)->set_description("All data");
					((MetaImplausible*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Standard error");
					plots.push_back((MetaImplausible*)meta);
				} else if (strcmp(column_name, Descriptor::FREQLABEL) == 0) {
					thresholds = descriptor->get_threshold(Descriptor::FREQLABEL);
					meta = new MetaImplausibleStrictAdjusted(thresholds->at(0), thresholds->at(1), 0.5, heap_size);
					numeric_metas.push_back((MetaNumeric*)meta);
					((MetaImplausibleStrictAdjusted*)meta)->set_description(token);
					((MetaImplausibleStrictAdjusted*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Minor allele frequency");
					plots.push_back((MetaImplausibleStrictAdjusted*)meta);
				} else if (strcmp(column_name, Descriptor::HWE_PVAL) == 0) {
					thresholds = descriptor->get_threshold(Descriptor::HWE_PVAL);
					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), heap_size);
					if (!gwafile->is_high_verbosity()) {
						((MetaImplausibleStrict*)meta)->set_plots(false, false, false);
					}
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Hardy-Weinberg equilibrium p-value");
					numeric_metas.push_back((MetaNumeric*)meta);
					plots.push_back((MetaImplausibleStrict*)meta);
				} else if (strcmp(column_name, Descriptor::CALLRATE) == 0) {
					thresholds = descriptor->get_threshold(Descriptor::CALLRATE);
					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), heap_size);
					if (!gwafile->is_high_verbosity()) {
						((MetaImplausibleStrict*)meta)->set_plots(false, false, false);
					}
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Genotyping callrate");
					numeric_metas.push_back((MetaNumeric*)meta);
					plots.push_back((MetaImplausibleStrict*)meta);
				} else if (strcmp(column_name, Descriptor::N_TOTAL) == 0) {
					meta = new MetaNumeric(heap_size);
					if (!gwafile->is_high_verbosity()) {
						((MetaNumeric*)meta)->set_plots(true, false, false);
					}
					((MetaNumeric*)meta)->set_description("All data");
					((MetaNumeric*)meta)->set_ouptut(true, true, true, false, false, false);
					meta->set_full_name("Sample size");
					numeric_metas.push_back((MetaNumeric*)meta);
					plots.push_back((MetaNumeric*)meta);
				} else if (strcmp(column_name, Descriptor::IMPUTED) == 0) {
					meta = new MetaNumeric(heap_size);
					if (!gwafile->is_high_verbosity()) {
						((MetaNumeric*)meta)->set_plots(false, false, false);
					}
					((MetaNumeric*)meta)->set_description("All data");
					numeric_metas.push_back((MetaNumeric*)meta);
					plots.push_back((MetaNumeric*)meta);
				} else if (strcmp(column_name, Descriptor::USED_FOR_IMP) == 0) {
					meta = new MetaNumeric(heap_size);
					if (!gwafile->is_high_verbosity()) {
						((MetaNumeric*)meta)->set_plots(false, false, false);
					}
					((MetaNumeric*)meta)->set_description("All data");
					numeric_metas.push_back((MetaNumeric*)meta);
					plots.push_back((MetaNumeric*)meta);
				} else if (strcmp(column_name, Descriptor::OEVAR_IMP) == 0) {
					thresholds = descriptor->get_threshold(Descriptor::OEVAR_IMP);
					meta = new MetaImplausibleStrict(thresholds->at(0), thresholds->at(1), heap_size);
					numeric_metas.push_back((MetaNumeric*)meta);
					((MetaImplausibleStrict*)meta)->set_description("All data");
					((MetaImplausibleStrict*)meta)->set_ouptut(false, false, false, true, true, true);
					meta->set_full_name("Imputation quality");
					plots.push_back((MetaImplausible*)meta);
				} else {
					column_name_copy = (char*)malloc((strlen(token) + 1) * sizeof(char));
					if (column_name_copy == NULL) {
						throw AnalyzerException("Analyzer", "process_header()", __LINE__, 2, (strlen(token) + 1) * sizeof(char));
					}
					strcpy(column_name_copy, token);
					not_required_columns.push_back(column_name_copy);
					metas.push_back(NULL);

					token = auxiliary::strtok(&header, header_separator);
					column_position += 1;

					continue;
				}

				if (gwafile->is_case_sensitive()) {
					for (vector_char_it = missing_columns.begin(); vector_char_it != missing_columns.end(); vector_char_it++) {
						if (strcmp(*vector_char_it, token) == 0) {
							free(*vector_char_it);
							missing_columns.erase(vector_char_it);
							break;
						}
					}
				} else {
					for (vector_char_it = missing_columns.begin(); vector_char_it != missing_columns.end(); vector_char_it++) {
						if (auxiliary::strcmp_ignore_case(*vector_char_it, token) == 0) {
							free(*vector_char_it);
							missing_columns.erase(vector_char_it);
							break;
						}
					}
				}

				meta->set_common_name(column_name);
				meta->set_actual_name(token);
				meta->set_id(column_position);
				meta->set_na_marker(descriptor->get_property(Descriptor::MISSING));

				metas.push_back(meta);
			} else {
				column_name_copy = (char*)malloc((strlen(token) + 1) * sizeof(char));
				if (column_name_copy == NULL) {
					throw AnalyzerException("Analyzer", "process_header()", __LINE__, 2, (strlen(token) + 1) * sizeof(char));
				}
				strcpy(column_name_copy, token);
				not_required_columns.push_back(column_name_copy);
				metas.push_back(NULL);
			}

			token = auxiliary::strtok(&header, header_separator);
			column_position += 1;
		}
	} catch (DescriptorException &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "process_header()", __LINE__, 7, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (MetaException &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "process_header()", __LINE__, 7, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (AnalyzerException &e) {
		e.add_message("Analyzer", "process_header()", __LINE__, 7, gwafile->get_descriptor()->get_full_path());
		throw;
	}
}


void Analyzer::process_data() throw (AnalyzerException) {
	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 1;
	char* token = NULL;
	char data_separator = '\0';
	unsigned int column_number = 0;
	unsigned int total_columns = 0;

	Meta* meta = NULL;

	vector<MetaFiltered*>::iterator filtered_metas_it;
	vector<MetaRatio*>::iterator ratio_metas_it;
	vector<MetaCrossTable*>::iterator cross_table_metas_it;
	vector< pair<pair<Meta*, Meta*>, int> >::iterator dependencies_it;

	if (gwafile == NULL) {
		return;
	}

	data_separator = gwafile->get_data_separator();
	total_columns = metas.size();

	try {
		while ((line_length = reader->read_line()) > 0) {
			column_number = 0;
			line = *(reader->line);
			while ((token = auxiliary::strtok(&line, data_separator)) != NULL) {
				if ((meta = metas.at(column_number)) != NULL) {
					meta->put(token);
				}
				column_number += 1;
			}

			if (column_number != total_columns) {
				throw AnalyzerException("Analyzer", "process_data()", __LINE__, 12, line_number, gwafile->get_descriptor()->get_full_path());
			}

			for (filtered_metas_it = filtered_metas.begin(); filtered_metas_it != filtered_metas.end(); filtered_metas_it++) {
				(*filtered_metas_it)->put(NULL);
			}

			for (ratio_metas_it = ratio_metas.begin(); ratio_metas_it != ratio_metas.end(); ratio_metas_it++) {
				(*ratio_metas_it)->put(NULL);
			}

			for (cross_table_metas_it = cross_table_metas.begin(); cross_table_metas_it != cross_table_metas.end(); cross_table_metas_it++) {
				(*cross_table_metas_it)->put(NULL);
			}

			for (dependencies_it = dependencies.begin(); dependencies_it != dependencies.end(); dependencies_it++) {
				if (!dependencies_it->first.first->is_na() && dependencies_it->first.second->is_na()) {
					dependencies_it->second += 1;
				}
			}

			line_number += 1;
		}

		if (line_length == 0) {
			throw AnalyzerException("Analyzer", "process_data()", __LINE__, 13, line_number, gwafile->get_descriptor()->get_full_path());
		}
	} catch (ReaderException &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "process_data()", __LINE__, 11, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (MetaException &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "process_data()", __LINE__, 11, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (AnalyzerException &e) {
		e.add_message("Analyzer", "process_data()", __LINE__, 11, gwafile->get_descriptor()->get_full_path());
		throw;
	} catch (std::out_of_range &e) {
		AnalyzerException new_e("Analyzer", "process_data()", __LINE__, 12, line_number, gwafile->get_descriptor()->get_full_path());
		new_e.add_message("Analyzer", "process_data()", __LINE__, 11, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	}
}

void Analyzer::finalize_processing() throw (AnalyzerException) {
	vector<Meta*>::iterator metas_it;
	vector<MetaFiltered*>::iterator filtered_metas_it;
	vector<MetaRatio*>::iterator ratio_metas_it;
	vector<MetaCrossTable*>::iterator cross_table_metas_it;

	if (gwafile == NULL) {
		return;
	}

	try {
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if ((*metas_it) != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::MARKER) == 0) {
					(*metas_it)->finalize();
				}
			}
		}

		for (ratio_metas_it = ratio_metas.begin(); ratio_metas_it != ratio_metas.end(); ratio_metas_it++) {
			(*ratio_metas_it)->finalize();
		}

		for (cross_table_metas_it = cross_table_metas.begin(); cross_table_metas_it != cross_table_metas.end(); cross_table_metas_it++) {
			(*cross_table_metas_it)->finalize();
		}

		for (filtered_metas_it = filtered_metas.begin(); filtered_metas_it != filtered_metas.end(); filtered_metas_it++) {
			(*filtered_metas_it)->finalize();
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if ((*metas_it) != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::MARKER) != 0) {
					(*metas_it)->finalize();
				}
			}
		}
	} catch (Exception &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "finalize_processing()", __LINE__, 10, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	}
}

double Analyzer::get_memory_usage() {
	vector<Meta*>::iterator metas_it;
	vector<MetaFiltered*>::iterator filtered_metas_it;
	vector<MetaRatio*>::iterator ratio_metas_it;
	vector<MetaCrossTable*>::iterator cross_table_meta_it;

	double memory_usage = 0.0;

	for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
		if ((*metas_it) != NULL) {
			memory_usage += (*metas_it)->get_memory_usage();
		}
	}

	for (filtered_metas_it = filtered_metas.begin(); filtered_metas_it != filtered_metas.end(); filtered_metas_it++) {
		memory_usage += (*filtered_metas_it)->get_memory_usage();
	}

	for (ratio_metas_it = ratio_metas.begin(); ratio_metas_it != ratio_metas.end(); ratio_metas_it++) {
		memory_usage += (*ratio_metas_it)->get_memory_usage();
	}

	for (cross_table_meta_it = cross_table_metas.begin(); cross_table_meta_it != cross_table_metas.end(); cross_table_meta_it++) {
		memory_usage += (*cross_table_meta_it)->get_memory_usage();
	}

	return memory_usage;
}

void Analyzer::initialize_column_dependencies() {
	vector<Meta*>::iterator metas_it;
	Meta* left_dependant = NULL;
	Meta* right_dependant = NULL;

	if (gwafile == NULL) {
		return;
	}

	for (int i = 0; i < COLUMN_DEPENDENCIES_SIZE; i++) {
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if ((*metas_it) != NULL) {
				if ((left_dependant == NULL) && (strcmp((*metas_it)->get_common_name(), column_dependencies[i][0]) == 0)) {
					left_dependant = *metas_it;
				}

				if ((right_dependant == NULL) && (strcmp((*metas_it)->get_common_name(), column_dependencies[i][1]) == 0)) {
					right_dependant = *metas_it;
				}

				if ((left_dependant != NULL) && (right_dependant != NULL)) {
					dependencies.push_back(pair<pair<Meta*, Meta*>, int>(pair<Meta*, Meta*>(left_dependant, right_dependant), 0));
					break;
				}
			}
		}

		left_dependant = NULL;
		right_dependant = NULL;
	}
}
void Analyzer::initialize_filtered_columns() throw (AnalyzerException) {
	vector<MetaNumeric*>::iterator numeric_metas_it;
	MetaFiltered* filtered_meta = NULL;
	vector<double>* maf_levels = NULL;
	vector<double>* imp_levels = NULL;
	vector<double>* snp_hq = NULL;
	MetaNumeric* effect = NULL;
	MetaNumeric* freqlabel = NULL;
	MetaNumeric* oevar_imp = NULL;
	MetaNumeric* pvalue = NULL;
	MetaNumeric* se = NULL;
	MetaNumeric* imputed = NULL;
	MetaNumeric* used_for_imp = NULL;
	MetaFiltered* effect_hq = NULL;
	MetaFiltered* se_hq = NULL;

	unsigned int heap_size = 0;

	stringstream string_stream;

	if (gwafile == NULL) {
		return;
	}

	try {
		maf_levels = gwafile->get_descriptor()->get_threshold(Descriptor::MAF);
		imp_levels = gwafile->get_descriptor()->get_threshold(Descriptor::IMP);
		snp_hq = gwafile->get_descriptor()->get_threshold(Descriptor::SNP_HQ);

		if (gwafile->get_estimated_size() > numeric_limits<unsigned int>::max()) {
			heap_size = Meta::HEAP_SIZE / 3;
		} else {
			heap_size = ((unsigned int)gwafile->get_estimated_size()) / 3;
			if (heap_size == 0) {
				heap_size = 1;
			}
		}

		numeric_metas_it = numeric_metas.begin();
		while (numeric_metas_it != numeric_metas.end()) {
			if (*numeric_metas_it != NULL) {
				if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::EFFECT) == 0) {
					effect = *numeric_metas_it;
				} else if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::FREQLABEL) == 0) {
					freqlabel = *numeric_metas_it;
				} else if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::OEVAR_IMP) == 0) {
					oevar_imp = *numeric_metas_it;
				} else if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::PVALUE) == 0) {
					pvalue = *numeric_metas_it;
				} else if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::STDERR) == 0) {
					se = *numeric_metas_it;
				} else if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::IMPUTED) == 0) {
					imputed = *numeric_metas_it;
				} else if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::USED_FOR_IMP) == 0) {
					used_for_imp = *numeric_metas_it;
				}
			}
			numeric_metas_it++;
		}

		if ((freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
			numeric_metas_it = numeric_metas.begin();
			while (numeric_metas_it != numeric_metas.end()) {
				if (*numeric_metas_it != NULL) {
					filtered_meta = new MetaFiltered(*numeric_metas_it, heap_size);

					filtered_meta->set_common_name((*numeric_metas_it)->get_common_name());
					if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::FREQLABEL) == 0) {
						filtered_meta->set_actual_name("MAF");
					}
					else {
						filtered_meta->set_actual_name((*numeric_metas_it)->get_common_name());
					}
					filtered_meta->set_plots(false, false, false);

					filtered_meta->add_dependency(*freqlabel, *(new Gt(snp_hq->at(0))));
					filtered_meta->add_dependency(*oevar_imp, *(new Gt(snp_hq->at(1))));
					filtered_meta->add_dependency(*se, *(new Gt(0)));

					filtered_metas.push_back(filtered_meta);

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

		if ((effect != NULL) && (freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
			filtered_meta = new MetaFiltered(effect, heap_size);

			filtered_meta->set_common_name(EFFECT_HQ);
			filtered_meta->set_actual_name(EFFECT_HQ);
			filtered_meta->set_plots(false, true, false);

			string_stream.str("");
			string_stream << "MAF > " << maf_levels->at(0) << " and IMP > " << imp_levels->at(0);
			filtered_meta->set_description(string_stream.str().c_str());

			filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(0))));
			filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(0))));
			filtered_meta->add_dependency(*se, *(new Gt(0)));

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}

		if (pvalue != NULL) {
			filtered_meta = new MetaFiltered(pvalue, heap_size);

			filtered_meta->set_common_name(PVALUE_FROM0TO1);
			filtered_meta->set_actual_name(PVALUE_FROM0TO1);
			filtered_meta->set_plots(false, false, true);
			filtered_meta->set_description("Unfiltered");
			filtered_meta->set_color("black");

			filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}

		if ((pvalue != NULL) && (freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
			filtered_meta = new MetaFiltered(pvalue, heap_size);

			filtered_meta->set_common_name(PVALUE_HQ_1);
			filtered_meta->set_actual_name(PVALUE_HQ_1);
			filtered_meta->set_plots(true, false, true);
			filtered_meta->set_color("red");

			string_stream.str("");
			string_stream << "MAF > " << maf_levels->at(0) << " and IMP > " << imp_levels->at(0);
			filtered_meta->set_description(string_stream.str().c_str());

			filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(0))));
			filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(0))));
			filtered_meta->add_dependency(*se, *(new Gt(0)));

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}

		if ((pvalue != NULL) && (freqlabel != NULL) && (oevar_imp != NULL) && (se != NULL)) {
			filtered_meta = new MetaFiltered(pvalue, heap_size);

			filtered_meta->set_common_name(PVALUE_HQ_2);
			filtered_meta->set_actual_name(PVALUE_HQ_2);
			filtered_meta->set_plots(false, false, true);
			filtered_meta->set_color("blue");

			string_stream.str("");
			string_stream << "MAF > " << maf_levels->at(1) << " and IMP > " << imp_levels->at(1);
			filtered_meta->set_description(string_stream.str().c_str());

			filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(1))));
			filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(1))));
			filtered_meta->add_dependency(*se, *(new Gt(0)));

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}

		if ((pvalue != NULL) && (freqlabel != NULL) && (se != NULL) && (effect != NULL)) {
			filtered_meta = new MetaFiltered(pvalue, heap_size);

			filtered_meta->set_common_name(PVALUE_MAF_1);
			filtered_meta->set_actual_name(PVALUE_MAF_1);
			filtered_meta->set_plots(false, false, true);
			filtered_meta->set_color("red");

			string_stream.str("");
			string_stream << "MAF > " << maf_levels->at(0);
			filtered_meta->set_description(string_stream.str().c_str());

			filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
			filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(0))));
			filtered_meta->add_dependency(*se, *(new Gt(0)));
			filtered_meta->add_dependency(*effect);

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}

		if ((pvalue != NULL) && (freqlabel != NULL) && (se != NULL) && (effect != NULL)) {
			filtered_meta = new MetaFiltered(pvalue, heap_size);

			filtered_meta->set_common_name(PVALUE_MAF_2);
			filtered_meta->set_actual_name(PVALUE_MAF_2);
			filtered_meta->set_plots(false, false, true);
			filtered_meta->set_color("blue");

			string_stream.str("");
			string_stream << "MAF > " << maf_levels->at(1);
			filtered_meta->set_description(string_stream.str().c_str());

			filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
			filtered_meta->add_dependency(*freqlabel, *(new Gt(maf_levels->at(1))));
			filtered_meta->add_dependency(*se, *(new Gt(0)));
			filtered_meta->add_dependency(*effect);

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}

		if ((pvalue != NULL) && (oevar_imp != NULL) && (se != NULL) && (effect != NULL)) {
			filtered_meta = new MetaFiltered(pvalue, heap_size);

			filtered_meta->set_common_name(PVALUE_IMP_1);
			filtered_meta->set_actual_name(PVALUE_IMP_1);
			filtered_meta->set_plots(false, false, true);
			filtered_meta->set_color("red");

			string_stream.str("");
			string_stream << "IMP > " << imp_levels->at(0);
			filtered_meta->set_description(string_stream.str().c_str());

			filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
			filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(0))));
			filtered_meta->add_dependency(*se, *(new Gt(0)));
			filtered_meta->add_dependency(*effect);

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}

		if ((pvalue != NULL) && (oevar_imp != NULL) && (se != NULL) && (effect != NULL)) {
			filtered_meta = new MetaFiltered(pvalue, heap_size);

			filtered_meta->set_common_name(PVALUE_IMP_2);
			filtered_meta->set_actual_name(PVALUE_IMP_2);
			filtered_meta->set_plots(false, false, true);
			filtered_meta->set_color("blue");

			string_stream.str("");
			string_stream << "IMP > " << imp_levels->at(1);
			filtered_meta->set_description(string_stream.str().c_str());

			filtered_meta->add_dependency(*pvalue, *(new Conjunction(*(new Gt(0.0)), *(new Let(1.0)))));
			filtered_meta->add_dependency(*oevar_imp, *(new Gt(imp_levels->at(1))));
			filtered_meta->add_dependency(*se, *(new Gt(0)));
			filtered_meta->add_dependency(*effect);

			filtered_metas.push_back(filtered_meta);
			plots.push_back(filtered_meta);
		}
	} catch (Exception &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "initialize_filtered_columns()", __LINE__, 8, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	}
}

void Analyzer::initialize_columns_ratios() throw (AnalyzerException) {
	vector<MetaFiltered*>::iterator filtered_metas_it;
	MetaFiltered* effect_hq = NULL;
	MetaFiltered* se_hq = NULL;
	vector<MetaNumeric*>::iterator numeric_metas_it;
	MetaNumeric* imputed = NULL;
	MetaNumeric* used_for_imp = NULL;
	MetaRatio* ratio_meta = NULL;
	MetaCrossTable* cross_table_meta = NULL;

	unsigned int heap_size = 0;

	if (gwafile == NULL) {
		return;
	}

	if ((gwafile->get_estimated_size() > numeric_limits<unsigned int>::max()) ||
			((heap_size = (unsigned int)gwafile->get_estimated_size()) == 0)) {
		heap_size = Meta::HEAP_SIZE;
	}

	filtered_metas_it = filtered_metas.begin();
	while (filtered_metas_it != filtered_metas.end()) {
		if (strcmp((*filtered_metas_it)->get_common_name(), Descriptor::EFFECT) == 0) {
			effect_hq = *filtered_metas_it;
		} else if (strcmp((*filtered_metas_it)->get_common_name(), Descriptor::STDERR) == 0) {
			se_hq = *filtered_metas_it;
		}
		filtered_metas_it++;
	}

	numeric_metas_it = numeric_metas.begin();
	while (numeric_metas_it != numeric_metas.end()) {
		if (*numeric_metas_it != NULL) {
			if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::IMPUTED) == 0) {
				imputed = *numeric_metas_it;
			} else if (strcmp((*numeric_metas_it)->get_common_name(), Descriptor::USED_FOR_IMP) == 0) {
				used_for_imp = *numeric_metas_it;
			}
		}
		numeric_metas_it++;
	}

	try {
		if ((effect_hq != NULL) && (se_hq != NULL)) {
			ratio_meta = new MetaRatio(effect_hq, se_hq, heap_size);
			ratio_metas.push_back(ratio_meta);
		}

		if ((imputed != NULL) && (used_for_imp != NULL)) {
			cross_table_meta = new MetaCrossTable(imputed, used_for_imp);
			cross_table_meta->set_full_name("SNPs used for imputation vs. imputed SNPs");
			cross_table_metas.push_back(cross_table_meta);
		}
	} catch (Exception &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "initialize_columns_ratios()", __LINE__, 9, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	}
}

void Analyzer::print_txt_report() throw (AnalyzerException) {
	const char* output_prefix = NULL;
	const char* file_name = NULL;
	const char* file_extension = NULL;
	const char* file_abbreviation = NULL;
	char* o_textfile_name = NULL;

	ofstream o_textfile_stream;

	if (gwafile == NULL) {
		return;
	}

	output_prefix = gwafile->get_descriptor()->get_property(Descriptor::PREFIX);
	file_extension = gwafile->get_descriptor()->get_extension();
	file_abbreviation = gwafile->get_descriptor()->get_abbreviation();
	if (file_abbreviation == NULL) {
		file_name = gwafile->get_descriptor()->get_name();
		if ((file_extension != NULL) &&
				((auxiliary::strcmp_ignore_case(file_extension, TXT_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, CSV_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, HTML_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, HTM_EXTENSION) == 0))) {
			auxiliary::transform_file_name(&o_textfile_name, output_prefix, file_name, TXT_EXTENSION, false);
		} else {
			auxiliary::transform_file_name(&o_textfile_name, output_prefix, file_name, TXT_EXTENSION, true);
		}
	} else {
		auxiliary::transform_file_name(&o_textfile_name, output_prefix, file_abbreviation, TXT_EXTENSION, true);
	}

	if (o_textfile_name == NULL) {
		throw AnalyzerException("Analyzer", "print_txt_report()", __LINE__, 14);
	}

	o_textfile_stream.exceptions(ofstream::failbit | ofstream::badbit);

	try {
		o_textfile_stream.open(o_textfile_name);
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_txt_report()", __LINE__, 15, o_textfile_name);
	}

	try {
		time_t system_time = time(NULL);

		const char* version = "Version     : ";
		const char* scanned = "File scanned: ";
		const char* date = "Date        : ";
		char* current_time = strtok(asctime(localtime(&system_time)), "\n");

		unsigned int section = 0, subsection = 0;
		bool print = true;

		vector<Meta*>::iterator metas_it;
		vector<MetaNumeric*>::iterator numeric_metas_it;
		vector<MetaCrossTable*>::iterator cross_table_metas_it;
		vector< pair<pair<Meta*, Meta*>, int> >::iterator dependency_it;
		vector<char*>::iterator vector_char_it;

		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << setfill(' ') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << version << VERSION << setfill(' ') << setw(76 - strlen(version) - strlen(VERSION)) << " #" << endl;
		o_textfile_stream << "# " << setfill(' ') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << scanned << gwafile->get_descriptor()->get_name() << setfill(' ') << setw(76 - strlen(scanned) - strlen(gwafile->get_descriptor()->get_name())) << " #" << endl;
		o_textfile_stream << "# " << date << current_time << setfill(' ') << setw(76 - strlen(date) - strlen(current_time)) << " #" << endl;
		o_textfile_stream << "# " << setfill(' ') << setw(76) << " #" << endl;
		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;

		o_textfile_stream << endl;

		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		if (missing_columns.empty()) {
			o_textfile_stream << " All the required columns are present" << endl;
		}
		else {
			o_textfile_stream << " The following required columns are missing or have different cases:" << endl;
			o_textfile_stream << endl;

			vector_char_it = missing_columns.begin();
			while (vector_char_it != missing_columns.end()) {
				o_textfile_stream << " " << *vector_char_it << endl;
				vector_char_it++;
			}
		}

		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		if (not_required_columns.empty()) {
			o_textfile_stream << " No additional columns were present" << endl;
		} else {
			o_textfile_stream << " The following columns do not match required column names or were not required:" << endl;
			o_textfile_stream << endl;

			vector_char_it = not_required_columns.begin();
			while (vector_char_it != not_required_columns.end()) {
				o_textfile_stream << " " << *vector_char_it << endl;
				vector_char_it++;
			}
		}
		o_textfile_stream << endl;

		section += 1;
		o_textfile_stream << section << ")";
		o_textfile_stream << " Duplicates among SNPIDs:" << endl;
		o_textfile_stream << endl;

		print = true;
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::MARKER) == 0) {
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
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::CHR) == 0) {
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
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if ((strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE1) == 0) ||
					(strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE2) == 0)) {
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
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::STRAND) == 0) {
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

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::N_TOTAL) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					} else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::FREQLABEL) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					} else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::OEVAR_IMP) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					} else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::EFFECT) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					} else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::STDERR) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					} else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::PVALUE) == 0) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					} else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if 	((strcmp((*metas_it)->get_common_name(), Descriptor::MARKER) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::CHR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE1) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE2) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::STRAND) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::N_TOTAL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::FREQLABEL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::OEVAR_IMP) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::EFFECT) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::STDERR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::PVALUE) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::IMPUTED) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::USED_FOR_IMP) != 0)) {
					subsection += 1;
					o_textfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_textfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")" << endl;
					} else {
						o_textfile_stream << " " << (*metas_it)->get_actual_name() << endl;
					}
					o_textfile_stream << endl;

					(*metas_it)->print(o_textfile_stream);

					o_textfile_stream << endl;
				}
			}
		}

		for (cross_table_metas_it = cross_table_metas.begin(); cross_table_metas_it != cross_table_metas.end(); cross_table_metas_it++) {
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
		for (dependency_it = dependencies.begin(); dependency_it != dependencies.end(); dependency_it++) {
			for (numeric_metas_it = numeric_metas.begin(); numeric_metas_it != numeric_metas.end(); numeric_metas_it++) {
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
			} else {
				print = true;
			}
		}
		o_textfile_stream << endl;

		o_textfile_stream << resetiosflags(o_textfile_stream.flags());
		o_textfile_stream << "# " << setfill('-') << setw(76) << " #" << endl;
		o_textfile_stream << endl;
		o_textfile_stream << "THE QUALITY CONTROL IS FINISHED" << endl;
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_txt_report()", __LINE__, 17, o_textfile_name);
	}

	try {
		o_textfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_txt_report()", __LINE__, 16, o_textfile_name);
	}
}

void Analyzer::print_csv_report() throw (AnalyzerException) {
	const char* output_prefix = NULL;
	const char* file_name = NULL;
	const char* file_extension = NULL;
	const char* file_abbreviation = NULL;
	char* o_csvfile_name = NULL;

	ofstream o_csvfile_stream;

	if (gwafile == NULL) {
		return;
	}

	output_prefix = gwafile->get_descriptor()->get_property(Descriptor::PREFIX);
	file_extension = gwafile->get_descriptor()->get_extension();
	file_abbreviation = gwafile->get_descriptor()->get_abbreviation();
	if (file_abbreviation == NULL) {
		file_name = gwafile->get_descriptor()->get_name();
		if ((file_extension != NULL) &&
				((auxiliary::strcmp_ignore_case(file_extension, TXT_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, CSV_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, HTML_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, HTM_EXTENSION) == 0))) {
			auxiliary::transform_file_name(&o_csvfile_name, output_prefix, file_name, CSV_EXTENSION, false);
		} else {
			auxiliary::transform_file_name(&o_csvfile_name, output_prefix, file_name, CSV_EXTENSION, true);
		}
	} else {
		auxiliary::transform_file_name(&o_csvfile_name, output_prefix, file_abbreviation, CSV_EXTENSION, true);
	}

	if (o_csvfile_name == NULL) {
		throw AnalyzerException("Analyzer", "print_csv_report()", __LINE__, 14);
	}

	o_csvfile_stream.exceptions(ofstream::failbit | ofstream::badbit);

	try {
		o_csvfile_stream.open(o_csvfile_name);
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_csv_report()", __LINE__, 15, o_csvfile_name);
	}

	try {
		vector<MetaFiltered*>::iterator filtered_metas_it;
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_source()->get_n();
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_n();
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						o_csvfile_stream << separator << (*filtered_metas_it)->get_na();
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						if (!ISNAN((*filtered_metas_it)->get_mean())) {
							o_csvfile_stream << separator << (*filtered_metas_it)->get_mean();
						} else {
							o_csvfile_stream << separator << "nan";
						}
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						if (!ISNAN((*filtered_metas_it)->get_sd())) {
							o_csvfile_stream << separator << (*filtered_metas_it)->get_sd();
						} else {
							o_csvfile_stream << separator << "nan";
						}
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						if (!ISNAN((*filtered_metas_it)->get_min())) {
							o_csvfile_stream << separator << (*filtered_metas_it)->get_min();
						} else {
							o_csvfile_stream << separator << "nan";
						}
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						if (!ISNAN((*filtered_metas_it)->get_max())) {
							o_csvfile_stream << separator << (*filtered_metas_it)->get_max();
						} else {
							o_csvfile_stream << separator << "nan";
						}
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						if (!ISNAN((*filtered_metas_it)->get_median())) {
							o_csvfile_stream << separator << (*filtered_metas_it)->get_median();
						} else {
							o_csvfile_stream << separator << "nan";
						}
					} else {
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						if (!ISNAN((*filtered_metas_it)->get_skew())) {
							o_csvfile_stream << separator << (*filtered_metas_it)->get_skew();
						} else {
							o_csvfile_stream << separator << "nan";
						}
					} else {
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

		ratio_metas_it = ratio_metas.begin();
		if ( (ratio_metas_it != ratio_metas.end()) &&
				( (*ratio_metas_it)->get_source_a()->is_numeric() ) &&
				( (*ratio_metas_it)->get_source_b()->is_numeric() ) ) {
			for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
				if (!ISNAN((*ratio_metas_it)->get_skewness()[j]) ) {
					o_csvfile_stream << separator << (*ratio_metas_it)->get_skewness()[j];
				} else {
					o_csvfile_stream << separator << "nan";
				}
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
			filtered_metas_it = filtered_metas.begin();
			while (filtered_metas_it != filtered_metas.end()) {
				if (strcmp((*filtered_metas_it)->get_actual_name(), csv_columns[i]) == 0) {
					if ((*filtered_metas_it)->is_numeric()) {
						if (!ISNAN((*filtered_metas_it)->get_kurtosis())) {
							o_csvfile_stream << separator << (*filtered_metas_it)->get_kurtosis();
						} else {
							o_csvfile_stream << separator << "nan";
						}
					} else {
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

		ratio_metas_it = ratio_metas.begin();
		if ((ratio_metas_it != ratio_metas.end()) &&
				((*ratio_metas_it)->get_source_a()->is_numeric()) &&
				((*ratio_metas_it)->get_source_b()->is_numeric())) {
			for (unsigned int j = 0; j < MetaRatio::SLICES_CNT; j++) {
				if (!ISNAN((*ratio_metas_it)->get_kurtosis()[j])) {
					o_csvfile_stream << separator << (*ratio_metas_it)->get_kurtosis()[j];
				} else {
					o_csvfile_stream << separator << "nan";
				}
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
		throw AnalyzerException("Analyzer", "print_csv_report()", __LINE__, 17, o_csvfile_name);
	}

	try {
		o_csvfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_csv_report()", __LINE__, 16, o_csvfile_name);
	}
}

void Analyzer::create_plots(vector<Plot*>& plots) throw (AnalyzerException) {
	Descriptor* descriptor = NULL;
	const char* file_name = NULL;
	const char* file_extension = NULL;
	const char* file_abbreviation = NULL;
	const char* output_prefix = NULL;

	Plot* plot = NULL;
	const char* current_plot = NULL;
	const char* previous_plot = NULL;
	const char* column_name = NULL;
	Plottable* plottable = NULL;
	vector<Plottable*>::iterator plottable_it;

	if (gwafile == NULL) {
		return;
	}

	try {
		descriptor = gwafile->get_descriptor();
		file_abbreviation = descriptor->get_abbreviation();
		file_name = descriptor->get_name();
		file_extension = descriptor->get_extension();
		output_prefix = descriptor->get_property(Descriptor::PREFIX);

		for (int i = 0; i < SINGLE_COLUMN_PLOTS_SIZE; i++) {
			current_plot = single_column_plots[i][0];
			column_name = single_column_plots[i][1];

			if ((previous_plot == NULL) || (strcmp(current_plot, previous_plot) != 0)) {
				previous_plot = current_plot;

				plot = NULL;
				for (plottable_it = this->plots.begin(); plottable_it != this->plots.end(); plottable_it++) {
					plottable = *plottable_it;
					if (strcmp(plottable->get_plot_name(), column_name) == 0) {
						if ((plottable->is_plottable()) && ((plottable->get_boxplot() != NULL) || (plottable->get_histogram() != NULL))) {
							plot = new Plot();
							plots.push_back(plot);
						}
						break;
					}
				}

				if (plot != NULL) {
					if (file_abbreviation != NULL) {
						plot->set_formatted_name("%s%s_%s", output_prefix, file_abbreviation, descriptor->get_column(column_name));
					} else if ((file_extension != NULL) &&
							((auxiliary::strcmp_ignore_case(file_extension, TXT_EXTENSION) == 0) ||
								(auxiliary::strcmp_ignore_case(file_extension, CSV_EXTENSION) == 0) ||
									(auxiliary::strcmp_ignore_case(file_extension, HTML_EXTENSION) == 0) ||
										(auxiliary::strcmp_ignore_case(file_extension, HTM_EXTENSION) == 0))) {
						plot->set_formatted_name("%s%.*s_%s", output_prefix, file_extension - file_name, file_name, descriptor->get_column(column_name));
					} else {
						plot->set_formatted_name("%s%s_%s", output_prefix, file_name, descriptor->get_column(column_name));
					}
					plot->set_formatted_title("Distribution of %s", gwafile->get_descriptor()->get_column(column_name));

					if(plottable->get_histogram() != NULL) {
						plot->add_histogram(plottable->get_histogram());
					}

					if (plottable->get_boxplot() != NULL) {
						plot->add_boxplot(plottable->get_boxplot());
					}

					plottable->set_plot(plot);
				}
			} else if (plot != NULL) {
				for (plottable_it = this->plots.begin(); plottable_it != this->plots.end(); plottable_it++) {
					plottable = *plottable_it;
					if (strcmp(plottable->get_plot_name(), column_name) == 0) {
						if (plottable->is_plottable()) {
							if (plottable->get_histogram() != NULL) {
								plot->add_histogram(plottable->get_histogram());
							}

							if (plottable->get_boxplot() != NULL) {
								plot->add_boxplot(plottable->get_boxplot());
							}
						}
						break;
					}
				}
			}
		}
	} catch (Exception &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "create_plots( vector<Plot*>& )", __LINE__);
		throw new_e;
	}
}


void Analyzer::create_combined_qqplots(vector<Plot*>& plots) throw (AnalyzerException) {
	Plot* plot = NULL;
	Qqplot* qqplot = NULL;

	const char* plot_name = NULL;
	int column_from = 0;
	int column_to = 0;

	const char* file_name = NULL;
	const char* file_extension = NULL;
	const char* file_abbreviation = NULL;
	const char* output_prefix = NULL;

	if (gwafile == NULL) {
		return;
	}

	try {
		file_name = gwafile->get_descriptor()->get_name();
		file_extension = gwafile->get_descriptor()->get_extension();
		file_abbreviation = gwafile->get_descriptor()->get_abbreviation();
		output_prefix = gwafile->get_descriptor()->get_property(Descriptor::PREFIX);

		plot_name = combined_qqplots[column_to][0];
		while (column_to < COMBINED_QQPLOTS_SIZE) {
			if (strcmp(plot_name, combined_qqplots[column_to][0]) != 0) {
				qqplot = create_combined_qqplot(column_from, column_to);

				if (qqplot != NULL) {
					plot = new Plot();
					if (file_abbreviation != NULL) {
						plot->set_formatted_name("%s%s_%s", output_prefix, file_abbreviation, plot_name);
					} else if ((file_extension != NULL) &&
							((auxiliary::strcmp_ignore_case(file_extension, TXT_EXTENSION) == 0) ||
								(auxiliary::strcmp_ignore_case(file_extension, CSV_EXTENSION) == 0) ||
									(auxiliary::strcmp_ignore_case(file_extension, HTML_EXTENSION) == 0) ||
										(auxiliary::strcmp_ignore_case(file_extension, HTM_EXTENSION) == 0))) {
						plot->set_formatted_name("%s%.*s_%s", output_prefix, file_extension - file_name, file_name, plot_name);
					} else {
						plot->set_formatted_name("%s%s_%s", output_prefix, file_name, plot_name);
					}
					plot->set_title(plot_name);
					plot->add_qqplot(qqplot);
					plots.push_back(plot);
				}

				column_from = column_to;
				plot_name = combined_qqplots[column_to][0];
			}

			column_to++;
		}

		qqplot = create_combined_qqplot(column_from, column_to);

		if (qqplot != NULL) {
			plot = new Plot();
			if (file_abbreviation != NULL) {
				plot->set_formatted_name("%s%s_%s", output_prefix, file_abbreviation, plot_name);
			} else if ((file_extension != NULL) &&
					((auxiliary::strcmp_ignore_case(file_extension, TXT_EXTENSION) == 0) ||
						(auxiliary::strcmp_ignore_case(file_extension, CSV_EXTENSION) == 0) ||
							(auxiliary::strcmp_ignore_case(file_extension, HTML_EXTENSION) == 0) ||
								(auxiliary::strcmp_ignore_case(file_extension, HTM_EXTENSION) == 0))) {
				plot->set_formatted_name("%s%.*s_%s", output_prefix, file_extension - file_name, file_name, plot_name);
			} else {
				plot->set_formatted_name("%s%s_%s", output_prefix, file_name, plot_name);
			}
			plot->set_title(plot_name);
			plot->add_qqplot(qqplot);
			plots.push_back(plot);
		}
	} catch (Exception &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "create_combined_qqplot( vector<Plot*>& )", __LINE__);
		throw new_e;
	}
}

Qqplot* Analyzer::create_combined_qqplot(int column_from, int column_to) throw (PlotException) {
	Qqplot* qqplot = NULL;
	Qqplot* combined_qqplot = NULL;
	vector<Plottable*>::iterator plots_it = plots.begin();
	int i = column_from;

	while (i < column_to) {
		while (plots_it != plots.end()) {
			if (strcmp((*plots_it)->get_plot_name() , combined_qqplots[i][1]) == 0) {
				if ((qqplot = (*plots_it)->get_qqplot()) != NULL) {
					combined_qqplot = Qqplot::merge(combined_qqplot, qqplot);
				}

				break;
			}
			plots_it++;
		}
		i += 1;
		plots_it = plots.begin();
	}

	return combined_qqplot;
}

void Analyzer::create_combined_boxplots(vector<Plot*>& plots) throw (AnalyzerException) {
	Plottable* plottable = NULL;
	vector<Plottable*>::iterator plottable_it;
	Plot* plot = NULL;
	Boxplot* boxplot = NULL;
	Meta* width_column = NULL;
	vector<Meta*>::iterator metas_it;
	const char* width_column_name = NULL;
	double width = numeric_limits<double>::quiet_NaN();
	const char* file_abbreviation = NULL;

	if (gwafile == NULL) {
		return;
	}

	try {
		width_column_name = gwafile->get_descriptor()->get_property(Descriptor::BOXPLOTWIDTH);
		if ((width_column_name != NULL) && (strlen(width_column_name) > 0)) {
			for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
				if ((*metas_it != NULL) && (strcmp((*metas_it)->get_common_name(), width_column_name) == 0)) {
					width_column = *metas_it;
					break;
				}
			}

			if (width_column != NULL) {
				width = ((MetaNumeric*)width_column)->get_median();
			}
		}

		for (int i = 0; i < COMBINED_BOXPLOTS_SIZE; i++) {
			plot = NULL;
			for (plottable_it = this->plots.begin(); plottable_it != this->plots.end(); plottable_it++) {
				plottable = *plottable_it;
				if (strcmp(plottable->get_plot_name(), combined_boxplots[i][0]) == 0) {
					if ((plottable->is_plottable()) && (plottable->get_boxplot() != NULL)) {
						plot = new Plot();
						plots.push_back(plot);
					}
					break;
				}
			}

			if (plot != NULL) {
				plot->set_formatted_name("boxplots_%s", combined_boxplots[i][0]);
				plot->set_title(combined_boxplots[i][1]);

				boxplot = plottable->get_boxplot();
				boxplot->set_width(1, width);
				if ((file_abbreviation = gwafile->get_descriptor()->get_abbreviation()) != NULL) {
					boxplot->set_name(1, file_abbreviation);
				} else {
					boxplot->set_name(1, gwafile->get_descriptor()->get_name(), 12);
				}

				plot->add_boxplot(boxplot);
			}
		}
	} catch (Exception &e) {
		AnalyzerException new_e(e);
		new_e.add_message("Analyzer", "create_combined_boxplots( vector<Plot*>& )", __LINE__);
		throw new_e;
	}
}

void Analyzer::print_html_report(char** html_report_path, vector<Plot*>& plots, const char* resource_path) throw (AnalyzerException) {
	const char* output_prefix = NULL;
	const char* file_name = NULL;
	const char* file_extension = NULL;
	const char* file_abbreviation = NULL;
	char* jscript = NULL;
	char* style = NULL;

	ofstream o_htmlfile_stream;
	ifstream i_resource_stream;

	if (gwafile == NULL) {
		return;
	}

	if (resource_path == NULL) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 0, "resource_path");
	}

	output_prefix = gwafile->get_descriptor()->get_property(Descriptor::PREFIX);
	file_extension = gwafile->get_descriptor()->get_extension();
	file_abbreviation = gwafile->get_descriptor()->get_abbreviation();
	if (file_abbreviation == NULL) {
		file_name = gwafile->get_descriptor()->get_name();
		if ((file_extension != NULL) &&
				((auxiliary::strcmp_ignore_case(file_extension, TXT_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, CSV_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, HTML_EXTENSION) == 0) ||
				(auxiliary::strcmp_ignore_case(file_extension, HTM_EXTENSION) == 0))) {
			auxiliary::transform_file_name(html_report_path, output_prefix, file_name, HTML_EXTENSION, false);
		} else {
			auxiliary::transform_file_name(html_report_path, output_prefix, file_name, HTML_EXTENSION, true);
		}
	} else {
		auxiliary::transform_file_name(html_report_path, output_prefix, file_abbreviation, HTML_EXTENSION, true);
	}

	if (*html_report_path == NULL) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 14);
	}

	jscript = (char*)malloc((strlen(resource_path) + strlen(JSCRIPT) + 1) * sizeof(char));
	if (jscript == NULL) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 2, (strlen(resource_path) + strlen(JSCRIPT) + 1) * sizeof(char));
	}
	strcpy(jscript, resource_path);
	strcat(jscript, JSCRIPT);

	style = (char*)malloc((strlen(resource_path) + strlen(CONTENT_STYLE) + 1) * sizeof(char));
	if (style == NULL) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 2, (strlen(resource_path) + strlen(CONTENT_STYLE) + 1) * sizeof(char));
	}
	strcpy(style, resource_path);
	strcat(style, CONTENT_STYLE);

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);
	i_resource_stream.exceptions(ifstream::failbit | ifstream::badbit);
	i_resource_stream.setf(ifstream::skipws);

	try {
		o_htmlfile_stream.open(*html_report_path);
		i_resource_stream.open(jscript);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	} catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 15, *html_report_path);
		} else {
			throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 18, jscript);
		}
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
		o_htmlfile_stream << "<head>" << endl;
		o_htmlfile_stream << "<title>" << VERSION << "</title>" << endl;
		o_htmlfile_stream << "<script type = \"text/javascript\">" << endl;
		o_htmlfile_stream << "// <![CDATA[" << endl;

		while (!i_resource_stream.eof()) {
			o_htmlfile_stream << (char)i_resource_stream.get();
		}
	} catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 17, *html_report_path);
		} else if (!i_resource_stream.eof()) {
			throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 20, jscript);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 19, jscript);
	}

	try {
		i_resource_stream.open(style);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 18, style);
	}

	try {
		o_htmlfile_stream << endl;
		o_htmlfile_stream << "// ]]>" << endl;
		o_htmlfile_stream << "</script>" << endl;
		o_htmlfile_stream << "<style type = \"text/css\">" << endl;

		while (!i_resource_stream.eof()) {
			o_htmlfile_stream << (char)i_resource_stream.get();
		}
	} catch (fstream::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 17, *html_report_path);
		} else if (!i_resource_stream.eof()) {
			throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 20, style);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 19, style);
	}

	try {
		time_t system_time = time(NULL);
		char* current_time = asctime(localtime(&system_time));
		unsigned int section = 0, subsection = 0;
		bool print = true;

		vector<Meta*>::iterator metas_it;
		vector<MetaNumeric*>::iterator numeric_metas_it;
		vector<MetaCrossTable*>::iterator cross_table_metas_it;
		vector< pair<pair<Meta*, Meta*>, int> >::iterator dependency_it;
		vector<char*>::iterator vector_char_it;

		stringstream string_stream;

		o_htmlfile_stream << "</style>";
		o_htmlfile_stream << "</head>";
		o_htmlfile_stream << "<body>";

		o_htmlfile_stream << "<div class = \"header\"><div class = \"headertext\">";
		o_htmlfile_stream << "<h1>"<< VERSION <<"</h1><hr />";
		o_htmlfile_stream << "<p>" << gwafile->get_descriptor()->get_name() << "<br />" << current_time << "</p>";
		o_htmlfile_stream << "</div></div>";

		o_htmlfile_stream << "<div class = \"section\">";
		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";

		if (missing_columns.empty()) {
			o_htmlfile_stream << " All the required columns are present";
			o_htmlfile_stream << "</p>";
		} else {
			o_htmlfile_stream << " The following required columns are missing or have different cases:";
			o_htmlfile_stream << "</p>";
			o_htmlfile_stream << "<div class = \"groups\">";
			o_htmlfile_stream << "<table class = \"groups\">";
			o_htmlfile_stream << "<tr><th>Column Name</th></tr>";

			vector_char_it = missing_columns.begin();
			while (vector_char_it != missing_columns.end()) {
				o_htmlfile_stream << "<tr><td>" << *vector_char_it << "</td></tr>";
				vector_char_it++;
			}

			o_htmlfile_stream << "</table>";
			o_htmlfile_stream << "</div>";
		}

		section += 1;
		o_htmlfile_stream << "<p class = \"sectionname\">";
		o_htmlfile_stream << section << ")";

		if (not_required_columns.empty()) {
			o_htmlfile_stream << " No additional columns were present";
			o_htmlfile_stream << "</p>";
		} else {
			o_htmlfile_stream << " The following columns do not match required column names or were not required:";
			o_htmlfile_stream << "</p>";
			o_htmlfile_stream << "<div class = \"groups\">";
			o_htmlfile_stream << "<table class = \"groups\">";
			o_htmlfile_stream << "<tr><th>Column Name</th></tr>";

			vector_char_it = not_required_columns.begin();
			while (vector_char_it != not_required_columns.end()) {
				o_htmlfile_stream << "<tr><td>" << *vector_char_it << "</td></tr>";
				vector_char_it++;
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
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::MARKER) == 0) {
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
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
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::CHR) == 0) {
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
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
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if ((strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE1) == 0) ||
					(strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE2) == 0)) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ") " << (*metas_it)->get_actual_name();
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
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
		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::STRAND) == 0) {
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
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

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::N_TOTAL) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					} else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::FREQLABEL) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					} else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::OEVAR_IMP) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					} else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::EFFECT) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					} else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::STDERR) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					} else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
					o_htmlfile_stream << "</div>";
					break;
				}
			}
		}

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if (strcmp((*metas_it)->get_common_name(), Descriptor::PVALUE) == 0) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					} else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());

					print = false;
					vector<Plot*>::iterator plots_it;
					if (combined_qqplots != NULL) {
						for (plots_it = plots.begin(); plots_it != plots.end(); plots_it++) {
							if (((*plots_it)->get_qqplots() != NULL) && ((*plots_it)->get_qqplots()->size() > 0)) {
								print = true;
								break;
							}
						}
					}

					if (print) {
						o_htmlfile_stream << "<div class = \"container\">";
						o_htmlfile_stream << "<div class = \"spacer\">&nbsp;</div>";

						for (plots_it = plots.begin(); plots_it != plots.end(); plots_it++) {
							if (((*plots_it)->get_qqplots() != NULL) && ((*plots_it)->get_qqplots()->size() > 0)) {
								o_htmlfile_stream << "<div class = \"float\">";
								o_htmlfile_stream << "<a href = \"" << (auxiliary::is_absolute_path((*plots_it)->get_name(), gwafile->get_descriptor()->get_path_separator()) ? "file:///" : "") << (*plots_it)->get_name() << ".png\">";
								o_htmlfile_stream << "<img src = \"" << (auxiliary::is_absolute_path((*plots_it)->get_name(), gwafile->get_descriptor()->get_path_separator()) ? "file:///" : "") << (*plots_it)->get_name() << ".png\" alt = \"" << (*plots_it)->get_name() << ".png\" title = \"Click to enlarge the figure\" width = \"200\" height = \"200\" />";
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

		for (metas_it = metas.begin(); metas_it != metas.end(); metas_it++) {
			if (*metas_it != NULL) {
				if 	((strcmp((*metas_it)->get_common_name(), Descriptor::MARKER) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::CHR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE1) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::ALLELE2) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::STRAND) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::N_TOTAL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::FREQLABEL) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::OEVAR_IMP) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::EFFECT) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::STDERR) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::PVALUE) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::IMPUTED) != 0) &&
					(strcmp((*metas_it)->get_common_name(), Descriptor::USED_FOR_IMP) != 0)) {
					subsection += 1;
					o_htmlfile_stream << "<div class = \"subsection\">";
					o_htmlfile_stream << "<p class = \"subsectionname\">";
					o_htmlfile_stream << section << "." << subsection << ")";
					if ((*metas_it)->get_full_name() != NULL) {
						o_htmlfile_stream << " " << (*metas_it)->get_full_name() << " (" << (*metas_it)->get_actual_name() << ")";
					} else {
						o_htmlfile_stream << " " << (*metas_it)->get_actual_name();
					}
					o_htmlfile_stream << "</p>";
					o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
					(*metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
					o_htmlfile_stream << "</div>";
				}
			}
		}

		for (cross_table_metas_it = cross_table_metas.begin(); cross_table_metas_it != cross_table_metas.end(); cross_table_metas_it++) {
			subsection += 1;
			o_htmlfile_stream << "<div class = \"subsection\">";
			o_htmlfile_stream << "<p class = \"subsectionname\">";
			o_htmlfile_stream << section << "." << subsection << ") " << (*cross_table_metas_it)->get_full_name();
			o_htmlfile_stream << "</p>";
			o_htmlfile_stream << "<button onclick=\"minmax(this);\">-</button>";
			(*cross_table_metas_it)->print_html(o_htmlfile_stream, gwafile->get_descriptor()->get_path_separator());
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
		for (dependency_it = dependencies.begin(); dependency_it != dependencies.end(); dependency_it++) {
			print = true;

			for (numeric_metas_it = numeric_metas.begin(); numeric_metas_it != numeric_metas.end(); numeric_metas_it++) {
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

			for (dependency_it = dependencies.begin(); dependency_it != dependencies.end(); dependency_it++) {
				print = true;

				for (numeric_metas_it = numeric_metas.begin(); numeric_metas_it != numeric_metas.end(); numeric_metas_it++) {
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
		} else {
			o_htmlfile_stream << "<div class = \"result\">";
			o_htmlfile_stream << "None";
			o_htmlfile_stream << "</div>";
		}
		o_htmlfile_stream << "</div>";

		o_htmlfile_stream << "</body>";
		o_htmlfile_stream << "</html>";
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>&  , const char* )", __LINE__, 17, *html_report_path);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_html_report( char** , vector<Plot*>& , const char* )", __LINE__, 16, *html_report_path);
	}
}

void Analyzer::print_main_html(vector<const char*>& gwa_file_names, vector<const char*>& html_report_paths, vector<const char*>& boxplot_names, const char* resource_path, char path_separator) throw (AnalyzerException) {
	char* o_mainfile_name = NULL;
	char* o_menufile_name = NULL;
	char* o_boxplotsfile_name = NULL;

	if (gwa_file_names.size() <= 0) {
		throw AnalyzerException("Analyzer", "print_main_html( vector<const char*>& , vector<const char*>& , vector<const char*>& , const char* )", __LINE__, 1, "gwa_file_names");
	}

	if (html_report_paths.size() <= 0) {
		throw AnalyzerException("Analyzer", "print_main_html( vector<const char*>& , vector<const char*>& , vector<const char*>& , const char* )", __LINE__, 1, "html_report_paths");
	}

	if (gwa_file_names.size() != html_report_paths.size()) {
		throw AnalyzerException("Analyzer", "print_main_html( vector<const char*>& , vector<const char*>& , vector<const char*>& , const char* )", __LINE__, 1, "gwa_file_names");
	}

	if (resource_path == NULL) {
		throw AnalyzerException("Analyzer", "print_main_html( vector<const char*>& , vector<const char*>& , vector<const char*>& , const char* )", __LINE__, 0, "resource_path");
	}

	o_mainfile_name = (char*)malloc((strlen(MAIN_FILE) + 1) * sizeof(char));
	if (o_mainfile_name == NULL) {
		throw AnalyzerException("Analyzer", "print_main_html( vector<const char*>& , vector<const char*>& , vector<const char*>& , const char* )", __LINE__, 2, (strlen(MAIN_FILE) + 1) * sizeof(char));
	}
	strcpy(o_mainfile_name, MAIN_FILE);

	o_menufile_name = (char*)malloc((strlen(MENU_FILE) + 1) * sizeof(char));
	if (o_menufile_name == NULL) {
		throw AnalyzerException("Analyzer", "print_main_html( vector<const char*>& , vector<const char*>& , vector<const char*>& , const char* )", __LINE__, 2, (strlen(MENU_FILE) + 1) * sizeof(char));
	}
	strcpy(o_menufile_name, MENU_FILE);

	if (boxplot_names.size() > 0) {
		o_boxplotsfile_name = (char*)malloc((strlen(BOXPLOTS_FILE) + 1) * sizeof(char));
		if (o_boxplotsfile_name == NULL) {
			throw AnalyzerException("Analyzer", "print_main_html( vector<const char*>& , vector<const char*>& , vector<const char*>& , const char* )", __LINE__, 2, (strlen(BOXPLOTS_FILE) + 1) * sizeof(char));
		}
		strcpy(o_boxplotsfile_name, BOXPLOTS_FILE);

		print_boxplots_html(boxplot_names, o_boxplotsfile_name, resource_path);
	}

	print_menu_html(gwa_file_names, html_report_paths, o_boxplotsfile_name, o_menufile_name, resource_path, path_separator);
	print_frameset_html(o_menufile_name, o_mainfile_name, html_report_paths.front(), path_separator);
}

void Analyzer::print_boxplots_html(vector<const char*>& boxplot_names, const char* html_output_name, const char* resource_path) throw (AnalyzerException) {
	char* jscript = NULL;
	char* style = NULL;

	ofstream o_htmlfile_stream;
	ifstream i_resource_stream;

	if (boxplot_names.size() <= 0) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 1, "boxplot_names");
	}

	if (html_output_name == 0) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 0, "html_output_name");
	}

	if (strlen(html_output_name) <= 0) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 1, "html_output_name");
	}

	if (resource_path == 0) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 0, "resource_path");
	}

	jscript = (char*)malloc((strlen(resource_path) + strlen(JSCRIPT) + 1) * sizeof(char));
	if (jscript == NULL) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 2, (strlen(resource_path) + strlen(JSCRIPT) + 1) * sizeof(char));
	}
	strcpy(jscript, resource_path);
	strcat(jscript, JSCRIPT);

	style = (char*)malloc((strlen(resource_path) + strlen(CONTENT_STYLE) + 1) * sizeof(char));
	if (style == NULL) {
		throw AnalyzerException("Analyzer", "print_boxlplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 2, (strlen(resource_path) + strlen(CONTENT_STYLE) + 1) * sizeof(char));
	}
	strcpy(style, resource_path);
	strcat(style, CONTENT_STYLE);

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);
	i_resource_stream.exceptions(ifstream::failbit | ifstream::badbit);
	i_resource_stream.setf(ifstream::skipws);

	try {
		o_htmlfile_stream.open(html_output_name);
		i_resource_stream.open(jscript);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	} catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 15, html_output_name);
		} else {
			throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 18, jscript);
		}
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
		o_htmlfile_stream << "<head>" << endl;
		o_htmlfile_stream << "<title>" << VERSION << "</title>" << endl;
		o_htmlfile_stream << "<script type = \"text/javascript\">" << endl;
		o_htmlfile_stream << "// <![CDATA[" << endl;

		while(!i_resource_stream.eof()) {
			o_htmlfile_stream << (char)i_resource_stream.get();
		}
	} catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 17, html_output_name);
		} else if (!i_resource_stream.eof()) {
			throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 20, jscript);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 19, jscript);
	}

	try {
		i_resource_stream.open(style);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	} catch (ifstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 18, style);
	}

	try {
		o_htmlfile_stream << endl;
		o_htmlfile_stream << "// ]]>" << endl;
		o_htmlfile_stream << "</script>" << endl;
		o_htmlfile_stream << "<style type = \"text/css\">" << endl;

		while(!i_resource_stream.eof()) {
			o_htmlfile_stream << (char)i_resource_stream.get();
		}
	} catch (fstream::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 17, html_output_name);
		} else if (!i_resource_stream.eof()) {
			throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 20, style);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 19, style);
	}

	try {
		vector<const char*>::iterator boxplot_names_it;
		time_t system_time = time(NULL);
		char* current_time = asctime(localtime(&system_time));

		int section = 1;

		o_htmlfile_stream << "</style>";
		o_htmlfile_stream << "</head>";

		o_htmlfile_stream << "<body>";

		o_htmlfile_stream << "<div class = \"header\">";
		o_htmlfile_stream << "<div class = \"headertext\">";
		o_htmlfile_stream << "<h1>" << VERSION << "</h1>";
		o_htmlfile_stream << "<hr />";
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

		for (boxplot_names_it = boxplot_names.begin(); boxplot_names_it != boxplot_names.end(); boxplot_names_it++) {
			o_htmlfile_stream << "<div class = \"float\">";
			o_htmlfile_stream << "<a href = \"" << *boxplot_names_it << "\">";
			o_htmlfile_stream << "<img src = \"" << *boxplot_names_it << "\" title = \"Click to enlarge the figure\" width = \"400\" height = \"200\" />";
			o_htmlfile_stream << "</a>";
			o_htmlfile_stream << "</div>";
		}

		o_htmlfile_stream << "<div class = \"spacer\">&nbsp;</div>";
		o_htmlfile_stream << "</div>";
		o_htmlfile_stream << "</div>";

		o_htmlfile_stream << "</body>";
		o_htmlfile_stream << "</html>";
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 17, html_output_name);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_boxplots_html( vector<const char*>& , const char* , const char* )", __LINE__, 16, html_output_name);
	}
}

void Analyzer::print_menu_html(vector<const char*>& gwa_file_names, vector<const char*>& html_report_paths, const char* boxplots_html, const char* html_output_name, const char* resource_path, char path_separator) throw (AnalyzerException) {
	char* style = NULL;

	ofstream o_htmlfile_stream;
	ifstream i_resource_stream;

	if (gwa_file_names.size() != html_report_paths.size()) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 1, "gwa_file_names");
	}

	if ((boxplots_html != NULL) && (strlen(boxplots_html) <= 0)) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 1, "boxplots_html");
	}

	if (html_output_name == NULL) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 0, "html_output_name");
	}

	if (strlen(html_output_name) <= 0) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 1, "html_output_name");
	}

	if (resource_path == NULL) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 0, "resource_path");
	}

	style = (char*)malloc((strlen(resource_path) + strlen(MENU_STYLE) + 1) * sizeof(char));
	if (style == NULL) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 2, (strlen(resource_path) + strlen(MENU_STYLE) + 1) * sizeof(char));
	}
	strcpy(style, resource_path);
	strcat(style, MENU_STYLE);

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);
	i_resource_stream.exceptions(ifstream::failbit | ifstream::badbit);
	i_resource_stream.setf(ifstream::skipws);

	try {
		o_htmlfile_stream.open(html_output_name);
		i_resource_stream.open(style);
		i_resource_stream.seekg((streampos)0, ifstream::beg);
	} catch (ios_base::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 15, html_output_name);
		} else {
			throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 18, style);
		}
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">";
		o_htmlfile_stream << "<head>";
		o_htmlfile_stream << "<title>" << VERSION << "</title>";

		o_htmlfile_stream << "<style type = \"text/css\">" << endl;

		while(!i_resource_stream.eof()) {
			o_htmlfile_stream << (char)i_resource_stream.get();
		}
	}
	catch (fstream::failure &e) {
		if (o_htmlfile_stream.fail()) {
			throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 17, html_output_name);
		} else if (!i_resource_stream.eof()) {
			throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 20, style);
		}
	}

	try {
		i_resource_stream.close();
		i_resource_stream.clear();
	} catch (ifstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 19, style);
	}

	try {
		o_htmlfile_stream << "</style>";
		o_htmlfile_stream << "</head>";

		o_htmlfile_stream << "<body>";
		o_htmlfile_stream << "<div class = \"panel\">";
		o_htmlfile_stream << "<div class = \"menu\">";
		o_htmlfile_stream << "<h1>File</h1>";
		o_htmlfile_stream << "<hr />";

		for (unsigned int i = 0; i < gwa_file_names.size(); i++) {
			o_htmlfile_stream << "<a href = \"" << (auxiliary::is_absolute_path(html_report_paths.at(i), path_separator) ? "file:///" : "") << html_report_paths.at(i) << "\" target = \"trait\">";
			o_htmlfile_stream << gwa_file_names.at(i) << "</a><br />";
		}
		o_htmlfile_stream << "<hr />";

		if (boxplots_html != NULL) {
			o_htmlfile_stream << "<a href = \"" << (auxiliary::is_absolute_path(boxplots_html, path_separator) ? "file:///" : "") << boxplots_html << "\" target = \"trait\">";
			o_htmlfile_stream << "Between-Study Comparisons";
			o_htmlfile_stream << "</a><br />";
		}

		o_htmlfile_stream << "</div>";
		o_htmlfile_stream << "</div>";
		o_htmlfile_stream << "</body>";
		o_htmlfile_stream << "</html>";
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 17, html_output_name);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_menu_html( vector<const char*>& , vector<const char*>& , const char* , const char* , const char* )", __LINE__, 16, html_output_name);
	}
}

void Analyzer::print_frameset_html(const char* menu_html, const char* html_output_name, const char* first_frame_html, char path_separator) throw (AnalyzerException) {
	ofstream o_htmlfile_stream;

	if (menu_html == NULL) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 0, "menu_html");
	}

	if (strlen(menu_html) <= 0) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 1, "menu_html");
	}

	if (html_output_name == NULL) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 0, "html_output_name");
	}

	if (strlen(html_output_name) <= 0) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 1, "html_output_name");
	}

	if (first_frame_html == NULL) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 0, "first_frame_html");
	}

	if (strlen(first_frame_html) <= 0) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 1, "first_frame_html");
	}

	o_htmlfile_stream.exceptions(ofstream::failbit | ofstream::badbit);

	try {
		o_htmlfile_stream.open(html_output_name);
	} catch (ios_base::failure &e) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 15, html_output_name);
	}

	try {
		o_htmlfile_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">" << endl;
		o_htmlfile_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">";
		o_htmlfile_stream << "<head>";
		o_htmlfile_stream << "<title>" << VERSION << "</title>";
		o_htmlfile_stream << "</head>";
		o_htmlfile_stream << "<frameset cols = \"240, *\">";
		o_htmlfile_stream << "<frame src=\"" << menu_html << "\" noresize=\"noresize\" scrolling=\"no\" frameborder=\"0\" marginwidth=\"0\" />";
		o_htmlfile_stream << "<frame src=\"";
		if (first_frame_html != NULL) {
			o_htmlfile_stream << (auxiliary::is_absolute_path(first_frame_html, path_separator) ? "file:///" : "") << first_frame_html;
		}
		o_htmlfile_stream << "\" name=\"trait\" noresize=\"noresize\" scrolling=\"auto\" frameborder=\"0\" marginwidth=\"0px\" />";

		o_htmlfile_stream << "</frameset>";
		o_htmlfile_stream << "</html>";
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 17, html_output_name);
	}

	try {
		o_htmlfile_stream.close();
	} catch (ofstream::failure &e) {
		throw AnalyzerException("Analyzer", "print_frameset_html( const char* , const char* , const char* )", __LINE__, 16, html_output_name);
	}
}
