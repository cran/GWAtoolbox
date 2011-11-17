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
#include <cstring>

#include "exception/include/Exception.h"
#include "descriptor/include/Descriptor.h"
#include "gwafile/include/GwaFile.h"
#include "gwasqc/include/analyzer/Analyzer.h"
#include "gwasformat/include/formatter/Formatter.h"

/* Define LINUX flag for compilation under Linux */
#ifndef WIN32
#define LINUX
#else
#undef LINUX
#endif

using namespace std;

extern "C" {

SEXP process_script(SEXP script_name, SEXP path_separator) {
	const char* c_script_name = NULL;
	const char* c_path_separator = NULL;

	vector<Descriptor*>* descriptors = NULL;

	SEXP descriptors_list = R_NilValue;
	SEXP external_descriptor_pointer = R_NilValue;

	if (script_name == R_NilValue) {
		error("\nThe input script name is NULL.");
	}

	if (!isString(script_name)) {
		error("\nThe input script name is not a string.");
	}

	if (length(script_name) <= 0) {
		error("\nThe input script name is empty.");
	}

	if (length(script_name) > 1) {
		error("\nThe input script name has multiple values.");
	}

	if (path_separator == R_NilValue) {
		error("\nThe path separator is NULL.");
	}

	if (!isString(path_separator)) {
		error("\nThe path separator is not a string.");
	}

	if (length(path_separator) <= 0) {
		error("\nThe path separator is empty.");
	}

	if (length(path_separator) > 1) {
		error("\nThe path separator has multiple values.");
	}

	c_script_name = CHAR(STRING_ELT(script_name, 0));
	if ((strlen(c_script_name) <= 0) || (strspn(c_script_name, " \t") == strlen(c_script_name))) {
		error("\nThe input script name is blank.");
	}

	c_path_separator = CHAR(STRING_ELT(path_separator, 0));
	if (strlen(c_path_separator) <= 0) {
		error("\nThe path separator is blank.");
	}

	if (strlen(c_path_separator) > 1) {
		error("\nThe path separator contains multiple characters.");
	}

	try {
		descriptors = Descriptor::process_instructions(c_script_name, c_path_separator[0]);
	} catch (Exception &e) {
		error("\n%s", e.what());
	}

	if (descriptors->empty()) {
		error("\nThe '%s' input script does not contain input file names.", c_script_name);
	}

	PROTECT(descriptors_list = allocVector(VECSXP, descriptors->size()));

	for (unsigned int i = 0; i < descriptors->size(); i++) {
		external_descriptor_pointer = R_MakeExternalPtr((void*)descriptors->at(i), R_NilValue, R_NilValue);
		SET_VECTOR_ELT(descriptors_list, i, external_descriptor_pointer);
	}

	UNPROTECT(1);

	return descriptors_list;
}

SEXP Descriptor2Robj(SEXP external_descriptor_pointer) {
	Descriptor* descriptor = NULL;
	char buffer[] = {'\0', '\0'};
	int i = 0;
	int j = 0;

	SEXP attributes = R_NilValue;
	SEXP class_name = R_NilValue;
	SEXP descriptor_robj = R_NilValue;

	SEXP path_separator = R_NilValue;
	SEXP name = R_NilValue;
	SEXP path = R_NilValue;
	SEXP full_path = R_NilValue;
	SEXP abbreviation = R_NilValue;
	SEXP columns = R_NilValue;
	SEXP properties = R_NilValue;
	SEXP thresholds = R_NilValue;
	SEXP threshold_names = R_NilValue;
	SEXP threshold_values = R_NilValue;
	SEXP threshold_value = R_NilValue;
	SEXP renamed_columns = R_NilValue;
	SEXP reordered_columns = R_NilValue;

	if (external_descriptor_pointer == R_NilValue) {
		error("\nThe external Descriptor pointer argument is NULL.");
	}

	if (TYPEOF(external_descriptor_pointer) != EXTPTRSXP) {
		error("\nThe external Descriptor pointer argument has incorrect type.");
	}

	descriptor = (Descriptor*)R_ExternalPtrAddr(external_descriptor_pointer);

	PROTECT(attributes = allocVector(STRSXP, 10));
	SET_STRING_ELT(attributes, 0, mkChar("path_separator"));
	SET_STRING_ELT(attributes, 1, mkChar("name"));
	SET_STRING_ELT(attributes, 2, mkChar("path"));
	SET_STRING_ELT(attributes, 3, mkChar("full_path"));
	SET_STRING_ELT(attributes, 4, mkChar("abbreviation"));
	SET_STRING_ELT(attributes, 5, mkChar("columns"));
	SET_STRING_ELT(attributes, 6, mkChar("properties"));
	SET_STRING_ELT(attributes, 7, mkChar("thresholds"));
	SET_STRING_ELT(attributes, 8, mkChar("renamed_columns"));
	SET_STRING_ELT(attributes, 9, mkChar("reordered_columns"));

	PROTECT(class_name = allocVector(STRSXP, 1));
	SET_STRING_ELT(class_name, 0, mkChar("Descriptor"));

	PROTECT(descriptor_robj = allocVector(VECSXP, 10));

	PROTECT(path_separator = allocVector(STRSXP, 1));
	buffer[0] = descriptor->get_path_separator();
	SET_STRING_ELT(path_separator, 0, mkChar(buffer));
	UNPROTECT(1);

	if (descriptor->get_name() != NULL) {
		PROTECT(name = allocVector(STRSXP, 1));
		SET_STRING_ELT(name, 0, mkChar(descriptor->get_name()));
		UNPROTECT(1);
	}

	if (descriptor->get_path() != NULL) {
		PROTECT(path = allocVector(STRSXP, 1));
		SET_STRING_ELT(path, 0, mkChar(descriptor->get_path()));
		UNPROTECT(1);
	}

	if (descriptor->get_full_path() != NULL) {
		PROTECT(full_path = allocVector(STRSXP, 1));
		SET_STRING_ELT(full_path, 0, mkChar(descriptor->get_full_path()));
		UNPROTECT(1);
	}

	if (descriptor->get_abbreviation() != NULL) {
		PROTECT(abbreviation = allocVector(STRSXP, 1));
		SET_STRING_ELT(abbreviation, 0, mkChar(descriptor->get_abbreviation()));
		UNPROTECT(1);
	}

	if (descriptor->columns.size() > 0) {
		PROTECT(columns = allocMatrix(STRSXP, descriptor->columns.size(), 2));

		descriptor->map_char_it = descriptor->columns.begin();
		i = 0;
		while (descriptor->map_char_it != descriptor->columns.end()) {
			SET_STRING_ELT(columns, i, mkChar(descriptor->map_char_it->first));
			SET_STRING_ELT(columns, i + descriptor->columns.size(), mkChar(descriptor->map_char_it->second));
			i += 1;
			descriptor->map_char_it++;
		}

		UNPROTECT(1);
	}

	if (descriptor->properties.size() > 0) {
		PROTECT(properties = allocMatrix(STRSXP, descriptor->properties.size(), 2));

		descriptor->map_char_it = descriptor->properties.begin();
		i = 0;
		while (descriptor->map_char_it != descriptor->properties.end()) {
			SET_STRING_ELT(properties, i, mkChar(descriptor->map_char_it->first));
			SET_STRING_ELT(properties, i + descriptor->properties.size(), mkChar(descriptor->map_char_it->second));
			i += 1;
			descriptor->map_char_it++;
		}

		UNPROTECT(1);
	}

	if (descriptor->thresholds.size() > 0) {
		PROTECT(threshold_names = allocVector(STRSXP, descriptor->thresholds.size()));
		PROTECT(threshold_values = allocVector(VECSXP, descriptor->thresholds.size()));

		descriptor->map_vect_it = descriptor->thresholds.begin();
		i = 0;
		while (descriptor->map_vect_it != descriptor->thresholds.end()) {
			PROTECT(threshold_value = allocVector(REALSXP, descriptor->map_vect_it->second->size()));

			descriptor->vector_double_it = descriptor->map_vect_it->second->begin();
			j = 0;
			while (descriptor->vector_double_it != descriptor->map_vect_it->second->end()) {
				REAL(threshold_value)[j] = *(descriptor->vector_double_it);
				j += 1;
				descriptor->vector_double_it++;
			}

			UNPROTECT(1);

			SET_STRING_ELT(threshold_names, i, mkChar(descriptor->map_vect_it->first));
			SET_VECTOR_ELT(threshold_values, i, threshold_value);

			i += 1;
			descriptor->map_vect_it++;
		}

		PROTECT(thresholds = allocVector(VECSXP, 2));

		SET_VECTOR_ELT(thresholds, 0, threshold_names);
		SET_VECTOR_ELT(thresholds, 1, threshold_values);

		UNPROTECT(3);
	}

	if (descriptor->renamed_columns.size() > 0) {
		PROTECT(renamed_columns = allocMatrix(STRSXP, descriptor->renamed_columns.size(), 2));

		descriptor->map_char_it = descriptor->renamed_columns.begin();
		i = 0;
		while (descriptor->map_char_it != descriptor->renamed_columns.end()) {
			SET_STRING_ELT(renamed_columns, i, mkChar(descriptor->map_char_it->first));
			SET_STRING_ELT(renamed_columns, i + descriptor->renamed_columns.size(), mkChar(descriptor->map_char_it->second));
			i += 1;
			descriptor->map_char_it++;
		}

		UNPROTECT(1);
	}

	if (descriptor->reordered_columns.size() > 0) {
		PROTECT(reordered_columns = allocVector(STRSXP, descriptor->reordered_columns.size()));

		descriptor->vector_char_it = descriptor->reordered_columns.begin();
		i = 0;
		while (descriptor->vector_char_it != descriptor->reordered_columns.end()) {
			SET_STRING_ELT(reordered_columns, i, mkChar(*(descriptor->vector_char_it)));
			i += 1;
			descriptor->vector_char_it++;
		}

		UNPROTECT(1);
	}

	SET_VECTOR_ELT(descriptor_robj, 0, path_separator);
	SET_VECTOR_ELT(descriptor_robj, 1, name);
	SET_VECTOR_ELT(descriptor_robj, 2, path);
	SET_VECTOR_ELT(descriptor_robj, 3, full_path);
	SET_VECTOR_ELT(descriptor_robj, 4, abbreviation);
	SET_VECTOR_ELT(descriptor_robj, 5, columns);
	SET_VECTOR_ELT(descriptor_robj, 6, properties);
	SET_VECTOR_ELT(descriptor_robj, 7, thresholds);
	SET_VECTOR_ELT(descriptor_robj, 8, renamed_columns);
	SET_VECTOR_ELT(descriptor_robj, 9, reordered_columns);

	setAttrib(descriptor_robj, R_NamesSymbol, attributes);
	setAttrib(descriptor_robj, R_ClassSymbol, class_name);

	UNPROTECT(3);

	return descriptor_robj;
}

SEXP delete_descriptor(SEXP external_descriptor_pointer) {
	Descriptor* descriptor = NULL;

	if (external_descriptor_pointer == R_NilValue) {
		error("\nThe external Descriptor pointer argument is NULL.");
	}

	if (TYPEOF(external_descriptor_pointer) != EXTPTRSXP) {
		error("\nThe external Descriptor pointer argument has an incorrect type.");
	}

	descriptor = (Descriptor*)R_ExternalPtrAddr(external_descriptor_pointer);

	delete descriptor;
	descriptor = NULL;

	return R_NilValue;
}

SEXP Robj2Descriptor(SEXP descriptor_Robj) {
	SEXP class_name = R_NilValue;
	SEXP attributes = R_NilValue;
	const char* value = NULL;

	SEXP path_separator = R_NilValue;
	SEXP full_path = R_NilValue;
	SEXP abbreviation = R_NilValue;
	SEXP columns = R_NilValue;
	SEXP properties = R_NilValue;
	SEXP thresholds = R_NilValue;
	SEXP threshold_names = R_NilValue;
	SEXP threshold_values = R_NilValue;
	SEXP threshold_value = R_NilValue;
	SEXP renamed_columns = R_NilValue;
	SEXP reordered_columns = R_NilValue;

	int ncol = 0;

	Descriptor* descriptor = NULL;
	SEXP external_descriptor_pointer = R_NilValue;

	if (descriptor_Robj == R_NilValue) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}
	if (!isVector(descriptor_Robj)) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}

	class_name = getAttrib(descriptor_Robj, R_ClassSymbol);
	if (class_name == R_NilValue) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}
	if (!isString(class_name)) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}
	if (length(class_name) != 1) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}
	if (strcmp(CHAR(STRING_ELT(class_name, 0)), "Descriptor") != 0) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}

	attributes = getAttrib(descriptor_Robj, R_NamesSymbol);
	if (attributes == R_NilValue) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}
	if (!isString(attributes)) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}
	if (length(attributes) != length(descriptor_Robj)) {
		error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
	}

	try {
		descriptor = new Descriptor();

		for (int i = 0; i < length(descriptor_Robj); i++) {
			value = CHAR(STRING_ELT(attributes, i));
			if (strcmp(value, "path_separator") == 0) {
				path_separator = VECTOR_ELT(descriptor_Robj, i);
			} else if (strcmp(value, "full_path") == 0) {
				full_path = VECTOR_ELT(descriptor_Robj, i);
			} else if (strcmp(value, "abbreviation") == 0) {
				abbreviation = VECTOR_ELT(descriptor_Robj, i);
				if (abbreviation != R_NilValue) {
					if (!isString(abbreviation)) {
						error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
					}
					if (length(abbreviation) != 1) {
						error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
					}
					descriptor->set_abbreviation(CHAR(STRING_ELT(abbreviation, 0)));
				}
			} else if (strcmp(value, "columns") == 0) {
				columns = VECTOR_ELT(descriptor_Robj, i);
				if (columns == R_NilValue) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if (!isMatrix(columns)) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if ((ncol = length(columns)) % 2 != 0) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				for (int j = 0; j < ncol / 2; j++) {
					descriptor->add_column(CHAR(STRING_ELT(columns, j)), CHAR(STRING_ELT(columns, j + ncol / 2)));
				}
			} else if (strcmp(value, "properties") == 0) {
				properties = VECTOR_ELT(descriptor_Robj, i);
				if (properties == R_NilValue) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if (!isMatrix(properties)) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if ((ncol = length(properties)) % 2 != 0) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				for (int j = 0; j < ncol / 2; j++) {
					descriptor->add_property(CHAR(STRING_ELT(properties, j)), CHAR(STRING_ELT(properties, j + ncol / 2)));
				}
			} else if (strcmp(value, "thresholds") == 0) {
				thresholds = VECTOR_ELT(descriptor_Robj, i);
				if (thresholds == R_NilValue) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if (!isVector(thresholds)) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if (length(thresholds) != 2) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}

				threshold_names = VECTOR_ELT(thresholds, 0);
				if (threshold_names == R_NilValue) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if (!isVector(threshold_names)) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}

				threshold_values = VECTOR_ELT(thresholds, 1);
				if (threshold_values == R_NilValue) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}
				if (!isVector(threshold_values)) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}

				ncol = length(threshold_names);
				if (ncol != length(threshold_values)) {
					error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
				}

				for (int j = 0; j < ncol; j++) {
					threshold_value = VECTOR_ELT(threshold_values, j);
					if (threshold_value == R_NilValue) {
						error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
					}
					if (length(threshold_value) <= 0) {
						error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
					}
					for (int k = 0; k < length(threshold_value); k++) {
						descriptor->add_threshold(CHAR(STRING_ELT(threshold_names, j)), REAL(threshold_value)[k]);
					}
				}
			} else if (strcmp(value, "renamed_columns") == 0) {
				renamed_columns = VECTOR_ELT(descriptor_Robj, i);
				if (renamed_columns != R_NilValue) {
					if (!isMatrix(renamed_columns)) {
						error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
					}
					if ((ncol = length(renamed_columns)) % 2 != 0) {
						error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
					}
					for (int j = 0; j < ncol / 2; j++) {
						descriptor->add_renamed_column(CHAR(STRING_ELT(renamed_columns, j)), CHAR(STRING_ELT(renamed_columns, j + ncol / 2)));
					}
				}
			} else if (strcmp(value, "reordered_columns") == 0) {
				reordered_columns = VECTOR_ELT(descriptor_Robj, i);
				if (reordered_columns != R_NilValue) {
					if (!isString(reordered_columns)) {
						error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
					}
					for (int j = 0; j < length(reordered_columns); j++) {
						descriptor->add_reordered_column(CHAR(STRING_ELT(reordered_columns, j)));
					}
				}
			}
		}

		if ((full_path == R_NilValue) || (!isString(full_path)) || (length(full_path) != 1)) {
			error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
		}
		if (strlen(CHAR(STRING_ELT(full_path, 0))) <= 0) {
			error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
		}

		if ((path_separator == R_NilValue) || (!isString(path_separator)) || (length(path_separator) != 1)) {
			error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
		}
		if (strlen(CHAR(STRING_ELT(path_separator, 0))) != 1) {
			error("\nMismatch in Descriptor class structure on line %d.", __LINE__);
		}

		descriptor->set_full_path(CHAR(STRING_ELT(full_path, 0)), CHAR(STRING_ELT(path_separator, 0))[0]);
	} catch (DescriptorException &e) {
		error("\n%s", e.what());
	}

	external_descriptor_pointer = R_MakeExternalPtr((void*)descriptor, R_NilValue, R_NilValue);

	return external_descriptor_pointer;
}

