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

#include <iostream>
#include <sstream>
#include <ctime>
#include <limits>

#include "include/analyzer/Analyzer.h"
#include "include/comparator/Comparator.h"
#include "include/annotation/Annotation.h"

#include "R.h"
#include "Rinternals.h"

/* Define LINUX flag for compilation under Linux */
#ifndef WIN32
#define LINUX
#else
#undef LINUX
#endif

using namespace std;

extern "C" {

/*
 *	Processes an input script (compatible with METAL).
 *	Arguments:
 *		script_name		-	name of an input script file
 *		file_separator	-	a system specific file separator ("\" or "/")
 *	Returns:
 *		vector			-	First element is an external pointer to vector<File*>
 *							Second element is a number of elements in vector<File*>, i.e. a number of files to process
 */
SEXP QC_process_script(SEXP script_name, SEXP file_separator) {
	vector<File*>* files = NULL;
	const char* c_script_name = NULL;
	const char* c_file_separator = NULL;

	SEXP list = R_NilValue;
	SEXP files_count = R_NilValue;
	SEXP external_pointer = R_NilValue;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	c_script_name = CHAR(STRING_ELT(script_name, 0));

	if ((strlen(c_script_name) == 0) || (strspn(c_script_name, " \t") == strlen(c_script_name))) {
		error("\nThe name of an input script file can't be empty.");
	}

	c_file_separator = CHAR(STRING_ELT(file_separator, 0));

	if ((strlen(c_file_separator) == 0)) {
		error("\nThe system specific file separator was not detected.");
	}

	try {
		Analyzer analyzer;

		Rprintf("Processing input script... ");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		start_time = clock();

		files = analyzer.process_script(c_script_name, c_file_separator[0]);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;

		Rprintf(" Done. (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

	} catch (AnalyzerException &e) {
		if (files != NULL) {
			for (unsigned int i = 0; i < files->size(); i++) {
				delete files->at(i);
			}
			files->clear();
			delete files;
		}
		error("\n%s", e.what());
	} catch (FileException &e) {
		if (files != NULL) {
			for (unsigned int i = 0; i < files->size(); i++) {
				delete files->at(i);
			}
			files->clear();
			delete files;
		}
		error("\n%s", e.what());
	}

	if ((files == NULL) || (files->empty())) {
		error("\nNo files to process are specified in the input script file.");
	}

	PROTECT(list = allocVector(VECSXP, 2));
	PROTECT(files_count = allocVector(INTSXP, 1));

	INTEGER(files_count)[0] = files->size();
	external_pointer = R_MakeExternalPtr((void*)files, R_NilValue, R_NilValue);

	SET_VECTOR_ELT(list, 0, files_count);
	SET_VECTOR_ELT(list, 1, external_pointer);

	UNPROTECT(2);

	return list;
}

/*
 *	Deletes all files that was processed by analyzer.
 *	Arguments:
 *		external_pointer	-	an external pointer to vector<File*>
 *	Returns:
 *		external_pointer	-	an external pointer to empty vector<File*>, that was passed
 */
SEXP QC_delete(SEXP external_pointer) {
	vector<File*>* files = NULL;
	vector<File*>::iterator file_it;

	files = (vector<File*>*) R_ExternalPtrAddr(external_pointer);

	for (file_it = files->begin(); file_it != files->end(); file_it++) {
		delete *file_it;
	}

	files->clear();
	delete files;

	return external_pointer;
}

/*
 *	Creates structures for common plots
 *	Arguments:
 *		external_pointer	-	an external pointer to vector<File*>
 *		resource_path		-	the location of GWASQC package resources (css/jscript)
 *	Returns:
 *		list				-	vector with R data structures for plots
 */
SEXP QC_common_plot(SEXP external_pointer, SEXP resource_path) {
	const char* c_resource_path = NULL;

	vector<File*>* files = NULL;
	vector<Boxplot*>* boxplots = NULL;
	vector<Boxplot*>::iterator boxplots_it;
	int i = 0;

	SEXP list = R_NilValue;
	SEXP plot_name = R_NilValue;
	SEXP plot_title = R_NilValue;

	SEXP boxplot = R_NilValue;
	SEXP tukey = R_NilValue;
	SEXP observations = R_NilValue;
	SEXP notches = R_NilValue;
	SEXP outliers = R_NilValue;
	SEXP groups = R_NilValue;
	SEXP names = R_NilValue;
	SEXP boxplot_names = R_NilValue;
	SEXP widths = R_NilValue;

	Boxplot* c_boxplot = NULL;
	const double* c_tukey = NULL;
	const int* c_observations = NULL;
	const double* c_outliers = NULL;
	const int* c_groups = NULL;
	const char* const* c_names = NULL;
	const double* c_widths = NULL;

	double* r_tukey = NULL;
	double* r_observations = NULL;
	double* r_outliers = NULL;
	double* r_groups = NULL;
	double* r_widths = NULL;
	int outliers_cnt = 0;
	int groups_cnt = 0;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	Rprintf("- Creating plots... ");
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif

	start_time = clock();

	c_resource_path = CHAR(STRING_ELT(resource_path, 0));

	if ((strlen(c_resource_path) == 0) || (strspn(c_resource_path, " \t") == strlen(c_resource_path))) {
		error("\nThe path to the package's resources can't be empty.");
	}

	files = (vector<File*>*)R_ExternalPtrAddr(external_pointer);
	if ((files == NULL) || (files->empty())) {
		error("\nThere are no files in memory for processing.");
	}

	try {
		Analyzer analyzer;

		boxplots = analyzer.create_common_boxplots(*files);

		if ((boxplots != NULL) && ((i = boxplots->size()) > 0)) {
			PROTECT(list = allocVector(VECSXP, i * 4));

			PROTECT(boxplot_names = allocVector(STRSXP, 6));
			SET_STRING_ELT(boxplot_names, 0, mkChar("stats"));
			SET_STRING_ELT(boxplot_names, 1, mkChar("n"));
			SET_STRING_ELT(boxplot_names, 2, mkChar("conf"));
			SET_STRING_ELT(boxplot_names, 3, mkChar("out"));
			SET_STRING_ELT(boxplot_names, 4, mkChar("group"));
			SET_STRING_ELT(boxplot_names, 5, mkChar("names"));

			i = 0;

			for (boxplots_it = boxplots->begin(); boxplots_it != boxplots->end(); boxplots_it++) {
				c_boxplot = *boxplots_it;

				PROTECT(plot_name = allocVector(STRSXP, 1));
				PROTECT(plot_title = allocVector(STRSXP, 1));
				PROTECT(boxplot = allocVector(VECSXP, 6));

				outliers_cnt = c_boxplot->get_outliers_cnt();
				groups_cnt = c_boxplot->get_groups_cnt();

				PROTECT(tukey = allocMatrix(REALSXP, 5, groups_cnt));
				PROTECT(observations = allocVector(REALSXP, groups_cnt));
				PROTECT(notches = allocVector(REALSXP, 0));
				PROTECT(outliers = allocVector(REALSXP, outliers_cnt));
				PROTECT(groups = allocVector(REALSXP, outliers_cnt));
				PROTECT(names = allocVector(STRSXP, groups_cnt));
				PROTECT(widths = allocVector(REALSXP, groups_cnt));

				c_tukey = c_boxplot->get_tukey();
				c_observations = c_boxplot->get_observations();
				c_outliers = c_boxplot->get_outliers();
				c_groups = c_boxplot->get_groups();
				c_names = c_boxplot->get_names();
				c_widths = c_boxplot->get_widths();

				r_tukey = REAL(tukey);
				r_observations = REAL(observations);
				r_outliers = REAL(outliers);
				r_groups = REAL(groups);
				r_widths = REAL(widths);

				for (int j = 0; j < (5 * groups_cnt); j++) {
					r_tukey[j] = c_tukey[j];
				}

				for (int j = 0; j < groups_cnt; j++) {
					r_observations[j] = c_observations[j];
					SET_STRING_ELT(names, j, mkChar(c_names[j]));
					r_widths[j] = c_widths[j];
				}

				for (int j = 0; j < outliers_cnt; j++) {
					r_outliers[j] = c_outliers[j];
					r_groups[j] = c_groups[j];
				}

				SET_VECTOR_ELT(boxplot, 0, tukey);
				SET_VECTOR_ELT(boxplot, 1, observations);
				SET_VECTOR_ELT(boxplot, 2, notches);
				SET_VECTOR_ELT(boxplot, 3, outliers);
				SET_VECTOR_ELT(boxplot, 4, groups);
				SET_VECTOR_ELT(boxplot, 5, names);

				setAttrib(boxplot, R_NamesSymbol, boxplot_names);

				SET_STRING_ELT(plot_name, 0, mkChar(c_boxplot->get_common_name()));
				SET_STRING_ELT(plot_title, 0, mkChar(c_boxplot->get_title()));

				SET_VECTOR_ELT(list, i, plot_name);
				i += 1;
				SET_VECTOR_ELT(list, i, plot_title);
				i += 1;
				SET_VECTOR_ELT(list, i, widths);
				i += 1;
				SET_VECTOR_ELT(list, i, boxplot);
				i += 1;

				UNPROTECT(10);
			}

			UNPROTECT(2);
		}

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf(" Done. (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		Rprintf("- Writing results... ");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		start_time = clock();

		analyzer.print_result_html(*files, boxplots, c_resource_path);
		/*analyzer.print_summary_txt(*files, Analyzer::SUMMARY_FILE);*/

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;

		Rprintf(" Done. (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
	} catch (AnalyzerException &e) {
		for (unsigned int i = 0; i < files->size(); i++) {
			delete files->at(i);
		}
		files->clear();
		delete files;

		if (boxplots != NULL) {
			for (unsigned int i = 0; i < boxplots->size(); i++) {
				delete boxplots->at(i);
			}
			boxplots->clear();
			delete boxplots;
		}

		error("\n%s", e.what());
	} catch (FileException &e) {
		for (unsigned int i = 0; i < files->size(); i++) {
			delete files->at(i);
		}
		files->clear();
		delete files;

		if (boxplots != NULL) {
			for (unsigned int i = 0; i < boxplots->size(); i++) {
				delete boxplots->at(i);
			}
			boxplots->clear();
			delete boxplots;
		}

		error("\n%s", e.what());
	} catch (PlotException &e) {
		for (unsigned int i = 0; i < files->size(); i++) {
			delete files->at(i);
		}
		files->clear();
		delete files;

		if (boxplots != NULL) {
			for (unsigned int i = 0; i < boxplots->size(); i++) {
				delete boxplots->at(i);
			}
			boxplots->clear();
			delete boxplots;
		}

		error("\n%s", e.what());
	}

	if (boxplots != NULL) {
		for (boxplots_it = boxplots->begin(); boxplots_it != boxplots->end(); boxplots_it++) {
			delete *boxplots_it;
		}

		boxplots->clear();
		delete boxplots;
	}

	return list;
}

/*
 *	Creates structures for common plots
 *	Arguments:
 *		external_pointer	-	an external pointer to vector<File*>
 *		file_position		-	a file position in the vector<File*> pointed by external_pointer
 *		resource_path		-	the location of GWASQC package resources (css/jscript)
 *	Returns:
 *		r_plots				-	vector with R data structures for plots
 */
SEXP QC_process_file(SEXP external_pointer, SEXP file_position, SEXP resource_path) {
	vector<File*>* files = NULL;
	File* file = NULL;

	vector<Plot*>* c_plots = NULL;
	vector<Histogram*>* c_historams = NULL;
	vector<Boxplot*>* c_boxplots = NULL;
	vector<Qqplot*>* c_qqplots = NULL;
	Plot* c_plot = NULL;
	Histogram* c_histogram = NULL;
	Boxplot* c_boxplot = NULL;
	Qqplot* c_qqplot = NULL;

	SEXP r_plots = R_NilValue;
	SEXP r_histograms = R_NilValue;
	SEXP r_boxplots = R_NilValue;
	SEXP r_qqplots = R_NilValue;
	SEXP r_plot = R_NilValue;
	SEXP r_histogram = R_NilValue;
	SEXP r_boxplot = R_NilValue;
	SEXP r_qqplot = R_NilValue;
	SEXP plot_name = R_NilValue;
	SEXP plot_title = R_NilValue;

	int plots = 0;
	int histograms = 0;
	int boxplots = 0;
	int qqplots = 0;

	const char* c_file_name = NULL;
	const char* c_resource_path = NULL;
	int c_file_position = 0;

	/* Histogram object */
	SEXP breaks = R_NilValue;
	SEXP counts = R_NilValue;
	SEXP density = R_NilValue;
	SEXP intensities = R_NilValue;
	SEXP mids = R_NilValue;
	SEXP xname = R_NilValue;
	SEXP equidist = R_NilValue;
	SEXP histogram_names = R_NilValue;
	SEXP histogram_class = R_NilValue;
	SEXP title = R_NilValue;

	const double* c_breaks = NULL;
	const int* c_counts = NULL;
	const double* c_density = NULL;
	const double* c_intensities = NULL;
	const double* c_mids = NULL;

	double* r_breaks = NULL;
	double* r_counts = NULL;
	double* r_density = NULL;
	double* r_intensities = NULL;
	double* r_mids = NULL;
	int nclass = 0;

	/* Quantiles & boxplot object */
	SEXP r_boxplot_quantile = R_NilValue;

	SEXP r_quantile = R_NilValue;
	double* r_quantiles = NULL;

	SEXP tukey = R_NilValue;
	SEXP observations = R_NilValue;
	SEXP notches = R_NilValue;
	SEXP outliers = R_NilValue;
	SEXP groups = R_NilValue;
	SEXP names = R_NilValue;
	SEXP boxplot_names = R_NilValue;

	const double* c_tukey = NULL;
	const double* c_outliers = NULL;
	const int* c_groups = NULL;

	double* r_tukey = NULL;
	double* r_outliers = NULL;
	double* r_groups = NULL;
	int outliers_cnt = 0;

	/* Qqplot object */
	SEXP name = R_NilValue;
	SEXP color = R_NilValue;
	SEXP lambda = R_NilValue;
	SEXP points = R_NilValue;
	SEXP x = R_NilValue;
	SEXP y = R_NilValue;

	const char* const* c_names = NULL;
	const char* const* c_colors = NULL;
	const double* c_lambdas = NULL;
	const int* c_points = NULL;
	const double* c_x = NULL;
	const double* c_y = NULL;

	double* r_x = NULL;
	double* r_y = NULL;

	int count = 0;
	const int* size = NULL;
	int c_j = 0;

	/* For timing */
	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	/* For memory tracking */
	int memory_used_bytes = 0;
	double memory_used_mb = 0.0;

	files = (vector<File*>*) R_ExternalPtrAddr(external_pointer);

	if ((files == NULL) || (files->empty())) {
		error("\nThere are no files in memory for processing.");
	}

	c_file_position = asInteger(file_position);

	if ((c_file_position < 1) || (c_file_position > (int)files->size())) {
		error("\nFile position in a file's vector is out of range.");
	}

	c_file_position -= 1;

	c_resource_path = CHAR(STRING_ELT(resource_path, 0));

	if ((strlen(c_resource_path) == 0) || (strspn(c_resource_path, " \t") == strlen(c_resource_path))) {
		error("\nThe path to the package resources can't be empty.");
	}

	file = files->at(c_file_position);
	c_file_name = file->get_file_name();

	Rprintf("Processing file: %s.(%d/%d)\n", c_file_name, c_file_position + 1, files->size());
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif

	try {
		Analyzer analyzer;

		Rprintf("- Calculating... ");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		start_time = clock();

		memory_used_bytes = analyzer.process_data(*file);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		memory_used_mb = memory_used_bytes / (double)(1024 * 1024);

		Rprintf(" Done. (%g sec, %g Mb)\n", execution_time, memory_used_mb);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		Rprintf("- Creating plots... ");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		c_plots = analyzer.create_plots(*file);

		plots = c_plots->size();
		if (plots > 0) {
			PROTECT(r_plots = allocVector(VECSXP, plots));

			PROTECT(histogram_names = allocVector(STRSXP, 7));
			SET_STRING_ELT(histogram_names, 0, mkChar("breaks"));
			SET_STRING_ELT(histogram_names, 1, mkChar("counts"));
			SET_STRING_ELT(histogram_names, 2, mkChar("density"));
			SET_STRING_ELT(histogram_names, 3, mkChar("intensities"));
			SET_STRING_ELT(histogram_names, 4, mkChar("mids"));
			SET_STRING_ELT(histogram_names, 5, mkChar("xname"));
			SET_STRING_ELT(histogram_names, 6, mkChar("equidist"));

			PROTECT(histogram_class = allocVector(STRSXP, 1));
			SET_STRING_ELT(histogram_class, 0, mkChar("histogram"));

			PROTECT(boxplot_names = allocVector(STRSXP, 6));
			SET_STRING_ELT(boxplot_names, 0, mkChar("stats"));
			SET_STRING_ELT(boxplot_names, 1, mkChar("n"));
			SET_STRING_ELT(boxplot_names, 2, mkChar("conf"));
			SET_STRING_ELT(boxplot_names, 3, mkChar("out"));
			SET_STRING_ELT(boxplot_names, 4, mkChar("group"));
			SET_STRING_ELT(boxplot_names, 5, mkChar("names"));

			for (int p = 0; p < plots; p++) {
				c_plot = c_plots->at(p);
				histograms = c_plot->histograms_count();
				boxplots = c_plot->boxplots_count();
				qqplots = c_plot->qqplots_count();

				PROTECT(r_plot = allocVector(VECSXP, 5));

				PROTECT(plot_title = allocVector(STRSXP, 1));
				SET_STRING_ELT(plot_title, 0, mkChar(c_plot->get_title()));
				SET_VECTOR_ELT(r_plot, 0, plot_title);
				UNPROTECT(1);

				PROTECT(plot_name = allocVector(STRSXP, 1));
				SET_STRING_ELT(plot_name, 0, mkChar(c_plot->get_name()));
				SET_VECTOR_ELT(r_plot, 1, plot_name);
				UNPROTECT(1);

				if (histograms > 0) {
					PROTECT(r_histograms = allocVector(VECSXP, histograms));
					c_historams = c_plot->get_histograms();

					for (int h = 0; h < histograms; h++) {
						PROTECT(r_histogram = allocVector(VECSXP, 8));
						c_histogram = c_historams->at(h);

						/* Create histogram object */
						nclass = c_histogram->get_nclass();

						PROTECT(breaks = allocVector(REALSXP, nclass + 1));
						PROTECT(counts = allocVector(REALSXP, nclass));
						PROTECT(density = allocVector(REALSXP, nclass));
						PROTECT(intensities = allocVector(REALSXP, nclass));
						PROTECT(mids = allocVector(REALSXP, nclass));
						PROTECT(xname = allocVector(STRSXP, 1));
						PROTECT(equidist = allocVector(LGLSXP, 1));
						PROTECT(title = allocVector(STRSXP, 1));

						c_breaks = c_histogram->get_breaks();
						c_counts = c_histogram->get_counts();
						c_density = c_histogram->get_density();
						c_intensities = c_histogram->get_intensities();
						c_mids = c_histogram->get_mids();

						r_breaks = REAL(breaks);
						r_counts = REAL(counts);
						r_density = REAL(density);
						r_intensities = REAL(intensities);
						r_mids = REAL(mids);

						for (int j = 0; j < nclass; j++) {
							r_breaks[j] = c_breaks[j];
							r_counts[j] = c_counts[j];
							r_density[j] = c_density[j];
							r_intensities[j] = c_intensities[j];
							r_mids[j] = c_mids[j];
						}

						r_breaks[nclass] = c_breaks[nclass];

						SET_STRING_ELT(xname, 0, mkChar(c_histogram->get_xname()));
						LOGICAL(equidist)[0] = 1;

						SET_STRING_ELT(title, 0, mkChar(c_histogram->get_title()));

						SET_VECTOR_ELT(r_histogram, 0, breaks);
						SET_VECTOR_ELT(r_histogram, 1, counts);
						SET_VECTOR_ELT(r_histogram, 2, density);
						SET_VECTOR_ELT(r_histogram, 3, intensities);
						SET_VECTOR_ELT(r_histogram, 4, mids);
						SET_VECTOR_ELT(r_histogram, 5, xname);
						SET_VECTOR_ELT(r_histogram, 6, equidist);
						SET_VECTOR_ELT(r_histogram, 7, title);

						setAttrib(r_histogram, R_NamesSymbol, histogram_names);
						setAttrib(r_histogram, R_ClassSymbol, histogram_class);

						SET_VECTOR_ELT(r_histograms, h, r_histogram);
						UNPROTECT(9);
					}

					SET_VECTOR_ELT(r_plot, 2, r_histograms);
					UNPROTECT(1);
				}
				else {
					SET_VECTOR_ELT(r_plot, 2, R_NilValue);
				}

				if (boxplots > 0) {
					PROTECT(r_boxplots = allocVector(VECSXP, boxplots));
					c_boxplots = c_plot->get_boxplots();

					for (int b = 0; b < boxplots; b++) {
						PROTECT(r_boxplot_quantile = allocVector(VECSXP, 2));
						PROTECT(r_boxplot = allocVector(VECSXP, 7));
						PROTECT(r_quantile = allocVector(REALSXP, 5));
						c_boxplot = c_boxplots->at(b);

						/* Create boxplot object */
						outliers_cnt = c_boxplot->get_outliers_cnt();

						PROTECT(tukey = allocMatrix(REALSXP, Boxplot::TUKEY_SIZE, 1));
						PROTECT(observations = allocVector(REALSXP, 1));
						PROTECT(notches = allocVector(REALSXP, 0));
						PROTECT(outliers = allocVector(REALSXP, outliers_cnt));
						PROTECT(groups = allocVector(REALSXP, outliers_cnt));
						PROTECT(names = allocVector(STRSXP, 1));
						PROTECT(title = allocVector(STRSXP, 1));

						c_tukey = c_boxplot->get_tukey();
						c_outliers = c_boxplot->get_outliers();
						c_groups = c_boxplot->get_groups();

						r_quantiles = REAL(r_quantile);
						r_tukey = REAL(tukey);
						r_outliers = REAL(outliers);
						r_groups = REAL(groups);

						r_quantiles[0] = c_boxplot->get_quantile(0);
						r_quantiles[1] = c_boxplot->get_quantile(25);
						r_quantiles[2] = c_boxplot->get_quantile(50);
						r_quantiles[3] = c_boxplot->get_quantile(75);
						r_quantiles[4] = c_boxplot->get_quantile(100);

						for (int j = 0; j < Boxplot::TUKEY_SIZE; j++) {
							r_tukey[j] = c_tukey[j];
						}

						REAL(observations)[0] = c_boxplot->get_observations()[0];

						for (int j = 0; j < outliers_cnt; j++) {
							r_outliers[j] = c_outliers[j];
							r_groups[j] = c_groups[j];
						}

						SET_STRING_ELT(names, 0, mkChar(c_boxplot->get_names()[0]));
						SET_STRING_ELT(title, 0, mkChar(c_boxplot->get_title()));

						SET_VECTOR_ELT(r_boxplot, 0, tukey);
						SET_VECTOR_ELT(r_boxplot, 1, observations);
						SET_VECTOR_ELT(r_boxplot, 2, notches);
						SET_VECTOR_ELT(r_boxplot, 3, outliers);
						SET_VECTOR_ELT(r_boxplot, 4, groups);
						SET_VECTOR_ELT(r_boxplot, 5, names);
						SET_VECTOR_ELT(r_boxplot, 6, title);

						setAttrib(r_boxplot, R_NamesSymbol, boxplot_names);

						SET_VECTOR_ELT(r_boxplot_quantile, 0, r_boxplot);
						SET_VECTOR_ELT(r_boxplot_quantile, 1, r_quantile);
						SET_VECTOR_ELT(r_boxplots, b, r_boxplot_quantile);
						UNPROTECT(10);
					}

					SET_VECTOR_ELT(r_plot, 3, r_boxplots);
					UNPROTECT(1);
				}
				else {
					SET_VECTOR_ELT(r_plot, 3, R_NilValue);
				}

				if (qqplots > 0) {
					PROTECT(r_qqplots = allocVector(VECSXP, qqplots));
					c_qqplots = c_plot->get_qqplots();

					for (int q = 0; q < qqplots; q++) {
						c_qqplot = c_qqplots->at(q);

						count = c_qqplot->get_count();
						size = c_qqplot->get_size();

						c_names = c_qqplot->get_names();
						c_colors = c_qqplot->get_colors();
						c_lambdas = c_qqplot->get_lambdas();
						c_points = c_qqplot->get_points();
						c_x = c_qqplot->get_x();
						c_y = c_qqplot->get_y();
						c_j = 0;

						PROTECT(r_qqplot = allocVector(VECSXP, count * 6));

						for (int k = 0; k < count; k++) {
							PROTECT(name = allocVector(STRSXP, 1));
							PROTECT(color = allocVector(STRSXP, 1));
							PROTECT(lambda = allocVector(REALSXP, 1));
							PROTECT(points = allocVector(INTSXP, 1));
							PROTECT(x = allocVector(REALSXP, size[k]));
							PROTECT(y = allocVector(REALSXP, size[k]));

							r_x = REAL(x);
							r_y = REAL(y);

							for (int j = 0; j < size[k]; j++) {
								r_x[j] = c_x[c_j];
								r_y[j] = c_y[c_j];
								c_j += 1;
							}

							SET_STRING_ELT(name, 0, mkChar(c_names[k]));
							SET_STRING_ELT(color, 0, mkChar(c_colors[k]));
							REAL(lambda)[0] = c_lambdas[k];
							INTEGER(points)[0] = c_points[k];

							SET_VECTOR_ELT(r_qqplot, k * 6, name);
							SET_VECTOR_ELT(r_qqplot, k * 6 + 1, color);
							SET_VECTOR_ELT(r_qqplot, k * 6 + 2, lambda);
							SET_VECTOR_ELT(r_qqplot, k * 6 + 3, points);
							SET_VECTOR_ELT(r_qqplot, k * 6 + 4, x);
							SET_VECTOR_ELT(r_qqplot, k * 6 + 5, y);

							UNPROTECT(6);
						}

						SET_VECTOR_ELT(r_qqplots, q, r_qqplot);
						UNPROTECT(1);
					}

					SET_VECTOR_ELT(r_plot, 4, r_qqplots);
					UNPROTECT(1);
				}
				else {
					SET_VECTOR_ELT(r_plot, 4, R_NilValue);
				}

				SET_VECTOR_ELT(r_plots, p, r_plot);
				UNPROTECT(1);
			}

			UNPROTECT(4);
		}

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf(" Done. (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		Rprintf("- Writing results... ");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		start_time = clock();

		analyzer.print_result_txt(*file);
		analyzer.print_result_html(*file, c_plots, c_resource_path);
		analyzer.print_result_csv(*file);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;

		Rprintf(" Done. (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
	} catch (AnalyzerException &e) {
		for (unsigned int i = 0; i < files->size(); i++) {
			delete files->at(i);
		}
		files->clear();
		delete files;
		error("\n%s", e.what());
	} catch (FileException &e) {
		for (unsigned int i = 0; i < files->size(); i++) {
			delete files->at(i);
		}
		files->clear();
		delete files;
		error("\n%s", e.what());
	} catch (PlotException &e) {
		for (unsigned int i = 0; i < files->size(); i++) {
			delete files->at(i);
		}
		files->clear();
		delete files;
		error("\n%s", e.what());
	} catch (MetaException &e) {
		for (unsigned int i = 0; i < files->size(); i++) {
			delete files->at(i);
		}
		files->clear();
		delete files;
		error("\n%s", e.what());
	}

	return r_plots;
}

/*
 *	Comparison between studies
 *	Arguments:
 *		scrip_file		-	name of an input script
 *		pval_threshold	-	a threshold for the P-value
 *		maf_threshold	-	a threshold for the allele frequency
 *	Returns:
 *		R_NilValue
 */
SEXP CMP_compare(SEXP script_file, SEXP pval_thershold, SEXP maf_threshold) {
	const char* c_script_file = NULL;
	double c_pval_threshold = 0;
	double c_maf_threshold = 0;

	vector<File*>* files = NULL;
	vector<File*>::iterator files_it;

	map<char*, Data*, bool(*)(const char*, const char*)>* snps = NULL;
	map<char*, Data*, bool(*)(const char*, const char*)>::iterator snps_it;

	double lambda = 0.0;
	int position = 0;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	c_script_file = CHAR(STRING_ELT(script_file, 0));
	if ((strlen(c_script_file) == 0) || (strspn(c_script_file, " \t") == strlen(c_script_file))) {
		error("The name of an input script file can't be empty.");
	}

	c_pval_threshold = asReal(pval_thershold);
	if (c_pval_threshold == NA_REAL) {
		error("The P-value threshold must be a value of type double.");
	}

	c_maf_threshold = asReal(maf_threshold);
	if (c_pval_threshold == NA_REAL) {
		error("The MAF threshold must be a value of type double.");
	}

	try {
		Comparator comparator;

		Rprintf("Processing '%s' file\n", c_script_file);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		/*** START: Reading the input script ***/
		Rprintf("  Reading the script... ");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		files = comparator.process_script(c_script_file);
		if ((files == NULL) ||  (files->size() <= 0)) {
			error("The input script does not contain any file for processing.");
		}

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("Done (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		/*** END: Reading the input script ***/

		/*** START: Calculating lambdas ***/
		Rprintf("  Calculating lambdas\n");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		files_it = files->begin();
		while (files_it != files->end()) {
			Rprintf("    processing %s... ", (*files_it)->get_abbreviation());
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif

			lambda = comparator.get_lambda(*files_it);
			(*files_it)->set_lambda(lambda);
			files_it++;

			Rprintf("Done (lambda = %g)\n", lambda);
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif
		}

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("  Done (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		/*** END: Calculating lambdas ***/

		/*** START: Filtering SNPs ***/
		Rprintf("  Filtering SNPs... ");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		snps = comparator.get_filtered_snps(files->at(0), files->at(0)->get_lambda(), c_pval_threshold, c_maf_threshold);
		comparator.print_all_columns(files->at(0), Comparator::FILTERED_SNPS, snps);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("Done (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		/*** END: Filtering SNPs ***/

		/*** START: Writing corrected P-values ***/
		Rprintf("  Retrieving corrected %s\n", Comparator::PVALUE);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		files_it = files->begin();
		files_it++;
		while (files_it != files->end()) {
			Rprintf("    processing %s... ", (*files_it)->get_abbreviation());
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif

			comparator.add_corrected_p_values(*files_it, (*files_it)->get_lambda(), snps);
			files_it++;

			Rprintf("Done\n");
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif
		}
		comparator.print_single_column(files, Comparator::CORRECTED_PVALUE, false, snps);
		comparator.clear_values(snps);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("  Done (%g sec)\n", execution_time);
		R_FlushConsole();
	#ifndef LINUX
		R_ProcessEvents();
	#endif
		/*** END: Writing corrected P-values ***/

		/*** START: Writing corrected StdErr ***/
		Rprintf("  Retrieving corrected %s\n", Comparator::STDERR);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		files_it = files->begin();
		while (files_it != files->end()) {
			Rprintf("    processing %s... ", (*files_it)->get_abbreviation());
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif

			comparator.add_corrected_stderr(*files_it, (*files_it)->get_lambda(), snps);
			files_it++;

			Rprintf("Done\n");
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif
		}
		comparator.print_single_column(files, comparator.CORRECTED_STDERR, false, snps);
		comparator.clear_values(snps);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("  Done (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		/*** END: Writing corrected StdErr ***/

		/*** START: Writing EFFECT ***/
		Rprintf("  Retrieving %s\n", Comparator::EFFECT);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		Rprintf("    processing %s... ", files->at(0)->get_abbreviation());
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		comparator.add_values(files->at(0), Comparator::EFFECT, snps);

		Rprintf("Done\n");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		files_it = files->begin();
		files_it++;
		position = 0;
		while (files_it != files->end()) {
			Rprintf("    processing %s... ", (*files_it)->get_abbreviation());
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif

			comparator.add_effect_values(*files_it, position, snps);
			files_it++;
			position += 1;

			Rprintf("Done\n");
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif
		}
		comparator.print_single_column(files, Comparator::EFFECT, false, snps);
		comparator.clear_values(snps);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("  Done (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		/*** END: Writing EFFECT ***/

		/*** START: Writing FREQLABEL ***/
		Rprintf("  Retrieving %s\n", Comparator::FREQLABEL);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		Rprintf("    processing %s... ", files->at(0)->get_abbreviation());
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		comparator.add_values(files->at(0), Comparator::FREQLABEL, snps);

		Rprintf("Done\n");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

		files_it = files->begin();
		files_it++;
		position = 0;
		while (files_it != files->end()) {
			Rprintf("    processing %s... ", (*files_it)->get_abbreviation());
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif

			comparator.add_freq_values(*files_it, position, snps);
			files_it++;
			position += 1;

			Rprintf("Done\n");
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif
		}
		comparator.print_single_column(files, Comparator::FREQLABEL, false, snps);
		comparator.clear_values(snps);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("  Done (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		/*** END: Writing FREQLABEL ***/

		/*** START: Writing IMP_QUALITY ***/
		Rprintf("  Retrieving %s\n", Comparator::OEVAR_IMP);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		start_time = clock();

		files_it = files->begin();
		files_it++;
		while (files_it != files->end()) {
			Rprintf("    processing %s... ", (*files_it)->get_abbreviation());
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif

			comparator.add_values(*files_it, Comparator::OEVAR_IMP, snps);
			files_it++;

			Rprintf("Done\n");
			R_FlushConsole();
	#ifndef LINUX
			R_ProcessEvents();
	#endif
		}
		comparator.print_single_column(files, Comparator::OEVAR_IMP, true, snps);
		comparator.clear_values(snps);

		end_time = clock();
		execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
		Rprintf("  Done (%g sec)\n", execution_time);
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif
		/*** END: Writing IMP_QUALITY ***/

		/*** START: Writing others ***/
		for (int i = 0; i < Comparator::COLUMNS_SIZE; i++) {
			Rprintf("  Retrieving %s\n", Comparator::columns[i]);
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif
			start_time = clock();

			files_it = files->begin();
			while (files_it != files->end()) {
				Rprintf("    processing %s... ", (*files_it)->get_abbreviation());
				R_FlushConsole();
#ifndef LINUX
				R_ProcessEvents();
#endif

				comparator.add_values(*files_it, Comparator::columns[i], snps);
				files_it++;

				Rprintf("Done\n");
				R_FlushConsole();
#ifndef LINUX
				R_ProcessEvents();
#endif
			}

			comparator.print_single_column(files, Comparator::columns[i], false, snps);
			comparator.clear_values(snps);

			end_time = clock();
			execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
			Rprintf("  Done (%g sec)\n", execution_time);
			R_FlushConsole();
#ifndef LINUX
			R_ProcessEvents();
#endif
		}
		/*** END: End others ***/

		Rprintf("Done\n");
		R_FlushConsole();
#ifndef LINUX
		R_ProcessEvents();
#endif

	}
	catch (ComparatorException &e) {
		if ((snps != NULL) && (snps->size() > 0)) {
			for (snps_it = snps->begin(); snps_it != snps->end(); snps_it++) {
				free(snps_it->first);
				delete snps_it->second;
			}
			snps->clear();
		}
		error("\n%s", e.what());
	}
	catch (FileException &e) {
		if ((snps != NULL) && (snps->size() > 0)) {
			for (snps_it = snps->begin(); snps_it != snps->end(); snps_it++) {
				free(snps_it->first);
				delete snps_it->second;
			}
			snps->clear();
		}
		error("\n%s", e.what());
	}

	return R_NilValue;
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
SEXP ANOT_snp_to_gene(SEXP genes_file, SEXP snps_file, SEXP output_file, SEXP append, SEXP delimiter, SEXP deviations) {
	const char* c_genes_file = NULL;
	const char* c_snps_file = NULL;
	const char* c_output_file = NULL;
	int c_append = 0;
	bool b_append = false;
	const char* c_delimiter = NULL;
	int* c_deviations = NULL;
	int* r_deviations = NULL;
	int deviations_size = 0;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	c_genes_file = CHAR(STRING_ELT(genes_file, 0));
	if ((strlen(c_genes_file) == 0) || (strspn(c_genes_file, " \t") == strlen(c_genes_file))) {
		error("The name of an input file with genes can't be empty.");
	}

	c_snps_file = CHAR(STRING_ELT(snps_file, 0));
	if ((strlen(c_snps_file) == 0) || (strspn(c_snps_file, " \t") == strlen(c_snps_file))) {
		error("The name of an input file with SNPs can't be empty.");
	}

	c_output_file = CHAR(STRING_ELT(output_file, 0));
	if ((strlen(c_output_file)== 0) || (strspn(c_output_file, " \t") == strlen(c_output_file))) {
		error("The name of an output file can't be empty.");
	}

	c_delimiter = CHAR(STRING_ELT(delimiter, 0));
	if (strlen(c_delimiter) ==  0) {
		error("The delimiter for the names of genes can't be empty");
	}

	if (!isInteger(deviations)) {
		error("Deviations are expected to be stored in a vector of integers.");
	}

	deviations_size = length(deviations);
	if (deviations_size == 0) {
		error("Vector of deviations is expected to be of non-zero length.");
	}

	r_deviations = INTEGER(deviations);

	if ((c_deviations = (int*)malloc(deviations_size * sizeof(int))) == NULL) {
		error("Error while allocating memory block.");
	}

	for (int i = 0; i < deviations_size; i++) {
		c_deviations[i] = r_deviations[i];
	}

	c_append = asLogical(append);

	if (c_append == NA_LOGICAL) {
		error("The append argument is expected to be FALSE/TRUE.");
	}

	if (c_append == TRUE) {
		b_append = true;
	}

	Rprintf("Processing... ");
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif
	start_time = clock();

	try {
		Annotation annotation;
		annotation.snp_to_gene(c_genes_file, c_snps_file, c_output_file, b_append, c_delimiter, c_deviations, deviations_size);
	}
	catch (AnnotationException &e) {
		error("\n%s", e.what());
	}

	end_time = clock();
	execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
	Rprintf("Done (%g sec)\n", execution_time);
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif

	free(c_deviations);
	return R_NilValue;
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
SEXP ANOT_k_nearest_genes(SEXP genes_file, SEXP snps_file, SEXP output_file, SEXP append, SEXP multicolumn, SEXP k_max) {
	const char* c_genes_file = NULL;
	const char* c_snps_file = NULL;
	const char* c_output_file = NULL;
	int c_append = 0;
	bool b_append = false;
	int c_multicolumn = 0;
	bool b_multicolumn = false;
	int c_k_max = 0;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	c_genes_file = CHAR(STRING_ELT(genes_file, 0));
	if ((strlen(c_genes_file) == 0) || (strspn(c_genes_file, " \t") == strlen(c_genes_file))) {
		error("The name of an input file with genes can't be empty.");
	}

	c_snps_file = CHAR(STRING_ELT(snps_file, 0));
	if ((strlen(c_snps_file) == 0) || (strspn(c_snps_file, " \t") == strlen(c_snps_file))) {
		error("The name of an input file with SNPs can't be empty.");
	}

	c_output_file = CHAR(STRING_ELT(output_file, 0));
	if ((strlen(c_output_file)== 0) || (strspn(c_output_file, " \t") == strlen(c_output_file))) {
		error("The name of an output file can't be empty.");
	}

	c_multicolumn = asLogical(multicolumn);

	if (c_multicolumn == NA_LOGICAL) {
		error("The multicoumn argument is expected to be FALSE/TRUE.");
	}

	if (c_multicolumn == TRUE) {
		b_multicolumn = true;
	}

	c_k_max = INTEGER(k_max)[0];
	if (c_k_max <= 0) {
		error("The number of nearest genes is expected to be greater than 0.");
	}

	c_append = asLogical(append);

	if (c_append == NA_LOGICAL) {
		error("The append argument is expected to be FALSE/TRUE.");
	}

	if (c_append == TRUE) {
		b_append = true;
	}

	Rprintf("Processing... ");
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif
	start_time = clock();

	try {
		Annotation annotation;
		annotation.k_nearest_genes(c_genes_file, c_snps_file, c_output_file, b_append, b_multicolumn, c_k_max);
	}
	catch (AnnotationException &e) {
		error("\n%s", e.what());
	}

	end_time = clock();
	execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
	Rprintf("Done (%g sec)\n", execution_time);
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif

	return R_NilValue;
}

SEXP ANOT_snp_to_gene_hs(SEXP chromosome, SEXP snps_file, SEXP ld_file, SEXP hotspots_file, SEXP genes_file, SEXP prefix) {
	const char* c_chromosome = NULL;
	const char* c_snps_file = NULL;
	const char* c_ld_file = NULL;
	const char* c_hotspots_file = NULL;
	const char* c_genes_file = NULL;
	const char* c_prefix = NULL;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	c_chromosome = CHAR(STRING_ELT(chromosome, 0));
	if ((strlen(c_chromosome) == 0) || (strspn(c_chromosome, " \t") == strlen(c_chromosome))) {
		error("The chromosome name can't be empty.");
	}

	c_snps_file = CHAR(STRING_ELT(snps_file, 0));
	if ((strlen(c_snps_file) == 0) || (strspn(c_snps_file, " \t") == strlen(c_snps_file))) {
		error("The name of an input file with SNPs can't be empty.");
	}

	c_ld_file = CHAR(STRING_ELT(ld_file, 0));
	if ((strlen(c_ld_file) == 0) || (strspn(c_ld_file, " \t") == strlen(c_ld_file))) {
		error("The name of an input file with LD values can't be empty.");
	}

	c_hotspots_file = CHAR(STRING_ELT(hotspots_file, 0));
	if ((strlen(c_hotspots_file) == 0) || (strspn(c_hotspots_file, " \t") == strlen(c_hotspots_file))) {
		error("The name of an input file with hotspots can't be empty.");
	}

	c_genes_file = CHAR(STRING_ELT(genes_file, 0));
	if ((strlen(c_genes_file) == 0) || (strspn(c_genes_file, " \t") == strlen(c_genes_file))) {
		error("The name of an input file with genes can't be empty.");
	}

	c_prefix = CHAR(STRING_ELT(prefix, 0));
	if ((strlen(c_prefix)== 0) || (strspn(c_prefix, " \t") == strlen(c_prefix))) {
		error("The prefix of output files can't be empty.");
	}

	Rprintf("Processing... ");
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif
	start_time = clock();

	try {
		Annotation annotation;
		annotation.snp_to_gene_hs(c_chromosome, c_snps_file, c_ld_file, c_hotspots_file, c_genes_file, c_prefix);
	}
	catch (AnnotationException &e) {
		error("\n%s", e.what());
	}

	end_time = clock();
	execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
	Rprintf("Done (%g sec)\n", execution_time);
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif

	return R_NilValue;
}

SEXP ANOT_gene_to_snp(SEXP genes_file, SEXP snps_file, SEXP output_file, SEXP append, SEXP multicolumn) {
	const char* c_genes_file = NULL;
	const char* c_snps_file = NULL;
	const char* c_output_file = NULL;
	int c_append = 0;
	bool b_append = false;
	int c_multicolumn = 0;
	bool b_multicolumn = false;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	c_genes_file = CHAR(STRING_ELT(genes_file, 0));
	if ((strlen(c_genes_file) == 0) || (strspn(c_genes_file, " \t") == strlen(c_genes_file))) {
		error("The name of an input file with genes can't be empty.");
	}

	c_snps_file = CHAR(STRING_ELT(snps_file, 0));
	if ((strlen(c_snps_file) == 0) || (strspn(c_snps_file, " \t") == strlen(c_snps_file))) {
		error("The name of an input file with SNPs can't be empty.");
	}

	c_output_file = CHAR(STRING_ELT(output_file, 0));
	if ((strlen(c_output_file)== 0) || (strspn(c_output_file, " \t") == strlen(c_output_file))) {
		error("The name of an output file can't be empty.");
	}

	c_multicolumn = asLogical(multicolumn);

	if (c_multicolumn == NA_LOGICAL) {
		error("The multicoumn argument is expected to be FALSE/TRUE.");
	}

	if (c_multicolumn == TRUE) {
		b_multicolumn = true;
	}

	c_append = asLogical(append);

	if (c_append == NA_LOGICAL) {
		error("The append argument is expected to be FALSE/TRUE.");
	}

	if (c_append == TRUE) {
		b_append = true;
	}

	Rprintf("Processing... ");
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif
	start_time = clock();

	try {
		Annotation annotation;
		annotation.gene_to_snp(c_genes_file, c_snps_file, c_output_file, b_append, b_multicolumn);
	}
	catch (AnnotationException &e) {
		error("\n%s", e.what());
	}

	end_time = clock();
	execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
	Rprintf("Done (%g sec)\n", execution_time);
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif

	return R_NilValue;
}

SEXP ANOT_gene_to_snp_by_name(SEXP genes_file, SEXP genesdb_file, SEXP snps_file, SEXP output_file, SEXP append, SEXP multicolumn) {
	const char* c_genes_file = NULL;
	const char* c_genesdb_file = NULL;
	const char* c_snps_file = NULL;
	const char* c_output_file = NULL;
	int c_append = 0;
	bool b_append = false;
	int c_multicolumn = 0;
	bool b_multicolumn = false;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;

	c_genes_file = CHAR(STRING_ELT(genes_file, 0));
	if ((strlen(c_genes_file) == 0) || (strspn(c_genes_file, " \t") == strlen(c_genes_file))) {
		error("The name of an input file with names of genes can't be empty.");
	}

	c_genesdb_file = CHAR(STRING_ELT(genesdb_file, 0));
	if ((strlen(c_genesdb_file) == 0) || (strspn(c_genesdb_file, " \t") == strlen(c_genesdb_file))) {
		error("The name of an input file with genes can't be empty.");
	}

	c_snps_file = CHAR(STRING_ELT(snps_file, 0));
	if ((strlen(c_snps_file) == 0) || (strspn(c_snps_file, " \t") == strlen(c_snps_file))) {
		error("The name of an input file with SNPs can't be empty.");
	}

	c_output_file = CHAR(STRING_ELT(output_file, 0));
	if ((strlen(c_output_file)== 0) || (strspn(c_output_file, " \t") == strlen(c_output_file))) {
		error("The name of an output file can't be empty.");
	}

	c_multicolumn = asLogical(multicolumn);

	if (c_multicolumn == NA_LOGICAL) {
		error("The multicoumn argument is expected to be FALSE/TRUE.");
	}

	if (c_multicolumn == TRUE) {
		b_multicolumn = true;
	}

	c_append = asLogical(append);

	if (c_append == NA_LOGICAL) {
		error("The append argument is expected to be FALSE/TRUE.");
	}

	if (c_append == TRUE) {
		b_append = true;
	}

	Rprintf("Processing... ");
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif
	start_time = clock();

	try {
		Annotation annotation;
		annotation.gene_to_snp_by_name(c_genes_file, c_genesdb_file, c_snps_file, c_output_file, b_append, b_multicolumn);
	}
	catch (AnnotationException &e) {
		error("\n%s", e.what());
	}

	end_time = clock();
	execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
	Rprintf("Done (%g sec)\n", execution_time);
	R_FlushConsole();
#ifndef LINUX
	R_ProcessEvents();
#endif

	return R_NilValue;
}

}

/*
 *	Description:
 *		Entry point for a stand-alone application.
 *		During compilation needs to be linked to R and GSL libraries.
 *	Arguments:
 *		argv[1]	--	METAL-like script file name
 */

int main(int args, char** argv) {
	vector<File*>* files = NULL;
	vector<File*>::iterator file_it;
	vector<Boxplot*>* boxplots = NULL;
	vector<Boxplot*>::iterator boxplots_it;

	clock_t start_time = 0;
	clock_t end_time = 0;
	double execution_time = 0.0;
	int memory_used_b = 0;
	double memory_used_mb = 0.0;

	vector<Plot*>* plots = NULL;

	char fileSep = '\\';
	const char* resLocation = "inst\\extdata\\";

	try {
		Analyzer analyzer;

		if (((files = analyzer.process_script(argv[1], fileSep)) == NULL) || (files->empty())) {
			cout << "No files to process" << endl;
			return 0;
		}

		for (file_it = files->begin(); file_it != files->end(); file_it++) {
			cout << "Processing file: " << (*file_it)->get_file_name() << " (" << ((*file_it)->get_file_path() == NULL ? "no path" : (*file_it)->get_file_path()) << ")" << endl;

			start_time = clock();

			memory_used_b = analyzer.process_data(**file_it);
			plots = analyzer.create_plots(**file_it);
			analyzer.print_result_txt(**file_it);
			analyzer.print_result_csv(**file_it);
			analyzer.print_result_html(**file_it, plots, resLocation);

			end_time = clock();
			execution_time = (end_time - start_time)/(double)CLOCKS_PER_SEC;
			memory_used_mb = memory_used_b / (double)(1024 * 1024);
			cout << "Done. (" << execution_time << " sec, " << memory_used_mb << " Mb)" << endl;
		}

		cout << "Creating common boxplots... " << endl;
		boxplots = analyzer.create_common_boxplots(*files);
 		cout << "Done." << endl;

 		cout << "Creating main report... " << endl;
		analyzer.print_result_html(*files, boxplots, resLocation);
		/*analyzer.print_summary_txt(*files, Analyzer::SUMMARY_FILE);*/
		cout << "Done." << endl;

		for (file_it = files->begin(); file_it != files->end(); file_it++) {
			delete *file_it;
		}

		for (boxplots_it = boxplots->begin(); boxplots_it != boxplots->end(); boxplots_it++) {
			delete *boxplots_it;
		}

		files->clear();
		boxplots->clear();
		delete files;
		delete boxplots;
	}
	catch (AnalyzerException &e) {
		cout << "Exception in the Analyzer" << endl;
		cout << e.what() << endl;
	}
	catch (MetaException &e) {
		cout << "Exception in the Meta object" << endl;
		cout << e.what() << endl;
	}
	catch (FileException &e) {
		cout << "Exception in the File object" << endl;
		cout << e.what() << endl;
	}
	catch (PlotException &e) {
		cout << "Exception in the Plot object" << endl;
		cout << e.what() << endl;
	}

	return 0;
}