SEXP Boxplot2Robj(SEXP external_boxplot_pointer) {
	Boxplot* boxplot = NULL;

	SEXP boxplot_wrapper_obj = R_NilValue;
	SEXP boxplot_robj = R_NilValue;

	SEXP tukey = R_NilValue;
	SEXP observations = R_NilValue;
	SEXP notches = R_NilValue;
	SEXP outliers = R_NilValue;
	SEXP groups = R_NilValue;
	SEXP names = R_NilValue;
	SEXP title = R_NilValue;
	SEXP widths = R_NilValue;
	SEXP quantiles = R_NilValue;
	SEXP boxplot_names = R_NilValue;

	double* tukey_ptr = NULL;
	double* observations_ptr = NULL;
	double* outliers_ptr = NULL;
	double* groups_ptr = NULL;
	double* widths_ptr = NULL;
	double* quantiles_ptr = NULL;

	if (external_boxplot_pointer == R_NilValue) {
		error("\nThe external Boxplot pointer argument is NULL.");
	}

	if (TYPEOF(external_boxplot_pointer) != EXTPTRSXP) {
		error("\nThe external Boxplot pointer argument has an incorrect type.");
	}

	boxplot = (Boxplot*)R_ExternalPtrAddr(external_boxplot_pointer);

	PROTECT(boxplot_names = allocVector(STRSXP, 6));
	SET_STRING_ELT(boxplot_names, 0, mkChar("stats"));
	SET_STRING_ELT(boxplot_names, 1, mkChar("n"));
	SET_STRING_ELT(boxplot_names, 2, mkChar("conf"));
	SET_STRING_ELT(boxplot_names, 3, mkChar("out"));
	SET_STRING_ELT(boxplot_names, 4, mkChar("group"));
	SET_STRING_ELT(boxplot_names, 5, mkChar("names"));

	PROTECT(boxplot_wrapper_obj = allocVector(VECSXP, 4));

	PROTECT(boxplot_robj = allocVector(VECSXP, 6));

	PROTECT(tukey = allocMatrix(REALSXP, Boxplot::TUKEY_SIZE, boxplot->groups_cnt));
	PROTECT(observations = allocVector(REALSXP, boxplot->groups_cnt));
	PROTECT(notches = allocVector(REALSXP, 0));
	PROTECT(outliers = allocVector(REALSXP, boxplot->outliers_cnt));
	PROTECT(groups = allocVector(REALSXP, boxplot->outliers_cnt));
	PROTECT(names = allocVector(STRSXP, boxplot->groups_cnt));
	PROTECT(widths = allocVector(REALSXP, boxplot->groups_cnt));
	PROTECT(quantiles = allocVector(REALSXP, 5));
	PROTECT(title = allocVector(STRSXP, 1));

	tukey_ptr = REAL(tukey);
	observations_ptr = REAL(observations);
	outliers_ptr = REAL(outliers);
	groups_ptr = REAL(groups);
	widths_ptr = REAL(widths);
	quantiles_ptr = REAL(quantiles);

	for (int j = 0; j < (Boxplot::TUKEY_SIZE * boxplot->groups_cnt); j++) {
		tukey_ptr[j] = boxplot->tukey[j];
	}

	for (int j = 0; j < boxplot->groups_cnt; j++) {
		observations_ptr[j] = boxplot->observations[j];
		SET_STRING_ELT(names, j, mkChar(boxplot->names[j]));
		widths_ptr[j] = boxplot->widths[j];
	}

	for (int j = 0; j < boxplot->outliers_cnt; j++) {
		outliers_ptr[j] = boxplot->outliers[j];
		groups_ptr[j] = boxplot->groups[j];
	}

	for (int j = 0; j < 5; j++) {
		quantiles_ptr[j] = boxplot->quantiles[j];
	}

	SET_STRING_ELT(title, 0, mkChar(boxplot->title));

	SET_VECTOR_ELT(boxplot_robj, 0, tukey);
	SET_VECTOR_ELT(boxplot_robj, 1, observations);
	SET_VECTOR_ELT(boxplot_robj, 2, notches);
	SET_VECTOR_ELT(boxplot_robj, 3, outliers);
	SET_VECTOR_ELT(boxplot_robj, 4, groups);
	SET_VECTOR_ELT(boxplot_robj, 5, names);

	setAttrib(boxplot_robj, R_NamesSymbol, boxplot_names);

	SET_VECTOR_ELT(boxplot_wrapper_obj, 0, boxplot_robj);
	SET_VECTOR_ELT(boxplot_wrapper_obj, 1, quantiles);
	SET_VECTOR_ELT(boxplot_wrapper_obj, 2, widths);
	SET_VECTOR_ELT(boxplot_wrapper_obj, 3, title);

	UNPROTECT(12);

	return boxplot_wrapper_obj;
}

SEXP Histogram2Robj(SEXP external_histogram_pointer) {
	Histogram* histogram = NULL;

	SEXP histogram_robj = R_NilValue;

	SEXP breaks = R_NilValue;
	SEXP counts = R_NilValue;
	SEXP density = R_NilValue;
	SEXP intensities = R_NilValue;
	SEXP mids = R_NilValue;
	SEXP xname = R_NilValue;
	SEXP equidist = R_NilValue;
	SEXP title = R_NilValue;
	SEXP histogram_names = R_NilValue;
	SEXP histogram_class = R_NilValue;

	double* breaks_ptr = NULL;
	double* counts_ptr = NULL;
	double* density_ptr = NULL;
	double* intensities_ptr = NULL;
	double* mids_ptr = NULL;

	if (external_histogram_pointer == R_NilValue) {
		error("\nThe external Histogram pointer argument is NULL.");
	}

	if (TYPEOF(external_histogram_pointer) != EXTPTRSXP) {
		error("\nThe external Histogram pointer argument has an incorrect type.");
	}

	histogram = (Histogram*)R_ExternalPtrAddr(external_histogram_pointer);

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

	PROTECT(histogram_robj = allocVector(VECSXP, 8));

	PROTECT(breaks = allocVector(REALSXP, histogram->nclass + 1));
	PROTECT(counts = allocVector(REALSXP, histogram->nclass));
	PROTECT(density = allocVector(REALSXP, histogram->nclass));
	PROTECT(intensities = allocVector(REALSXP, histogram->nclass));
	PROTECT(mids = allocVector(REALSXP, histogram->nclass));
	PROTECT(xname = allocVector(STRSXP, 1));
	PROTECT(equidist = allocVector(LGLSXP, 1));
	PROTECT(title = allocVector(STRSXP, 1));

	breaks_ptr = REAL(breaks);
	counts_ptr = REAL(counts);
	density_ptr = REAL(density);
	intensities_ptr = REAL(intensities);
	mids_ptr = REAL(mids);

	for (int j = 0; j < histogram->nclass; j++) {
		breaks_ptr[j] = histogram->breaks[j];
		counts_ptr[j] = histogram->counts[j];
		density_ptr[j] = histogram->density[j];
		intensities_ptr[j] = histogram->intensities[j];
		mids_ptr[j] = histogram->mids[j];
	}

	breaks_ptr[histogram->nclass] = histogram->breaks[histogram->nclass];

	SET_STRING_ELT(xname, 0, mkChar(histogram->xname));
	LOGICAL(equidist)[0] = 1;

	SET_STRING_ELT(title, 0, mkChar(histogram->title));

	SET_VECTOR_ELT(histogram_robj, 0, breaks);
	SET_VECTOR_ELT(histogram_robj, 1, counts);
	SET_VECTOR_ELT(histogram_robj, 2, density);
	SET_VECTOR_ELT(histogram_robj, 3, intensities);
	SET_VECTOR_ELT(histogram_robj, 4, mids);
	SET_VECTOR_ELT(histogram_robj, 5, xname);
	SET_VECTOR_ELT(histogram_robj, 6, equidist);
	SET_VECTOR_ELT(histogram_robj, 7, title);

	setAttrib(histogram_robj, R_NamesSymbol, histogram_names);
	setAttrib(histogram_robj, R_ClassSymbol, histogram_class);

	UNPROTECT(11);

	return histogram_robj;
}

SEXP Qqplot2Robj(SEXP external_qqplot_pointer) {
	Qqplot* qqplot = NULL;

	SEXP qqplot_robj = R_NilValue;

	SEXP name = R_NilValue;
	SEXP color = R_NilValue;
	SEXP lambda = R_NilValue;
	SEXP points = R_NilValue;
	SEXP x = R_NilValue;
	SEXP y = R_NilValue;

	double* x_ptr = NULL;
	double* y_ptr = NULL;

	int qqplot_coord_index = 0;

	if (external_qqplot_pointer == R_NilValue) {
		error("\nThe external Qqplot pointer argument is NULL.");
	}

	if (TYPEOF(external_qqplot_pointer) != EXTPTRSXP) {
		error("\nThe external Qqplot pointer argument has an incorrect type.");
	}

	qqplot = (Qqplot*)R_ExternalPtrAddr(external_qqplot_pointer);

	PROTECT(qqplot_robj = allocVector(VECSXP, qqplot->count * 6));

	for (int k = 0; k < qqplot->count; k++) {
		PROTECT(name = allocVector(STRSXP, 1));
		PROTECT(color = allocVector(STRSXP, 1));
		PROTECT(lambda = allocVector(REALSXP, 1));
		PROTECT(points = allocVector(INTSXP, 1));
		PROTECT(x = allocVector(REALSXP, qqplot->size[k]));
		PROTECT(y = allocVector(REALSXP, qqplot->size[k]));

		x_ptr = REAL(x);
		y_ptr = REAL(y);

		for (int j = 0; j < qqplot->size[k]; j++) {
			x_ptr[j] = qqplot->x[qqplot_coord_index];
			y_ptr[j] = qqplot->y[qqplot_coord_index];
			qqplot_coord_index += 1;
		}

		SET_STRING_ELT(name, 0, mkChar(qqplot->names[k]));
		SET_STRING_ELT(color, 0, mkChar(qqplot->colors[k]));
		REAL(lambda)[0] = qqplot->lambdas[k];
		INTEGER(points)[0] = qqplot->points[k];

		SET_VECTOR_ELT(qqplot_robj, k * 6, name);
		SET_VECTOR_ELT(qqplot_robj, k * 6 + 1, color);
		SET_VECTOR_ELT(qqplot_robj, k * 6 + 2, lambda);
		SET_VECTOR_ELT(qqplot_robj, k * 6 + 3, points);
		SET_VECTOR_ELT(qqplot_robj, k * 6 + 4, x);
		SET_VECTOR_ELT(qqplot_robj, k * 6 + 5, y);

		UNPROTECT(6);
	}

	UNPROTECT(1);

	return qqplot_robj;
}

SEXP Plot2Robj(SEXP external_plot_pointer) {
	Plot* plot = NULL;

	SEXP plot_robj = R_NilValue;

	SEXP plot_title = R_NilValue;
	SEXP plot_name = R_NilValue;
	SEXP external_pointer = R_NilValue;
	SEXP histograms = R_NilValue;
	SEXP histogram = R_NilValue;
	SEXP boxplots = R_NilValue;
	SEXP boxplot = R_NilValue;
	SEXP qqplots = R_NilValue;
	SEXP qqplot = R_NilValue;

	if (external_plot_pointer == R_NilValue) {
		error("\nThe external Plot pointer argument is NULL.");
	}

	if (TYPEOF(external_plot_pointer) != EXTPTRSXP) {
		error("\nThe external Plot pointer argument has an incorrect type.");
	}

	plot = (Plot*)R_ExternalPtrAddr(external_plot_pointer);

	PROTECT(plot_robj = allocVector(VECSXP, 5));

	PROTECT(plot_title = allocVector(STRSXP, 1));
	SET_STRING_ELT(plot_title, 0, (plot->title != NULL) ? mkChar(plot->title) : mkChar(""));
	SET_VECTOR_ELT(plot_robj, 0, plot_title);
	UNPROTECT(1);

	PROTECT(plot_name = allocVector(STRSXP, 1));
	SET_STRING_ELT(plot_name, 0, (plot->name != NULL) ? mkChar(plot->name) : mkChar(""));
	SET_VECTOR_ELT(plot_robj, 1, plot_name);
	UNPROTECT(1);

	if (plot->histograms.size() > 0) {
		PROTECT(histograms = allocVector(VECSXP, plot->histograms.size()));
		for (unsigned int h = 0; h < plot->histograms.size(); h++) {
			external_pointer = R_MakeExternalPtr((void*)plot->histograms.at(h), R_NilValue, R_NilValue);
			histogram = Histogram2Robj(external_pointer);
			SET_VECTOR_ELT(histograms, h, histogram);
		}

		SET_VECTOR_ELT(plot_robj, 2, histograms);
		UNPROTECT(1);
	} else {
		SET_VECTOR_ELT(plot_robj, 2, R_NilValue);
	}

	if (plot->boxplots.size() > 0) {
		PROTECT(boxplots = allocVector(VECSXP, plot->boxplots.size()));
		for (unsigned int b = 0; b < plot->boxplots.size(); b++) {
			external_pointer = R_MakeExternalPtr((void*)plot->boxplots.at(b), R_NilValue, R_NilValue);
			boxplot = Boxplot2Robj(external_pointer);
			SET_VECTOR_ELT(boxplots, b, boxplot);
		}

		SET_VECTOR_ELT(plot_robj, 3, boxplots);
		UNPROTECT(1);
	} else {
		SET_VECTOR_ELT(plot_robj, 3, R_NilValue);
	}

	if (plot->qqplots.size() > 0) {
		PROTECT(qqplots = allocVector(VECSXP, plot->qqplots.size()));
		for (unsigned int q = 0; q < plot->qqplots.size(); q++) {
			external_pointer = R_MakeExternalPtr((void*)plot->qqplots.at(q), R_NilValue, R_NilValue);
			qqplot = Qqplot2Robj(external_pointer);
			SET_VECTOR_ELT(qqplots, q, qqplot);
		}

		SET_VECTOR_ELT(plot_robj, 4, qqplots);
		UNPROTECT(1);
	} else {
		SET_VECTOR_ELT(plot_robj, 4, R_NilValue);
	}

	UNPROTECT(1);

	return plot_robj;
}

SEXP perform_quality_check(SEXP external_descriptor_pointer, SEXP resource_path) {
	Descriptor* descriptor = NULL;
	const char* c_resource_path = NULL;
	GwaFile* gwa_file = NULL;

	void (GwaFile::*check_functions[7])(Descriptor*) = {
			&GwaFile::check_filters,
			&GwaFile::check_thresholds,
			&GwaFile::check_prefix,
			&GwaFile::check_casesensitivity,
			&GwaFile::check_missing_value,
			&GwaFile::check_separators,
			&GwaFile::check_filesize
	};

	SEXP output_robj = R_NilValue;
	SEXP single_file_plots_robj = R_NilValue;
	SEXP common_boxplots_robj = R_NilValue;
	SEXP external_pointer = R_NilValue;
	SEXP plot = R_NilValue;
	SEXP name = R_NilValue;
	SEXP path = R_NilValue;
	SEXP stats = R_NilValue;

	clock_t start_time = 0;
	double execution_time = 0.0;
	double used_memory = 0.0;

	if (external_descriptor_pointer == R_NilValue) {
		error("\nThe external Descriptor pointer argument is NULL.");
	}

	if (TYPEOF(external_descriptor_pointer) != EXTPTRSXP) {
		error("\nThe external Descriptor pointer argument has an incorrect type.");
	}

	if (resource_path == R_NilValue) {
		error("\nThe resource path is NULL.");
	}

	if (!isString(resource_path)) {
		error("\nThe resource path is not a string.");
	}

	if (length(resource_path) <= 0) {
		error("\nThe resource path is empty.");
	}

	if (length(resource_path) > 1) {
		error("\nThe resource path has multiple values.");
	}

	descriptor = (Descriptor*)R_ExternalPtrAddr(external_descriptor_pointer);
	c_resource_path = CHAR(STRING_ELT(resource_path, 0));

	vector<Plot*> single_file_plots;
	vector<Plot*> combined_boxplots;

	char* result_html_path = NULL;

	try {
		start_time = clock();

		Analyzer analyzer;

		gwa_file = new GwaFile(descriptor, check_functions, 7);

		analyzer.open_gwafile(gwa_file);

		analyzer.process_header();

		analyzer.initialize_column_dependencies();
		analyzer.initialize_filtered_columns();
		analyzer.initialize_columns_ratios();

		analyzer.process_data();

		used_memory = analyzer.get_memory_usage();

		analyzer.finalize_processing();

		analyzer.create_plots(single_file_plots);
		analyzer.create_combined_qqplots(single_file_plots);
		analyzer.create_combined_boxplots(combined_boxplots);

		analyzer.print_txt_report();
		analyzer.print_csv_report();
		analyzer.print_html_report(&result_html_path, single_file_plots, c_resource_path);

		PROTECT(output_robj = allocVector(VECSXP, 5));

		if (single_file_plots.size() > 0) {
			PROTECT(single_file_plots_robj = allocVector(VECSXP, single_file_plots.size()));
			for (unsigned int i = 0; i < single_file_plots.size(); i++) {
				external_pointer = R_MakeExternalPtr((void*)single_file_plots.at(i), R_NilValue, R_NilValue);
				plot = Plot2Robj(external_pointer);
				SET_VECTOR_ELT(single_file_plots_robj, i, plot);
			}
			UNPROTECT(1);
		}

		if (combined_boxplots.size() > 0) {
			PROTECT(common_boxplots_robj = allocVector(VECSXP, combined_boxplots.size()));
			for (unsigned int i = 0; i < combined_boxplots.size(); i++) {
				external_pointer = R_MakeExternalPtr((void*)combined_boxplots.at(i), R_NilValue, R_NilValue);
				plot = Plot2Robj(external_pointer);
				SET_VECTOR_ELT(common_boxplots_robj, i, plot);
			}
			UNPROTECT(1);
		}

		PROTECT(name = allocVector(STRSXP, 1));
		SET_STRING_ELT(name, 0, mkChar(gwa_file->get_descriptor()->get_name()));
		UNPROTECT(1);

		PROTECT(path = allocVector(STRSXP, 1));
		SET_STRING_ELT(path, 0, mkChar(result_html_path));
		UNPROTECT(1);

		execution_time = (clock() - start_time)/(double)CLOCKS_PER_SEC;

		PROTECT(stats = allocVector(REALSXP, 2));
		REAL(stats)[0] = execution_time;
		REAL(stats)[1] = used_memory;
		UNPROTECT(1);

		SET_VECTOR_ELT(output_robj, 0, name);
		SET_VECTOR_ELT(output_robj, 1, path);
		SET_VECTOR_ELT(output_robj, 2, single_file_plots_robj);
		SET_VECTOR_ELT(output_robj, 3, common_boxplots_robj);
		SET_VECTOR_ELT(output_robj, 4, stats);

		UNPROTECT(1);

		analyzer.close_gwafile();

		delete gwa_file;
	} catch (Exception &e) {
		error("\n%s", e.what());
	}

	return output_robj;
}

SEXP print_combined_reports(SEXP reports, SEXP boxplot_names, SEXP resource_path, SEXP path_separator) {
	int ncol = 0;
	const char* c_resource_path = NULL;
	const char* c_path_separator = NULL;

	vector<const char*> c_gwa_file_names;
	vector<const char*> c_html_report_paths;
	int combined_boxplots_number = 0;
	vector<const char*> c_boxplot_names;

	if (resource_path == R_NilValue) {
		error("\nThe resource path is NULL.");
	}

	if (!isString(resource_path)) {
		error("\nThe resource path is not a string.");
	}

	if (length(resource_path) <= 0) {
		error("\nThe resource path is empty.");
	}

	if (length(resource_path) > 1) {
		error("\nThe resource path has multiple values.");
	}

	c_resource_path = CHAR(STRING_ELT(resource_path, 0));

	if (path_separator == R_NilValue) {
		error("\nThe path separator is NULL.");
	}

	if (!isString(path_separator)) {
		error("\nThe path separator is not a string.");
	}

	if (length(path_separator) <= 0) {
		error("\nThe path separator is empty.");
	}

	if (length(path_separator) > 1) {
		error("\nThe path separator has multiple values.");
	}

	c_path_separator = CHAR(STRING_ELT(path_separator, 0));
	if (strlen(c_path_separator) <= 0) {
		error("\nThe path separator is blank.");
	}

	if (strlen(c_path_separator) > 1) {
		error("\nThe path separator contains multiple characters.");
	}

	if (reports == R_NilValue) {
		error("\nThe reports argument is NULL.");
	}

	if (!isMatrix(reports)) {
		error("\nThe reports argument is not a matrix.");
	}

	if (!isString(reports)) {
		error("\nThe type of reports argument is not 'character'.");
	}

	if ((ncol = length(reports)) % 2 != 0) {
		error("\nThe number of columns in reports matrix is incorrect.");
	}

	for (int i = 0; i < ncol / 2; i++) {
		c_gwa_file_names.push_back(CHAR(STRING_ELT(reports, i)));
		c_html_report_paths.push_back(CHAR(STRING_ELT(reports, i + ncol / 2)));
	}

	if (boxplot_names == R_NilValue) {
		error("\nThe boxplot names argument is NULL.");
	}

	if (!isVector(boxplot_names)) {
		error("\nThe boxplot names argument is not a vector.");
	}

	if ((combined_boxplots_number = length(boxplot_names)) > 0) {
		if (!isString(boxplot_names)) {
			error("\nThe type of boxplot names argument is not 'character'.");
		}

		for (int i = 0; i < combined_boxplots_number; i++) {
			c_boxplot_names.push_back(CHAR(STRING_ELT(boxplot_names, i)));
		}
	}

	try {
		Analyzer::print_main_html(c_gwa_file_names, c_html_report_paths, c_boxplot_names, c_resource_path, c_path_separator[0]);
	} catch (Exception &e) {
		error("\n%s", e.what());
	}

	c_gwa_file_names.clear();
	c_html_report_paths.clear();
	c_boxplot_names.clear();

	return R_NilValue;
}

SEXP perform_formatting(SEXP external_descriptor_pointer) {
	Descriptor* descriptor = NULL;
	GwaFile* gwa_file = NULL;

	void (GwaFile::*check_functions[8])(Descriptor*) = {
			&GwaFile::check_filters,
			&GwaFile::check_thresholds,
			&GwaFile::check_prefix,
			&GwaFile::check_casesensitivity,
			&GwaFile::check_missing_value,
			&GwaFile::check_separators,
			&GwaFile::check_order,
			&GwaFile::check_genomiccontrol
	};

	double inflation_factor = numeric_limits<double>::quiet_NaN();
	int n_total = 0;
	int n_filtered = 0;

	SEXP output_robj = R_NilValue;
	SEXP attributes = R_NilValue;
	SEXP name = R_NilValue;
	SEXP inflation_factor_value = R_NilValue;
	SEXP inflation_factor_stats = R_NilValue;
	SEXP formatting_stats = R_NilValue;

	clock_t start_time = 0;
	double execution_time = 0.0;

	if (external_descriptor_pointer == R_NilValue) {
		error("\nThe external Descriptor pointer argument is NULL.");
	}

	if (TYPEOF(external_descriptor_pointer) != EXTPTRSXP) {
		error("\nThe external Descriptor pointer argument has an incorrect type.");
	}

	descriptor = (Descriptor*)R_ExternalPtrAddr(external_descriptor_pointer);

	PROTECT(output_robj = allocVector(VECSXP, 4));

	PROTECT(attributes = allocVector(STRSXP, 4));
	SET_STRING_ELT(attributes, 0, mkChar("name"));
	SET_STRING_ELT(attributes, 1, mkChar("inflation_factor_value"));
	SET_STRING_ELT(attributes, 2, mkChar("inflation_factor_stats"));
	SET_STRING_ELT(attributes, 3, mkChar("formatting_stats"));

	PROTECT(name = allocVector(STRSXP, 1));
	SET_STRING_ELT(name, 0, mkChar(descriptor->get_name()));
	UNPROTECT(1);

	try {
		Formatter formatter;

		gwa_file = new GwaFile(descriptor, check_functions, 8);

		formatter.open_gwafile(gwa_file);
		formatter.process_header();

		if (gwa_file->is_gc_on()) {
			inflation_factor = gwa_file->get_inflation_factor();
			if (isnan(inflation_factor)) {
				start_time = clock();
				inflation_factor = formatter.calculate_lambda(n_total, n_filtered);
				execution_time = (clock() - start_time)/(double)CLOCKS_PER_SEC;

				PROTECT(inflation_factor_stats = allocVector(REALSXP, 3));
				REAL(inflation_factor_stats)[0] = execution_time;
				REAL(inflation_factor_stats)[1] = n_filtered;
				REAL(inflation_factor_stats)[2] = n_total;
				UNPROTECT(1);
			}

			PROTECT(inflation_factor_value = allocVector(REALSXP, 1));
			REAL(inflation_factor_value)[0] = inflation_factor;
			UNPROTECT(1);
		}

		start_time = clock();
		formatter.format(inflation_factor, '\t', n_total, n_filtered);
		execution_time = (clock() - start_time)/(double)CLOCKS_PER_SEC;

		PROTECT(formatting_stats = allocVector(REALSXP, 3));
		REAL(formatting_stats)[0] = execution_time;
		REAL(formatting_stats)[1] = n_filtered;
		REAL(formatting_stats)[2] = n_total;
		UNPROTECT(1);

		SET_VECTOR_ELT(output_robj, 0, name);
		SET_VECTOR_ELT(output_robj, 1, inflation_factor_value);
		SET_VECTOR_ELT(output_robj, 2, inflation_factor_stats);
		SET_VECTOR_ELT(output_robj, 3, formatting_stats);

		setAttrib(output_robj, R_NamesSymbol, attributes);

		UNPROTECT(2);

		formatter.close_gwafile();

		delete gwa_file;
	} catch (Exception &e) {
		error("\n%s", e.what());
	}

	return output_robj;
}

}

/*
 *	Description:
 *		Entry point for a stand-alone application.
 *		During compilation needs to be linked to R library.
 *	Arguments:
 *		argv[1]	--	METAL-like script file name
 */
int main(int args, char** argv) {
/*	char fileSep = '\\';
	const char* resLocation = "inst\\extdata\\";

	vector<Descriptor*>* ds = NULL;
	vector<Descriptor*>::iterator ds_it;
	GwaFile* gwa_file = NULL;
	double used_memory = 0.0;

	 gwasqc() functionality
	try {
		Analyzer analyzer;

		void (GwaFile::*check_functions[7])(Descriptor*) = {
				&GwaFile::check_filters,
				&GwaFile::check_thresholds,
				&GwaFile::check_prefix,
				&GwaFile::check_casesensitivity,
				&GwaFile::check_missing_value,
				&GwaFile::check_separators,
				&GwaFile::check_filesize
		};

		vector<Plot*> plots;
		vector<Plot*> combined_boxplots;

		char* html_path = NULL;

		ds = Descriptor::process_instructions("qc_script.txt", fileSep);

		for (ds_it = ds->begin(); ds_it != ds->end(); ds_it++) {
			gwa_file = new GwaFile(*ds_it, check_functions, 7);

			cout << gwa_file->get_descriptor()->get_full_path() << endl;
			cout << gwa_file->get_estimated_size() << endl;

			analyzer.open_gwafile(gwa_file);

			analyzer.process_header();

			analyzer.initialize_column_dependencies();
			analyzer.initialize_filtered_columns();
			analyzer.initialize_columns_ratios();

			analyzer.process_data();

			used_memory = analyzer.get_memory_usage();
			cout << used_memory << " Mb" << endl;

			analyzer.finalize_processing();

			analyzer.create_plots(plots);
			analyzer.create_combined_qqplots(plots);
			analyzer.create_combined_boxplots(combined_boxplots);

			analyzer.print_html_report(&html_path, plots, resLocation);
			analyzer.print_txt_report();
			analyzer.print_csv_report();

			analyzer.close_gwafile();

			delete gwa_file;
			delete (*ds_it);
		}

		ds->clear();
	} catch (Exception &e) {
		cout << endl;
		cout << e.what() << endl;
	}*/

	/* gwasformat() functionality */
/*	try {
		Formatter formatter;

		void (GwaFile::*check_functions[8])(Descriptor*) = {
				&GwaFile::check_filters,
				&GwaFile::check_thresholds,
				&GwaFile::check_prefix,
				&GwaFile::check_casesensitivity,
				&GwaFile::check_missing_value,
				&GwaFile::check_separators,
				&GwaFile::check_order,
				&GwaFile::check_genomiccontrol
		};

		double inflation_factor = numeric_limits<double>::quiet_NaN();
		int n_total = 0;
		int n_filtered = 0;

		ds = Descriptor::process_instructions("QC_script_format.txt", fileSep);

		for (ds_it = ds->begin(); ds_it != ds->end(); ds_it++) {
			gwa_file = new GwaFile(*ds_it, check_functions, 8);

			cout << gwa_file->get_descriptor()->get_full_path() << endl;

			formatter.open_gwafile(gwa_file);
			formatter.process_header();

			if (gwa_file->is_gc_on()) {
				inflation_factor = gwa_file->get_inflation_factor();
				if (isnan(inflation_factor)) {
					inflation_factor = formatter.calculate_lambda(n_total, n_filtered);
				}
			}

			cout << "Inflation factor: " << inflation_factor << endl;

			formatter.format(inflation_factor, '\t', n_total, n_filtered);

			formatter.close_gwafile();

			delete gwa_file;
			delete (*ds_it);
		}
		ds->clear();
	} catch (Exception &e) {
		cout << endl;
		cout << e.what() << endl;
	}*/

	return 0;
}

