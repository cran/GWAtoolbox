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

#include "../../include/formatter/Formatter.h"

const unsigned int Formatter::HEAP_SIZE = 3500000;
const unsigned int Formatter::HEAP_INCREMENT = 100000;
const double Formatter::EPSILON = 0.00000001;

Formatter::Formatter() : gwafile(NULL), maf_column(NULL), pvalue_column(NULL),
	stderr_column(NULL), n_total_column(NULL), oevar_imp_column(NULL),
	pvalue_column_pos(numeric_limits<int>::min()),
	maf_column_pos(numeric_limits<int>::min()),
	oevar_imp_column_pos(numeric_limits<int>::min()) {

}

Formatter::~Formatter() {
	gwafile = NULL;

	input_columns.clear();

	for (columns_it = output_columns.begin(); columns_it != output_columns.end(); columns_it++) {
		delete *columns_it;
	}
	output_columns.clear();

	maf_column = NULL;
	pvalue_column = NULL;
	stderr_column = NULL;
	n_total_column = NULL;
	oevar_imp_column = NULL;

	pvalue_column_pos = numeric_limits<int>::min();
	maf_column_pos = numeric_limits<int>::min();
	oevar_imp_column_pos = numeric_limits<int>::min();
}

bool Formatter::compare_columns(Column* first, Column* second) {
	return (first->get_order() < second->get_order());
}

bool Formatter::compare_columns_by_name(Column* first, Column* second) {
	return (strcmp(first->get_header(), second->get_header()) < 0);
}

void Formatter::open_gwafile(GwaFile* gwafile) throw (FormatterException) {
	if (gwafile == NULL) {
		throw FormatterException("Formatter", "open_gwafile( GwaFile* )", __LINE__, 0, "gwafile");
	}

	try {
		close_gwafile();
		this->gwafile = gwafile;
		reader.set_file_name(gwafile->get_descriptor()->get_full_path());
		reader.open();
	} catch (ReaderException& e) {
		FormatterException new_e(e);
		new_e.add_message("Formatter", "open_gwafile( GwaFile* )", __LINE__, 3, gwafile->get_descriptor()->get_full_path());
		throw new_e;
	} catch (FormatterException& e) {
		e.add_message("Formatter", "open_gwafile( GwaFile* )", __LINE__, 3, gwafile->get_descriptor()->get_full_path());
		throw;
	}
}

void Formatter::close_gwafile() throw (FormatterException) {
	try {
		reader.close();
	} catch (ReaderException &e) {
		FormatterException new_e(e);
		new_e.add_message("Formatter", "close_gwafile()", __LINE__, 4, gwafile != NULL ? gwafile->get_descriptor()->get_full_path() : "NULL");
		throw new_e;
	}

	gwafile = NULL;

	input_columns.clear();

	for (columns_it = output_columns.begin(); columns_it != output_columns.end(); columns_it++) {
		delete *columns_it;
	}
	output_columns.clear();

	maf_column = NULL;
	pvalue_column = NULL;
	stderr_column = NULL;
	n_total_column = NULL;
	oevar_imp_column = NULL;

	pvalue_column_pos = numeric_limits<int>::min();
	maf_column_pos = numeric_limits<int>::min();
	oevar_imp_column_pos = numeric_limits<int>::min();
}

void Formatter::process_header() throw (FormatterException) {
	Descriptor* descriptor = NULL;
	char header_separator = '\0';
	char* header = NULL;
	char* token = NULL;
	int column_position = 0;
	const char* column_name = NULL;
	const char* new_column_name = NULL;

	Column* column = NULL;

	if (gwafile == NULL) {
		return;
	}

	try {
		if (reader.read_line() <= 0) {
			throw FormatterException("Formatter", "process_header()", __LINE__, 5, gwafile->get_descriptor()->get_name());
		}

		descriptor = gwafile->get_descriptor();
		header_separator = gwafile->get_header_separator();
		header = *reader.line;

		token = auxiliary::strtok(&header, header_separator);
		while (token != NULL) {
			column = new Column();

			new_column_name = descriptor->get_renamed_column(token);
			if (new_column_name == NULL) {
				new_column_name = token;
			}

			column_name = descriptor->get_default_column(new_column_name, gwafile->is_case_sensitive());
			if (column_name != NULL) {
				if (strcmp(column_name, Descriptor::MARKER) == 0) {
				} else if (strcmp(column_name, Descriptor::CHR) == 0) {
				} else if (strcmp(column_name, Descriptor::POSITION) == 0) {
				} else if (strcmp(column_name, Descriptor::ALLELE1) == 0) {
				} else if (strcmp(column_name, Descriptor::ALLELE2) == 0) {
				} else if (strcmp(column_name, Descriptor::STRAND) == 0) {
				} else if (strcmp(column_name, Descriptor::EFFECT) == 0) {
				} else if (strcmp(column_name, Descriptor::STDERR) == 0) {
					stderr_column = column;
				} else if (strcmp(column_name, Descriptor::PVALUE) == 0) {
					pvalue_column = column;
					pvalue_column_pos = column_position;
				} else if (strcmp(column_name, Descriptor::FREQLABEL) == 0) {
					maf_column = column;
					maf_column_pos = column_position;
				} else if (strcmp(column_name, Descriptor::HWE_PVAL) == 0) {
				} else if (strcmp(column_name, Descriptor::CALLRATE) == 0) {
				} else if (strcmp(column_name, Descriptor::N_TOTAL) == 0) {
					n_total_column = column;
				} else if (strcmp(column_name, Descriptor::IMPUTED) == 0) {
				} else if (strcmp(column_name, Descriptor::USED_FOR_IMP) == 0) {
				} else if (strcmp(column_name, Descriptor::OEVAR_IMP) == 0) {
					oevar_imp_column = column;
					oevar_imp_column_pos = column_position;
				} else if (strcmp(column_name, Descriptor::AVPOSTPROB) == 0) {
				}
			}

			column->set_header(new_column_name);
			column->set_order(descriptor->get_column_order(new_column_name, gwafile->is_case_sensitive()));

			input_columns.push_back(column);

			token = auxiliary::strtok(&header, header_separator);
			column_position += 1;
		}
	} catch (ReaderException &e) {
		FormatterException new_e(e);
		new_e.add_message("Formatter", "process_header()", __LINE__, 6, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (DescriptorException &e) {
		FormatterException new_e(e);
		new_e.add_message("Formatter", "process_header()", __LINE__, 6, gwafile->get_descriptor()->get_name());
		throw new_e;
	}
}

double Formatter::calculate_lambda(int& n_total, int& n_filtered) throw (FormatterException) {
	Descriptor* descriptor = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 2;
	char data_separator = '\0';

	vector<double>* snp_hq = NULL;
	double maf_filter_value = -numeric_limits<double>::infinity();
	double oevar_imp_filter_value = -numeric_limits<double>::infinity();
	bool maf_filter = false;
	bool oevar_imp_filter = false;

	char* token = NULL;
	char* pvalue_token = NULL;
	char* maf_token = NULL;
	char* oevar_imp_token = NULL;
	int column_position = 0;

	char* end_ptr = NULL;
	double d_value = 0.0;

	int n = 0;
	double* data = NULL;
	double* new_data = NULL;
	int current_heap_size = HEAP_SIZE;

	double lambda = numeric_limits<double>::quiet_NaN();

	n_total = 0;
	n_filtered = 0;

	if ((gwafile == NULL) || (pvalue_column_pos < 0)) {
		return lambda;
	}

	try {
		descriptor = gwafile->get_descriptor();
		snp_hq = descriptor->get_threshold(Descriptor::SNP_HQ);
		data_separator = gwafile->get_data_separator();

		if (((maf_filter_value = snp_hq->at(0)) > 0) && (maf_column_pos >= 0)){
			maf_filter = true;
		}

		if (((oevar_imp_filter_value = snp_hq->at(1)) > 0) && (oevar_imp_column_pos >= 0)) {
			oevar_imp_filter = true;
		}

		data = (double*)malloc(HEAP_SIZE * sizeof(double));
		if (data == NULL) {
			throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 2, HEAP_SIZE * sizeof(double));
		}

		if (maf_filter) {
			if (oevar_imp_filter) {
				// all filters
				while ((line_length = reader.read_line()) > 0) {
					line = *reader.line;

					column_position = 0;
					pvalue_token = NULL;
					maf_token = NULL;
					oevar_imp_token = NULL;
					token = auxiliary::strtok(&line, data_separator);
					while (token != NULL) {
						if (column_position == pvalue_column_pos) {
							pvalue_token = token;
						} else if (column_position == maf_column_pos) {
							maf_token = token;
						} else if (column_position == oevar_imp_column_pos) {
							oevar_imp_token = token;
						}

						token = auxiliary::strtok(&line, data_separator);
						column_position += 1;
					}

					if (pvalue_token == NULL) {
						throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, pvalue_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
					}

					if (maf_token == NULL) {
						throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, maf_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
					}

					if (oevar_imp_token == NULL) {
						throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, oevar_imp_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
					}

					d_value = R_strtod(maf_token, &end_ptr);
					if ((*end_ptr != '\0') || (ISNAN(d_value))) {
						line_number += 1;
						continue;
					}

					d_value = d_value > 0.5 ? 1.0 - d_value : d_value;

					if (auxiliary::fcmp(d_value, maf_filter_value, EPSILON) == 1) {
						d_value = R_strtod(oevar_imp_token, &end_ptr);
						if ((*end_ptr != '\0') || (ISNAN(d_value))) {
							line_number += 1;
							continue;
						}

						if (auxiliary::fcmp(d_value, oevar_imp_filter_value, EPSILON) == 1) {
							d_value = R_strtod(pvalue_token, &end_ptr);
							if ((*end_ptr != '\0') || (ISNAN(d_value))) {
								line_number += 1;
								continue;
							}

							n += 1;
							if (n > current_heap_size) {
								current_heap_size += HEAP_INCREMENT;

								new_data = (double*)realloc(data, current_heap_size * sizeof(double));
								if (new_data == NULL) {
									free(data);
									data = NULL;
									throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 2, current_heap_size * sizeof(double));
								}
								data = new_data;
							}

							data[n - 1] = d_value;
						}
					}

					line_number += 1;
				}
			} else {
				// only maf filter
				while ((line_length = reader.read_line()) > 0) {
					line = *reader.line;

					column_position = 0;
					pvalue_token = NULL;
					maf_token = NULL;
					token = auxiliary::strtok(&line, data_separator);
					while (token != NULL) {
						if (column_position == pvalue_column_pos) {
							pvalue_token = token;
						} else if (column_position == maf_column_pos) {
							maf_token = token;
						}

						token = auxiliary::strtok(&line, data_separator);
						column_position += 1;
					}

					if (pvalue_token == NULL) {
						throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, pvalue_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
					}

					if (maf_token == NULL) {
						throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, maf_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
					}

					d_value = R_strtod(maf_token, &end_ptr);
					if ((*end_ptr != '\0') || (ISNAN(d_value))) {
						line_number += 1;
						continue;
					}

					d_value = d_value > 0.5 ? 1.0 - d_value : d_value;

					if (auxiliary::fcmp(d_value, maf_filter_value, EPSILON) == 1) {
						d_value = R_strtod(pvalue_token, &end_ptr);
						if ((*end_ptr != '\0') || (ISNAN(d_value))) {
							line_number += 1;
							continue;
						}

						n += 1;
						if (n > current_heap_size) {
							current_heap_size += HEAP_INCREMENT;

							new_data = (double*)realloc(data, current_heap_size * sizeof(double));
							if (new_data == NULL) {
								free(data);
								data = NULL;
								throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 2, current_heap_size * sizeof(double));
							}
							data = new_data;
						}

						data[n - 1] = d_value;
					}

					line_number += 1;
				}
			}
		} else if (oevar_imp_filter) {
			// only oevar_imp_filter
			while ((line_length = reader.read_line()) > 0) {
				line = *reader.line;

				column_position = 0;
				pvalue_token = NULL;
				oevar_imp_token = NULL;
				token = auxiliary::strtok(&line, data_separator);
				while (token != NULL) {
					if (column_position == pvalue_column_pos) {
						pvalue_token = token;
					} else if (column_position == oevar_imp_column_pos) {
						oevar_imp_token = token;
					}

					token = auxiliary::strtok(&line, data_separator);
					column_position += 1;
				}

				if (pvalue_token == NULL) {
					throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, pvalue_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
				}

				if (oevar_imp_token == NULL) {
					throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, oevar_imp_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
				}

				d_value = R_strtod(oevar_imp_token, &end_ptr);
				if ((*end_ptr != '\0') || (ISNAN(d_value))) {
					line_number += 1;
					continue;
				}

				if (auxiliary::fcmp(d_value, oevar_imp_filter_value, EPSILON) == 1) {
					d_value = R_strtod(pvalue_token, &end_ptr);
					if ((*end_ptr != '\0') || (ISNAN(d_value))) {
						line_number += 1;
						continue;
					}

					n += 1;
					if (n > current_heap_size) {
						current_heap_size += HEAP_INCREMENT;

						new_data = (double*)realloc(data, current_heap_size * sizeof(double));
						if (new_data == NULL) {
							free(data);
							data = NULL;
							throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 2, current_heap_size * sizeof(double));
						}
						data = new_data;
					}

					data[n - 1] = d_value;
				}

				line_number += 1;
			}
		} else {
			// no filters
			while ((line_length = reader.read_line()) > 0) {
				line = *reader.line;

				column_position = 0;
				pvalue_token = NULL;
				token = auxiliary::strtok(&line, data_separator);
				while (token != NULL) {
					if (column_position == pvalue_column_pos) {
						pvalue_token = token;
						break;
					}

					token = auxiliary::strtok(&line, data_separator);
					column_position += 1;
				}

				if (pvalue_token == NULL) {
					throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 8, pvalue_column->get_header(), line_number, gwafile->get_descriptor()->get_name());
				}

				d_value = R_strtod(pvalue_token, &end_ptr);
				if ((*end_ptr != '\0') || (ISNAN(d_value))) {
					line_number += 1;
					continue;
				}

				n += 1;
				if (n > current_heap_size) {
					current_heap_size += HEAP_INCREMENT;

					new_data = (double*)realloc(data, current_heap_size * sizeof(double));
					if (new_data == NULL) {
						free(data);
						data = NULL;
						throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 2, current_heap_size * sizeof(double));
					}
					data = new_data;
				}

				data[n - 1] = d_value;

				line_number += 1;
			}
		}

		n_filtered = n;
		n_total = line_number - 2;

		if (line_length == 0) {
			throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 7, line_number, gwafile->get_descriptor()->get_name());
		}

		reader.reset();
		if (reader.read_line() <= 0) {
			throw FormatterException("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 5, line_number, gwafile->get_descriptor()->get_name());
		}
	} catch (DescriptorException &e) {
		FormatterException new_e(e);
		new_e.add_message("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 9, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (ReaderException &e) {
		FormatterException new_e(e);
		new_e.add_message("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 9, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (FormatterException &e) {
		e.add_message("Formatter", "double calculate_lambda( int& , int& )", __LINE__, 9, gwafile->get_descriptor()->get_name());
		throw;
	}

	if (n > 0) {
		for (int i = 0; i < n; i++) {
			data[i] = pow(Rf_qnorm5(0.5 * data[i], 0.0, 1.0, 0, 0), 2.0);
		}

		qsort(data, n, sizeof(double), auxiliary::dblcmp);

		lambda =  auxiliary::stats_median_from_sorted_data(data, n) / Rf_qchisq(0.5, 1.0, 0, 0);
	}

	free(data);
	data = NULL;

	return lambda;
}

void Formatter::format(double lambda, char new_separator, int& n_total, int& n_filtered) throw (FormatterException) {
	Descriptor* descriptor = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 2;
	char data_separator = '\0';

	const char* output_prefix = NULL;
	const char* file_name = NULL;
	char* o_gwafile_name = NULL;

	vector<double>* snp_hq = NULL;
	double maf_filter_value = -numeric_limits<double>::infinity();
	double oevar_imp_filter_value = -numeric_limits<double>::infinity();
	bool maf_filter = false;
	bool oevar_imp_filter = false;

	Column* column = NULL;

	ofstream ofile_stream;

	double d_value = 0.0;

	n_total = 0;
	n_filtered = 0;

	if (gwafile == NULL) {
		return;
	}

	try {
		descriptor = gwafile->get_descriptor();
		output_prefix = descriptor->get_property(Descriptor::PREFIX);
		file_name = descriptor->get_name();
		snp_hq = descriptor->get_threshold(Descriptor::SNP_HQ);
		data_separator = gwafile->get_data_separator();

		auxiliary::transform_file_name(&o_gwafile_name, output_prefix, file_name, NULL, true);
		if (o_gwafile_name == NULL) {
			throw FormatterException("Formatter", "format( double , char , int& , int& )", __LINE__, 10);
		}

		if (((maf_filter_value = snp_hq->at(0)) > 0) && (maf_column_pos >= 0)){
			maf_filter = true;
		}

		if (((oevar_imp_filter_value = snp_hq->at(1)) > 0) && (oevar_imp_column_pos >= 0)) {
			oevar_imp_filter = true;
		}

		for (columns_it = input_columns.begin(); columns_it != input_columns.end(); columns_it++) {
			output_columns.push_back(*columns_it);
		}

		if (!ISNAN(lambda)) {
			if (stderr_column != NULL) {
				column = new CorrectedStandardErrorColumn(stderr_column, lambda);
				column->set_header("%s_gc", stderr_column->get_header());
				column->set_order(descriptor->get_column_order(column->get_header(), gwafile->is_case_sensitive()));
				output_columns.push_back(column);
			}

			if (pvalue_column != NULL) {
				column = new CorrectedPvalueColumn(pvalue_column, lambda);
				column->set_header("%s_gc", pvalue_column->get_header());
				column->set_order(descriptor->get_column_order(column->get_header(), gwafile->is_case_sensitive()));
				output_columns.push_back(column);
			}
		}

		if ((oevar_imp_column != NULL) && (n_total_column != NULL)) {
			column = new EffectiveSampleSizeColumn(n_total_column, oevar_imp_column);
			column->set_header("%s_effective", n_total_column->get_header());
			column->set_order(descriptor->get_column_order(column->get_header(), gwafile->is_case_sensitive()));
			output_columns.push_back(column);
		}

		if (gwafile->is_order_on()) {
			if (descriptor->get_reordered_columns_number() > 0) {
				stable_sort(output_columns.begin(), output_columns.end(), compare_columns);
			} else {
				stable_sort(output_columns.begin(), output_columns.end(), compare_columns_by_name);
			}
		}

		ofile_stream.exceptions(ios_base::failbit | ios_base::badbit);
		ofile_stream.precision(numeric_limits<double>::digits10);

		try {
			ofile_stream.open(o_gwafile_name);
		} catch (ofstream::failure &e) {
			throw FormatterException("Formatter", "format( double , char , int& , int& )", __LINE__, 11, o_gwafile_name);
		}

		try {
			columns_it = output_columns.begin();
			if (columns_it != output_columns.end()) {
				ofile_stream << (*columns_it)->get_header();
				while (++columns_it != output_columns.end()) {
					ofile_stream << new_separator << (*columns_it)->get_header();
				}
				ofile_stream << endl;
			}

			if (maf_filter) {
				if (oevar_imp_filter) {
					// all filters
					while ((line_length = reader.read_line()) > 0) {
						line = *reader.line;

						for (columns_it = input_columns.begin(); columns_it != input_columns.end(); (*columns_it)->char_value = auxiliary::strtok(&line, data_separator), columns_it++);

						d_value = maf_column->get_numeric_value();
						d_value = d_value > 0.5 ? 1.0 - d_value : d_value;
						if (auxiliary::fcmp(d_value, maf_filter_value, EPSILON) == 1) {
							d_value = oevar_imp_column->get_numeric_value();
							if (auxiliary::fcmp(d_value, oevar_imp_filter_value, EPSILON) == 1) {
								columns_it = output_columns.begin();
								if (columns_it != output_columns.end()) {
									(*columns_it)->out(ofile_stream);
									while (++columns_it != output_columns.end()) {
										ofile_stream << new_separator;
										(*columns_it)->out(ofile_stream);
									}
									ofile_stream << endl;
								}

								n_filtered += 1;
							}
						}

						line_number += 1;
					}
				} else {
					// only maf filter
					while ((line_length = reader.read_line()) > 0) {
						line = *reader.line;

						for (columns_it = input_columns.begin(); columns_it != input_columns.end(); (*columns_it)->char_value = auxiliary::strtok(&line, data_separator), columns_it++);

						d_value = maf_column->get_numeric_value();
						d_value = d_value > 0.5 ? 1.0 - d_value : d_value;
						if (auxiliary::fcmp(d_value, maf_filter_value, EPSILON) == 1) {
							columns_it = output_columns.begin();
							if (columns_it != output_columns.end()) {
								(*columns_it)->out(ofile_stream);
								while (++columns_it != output_columns.end()) {
									ofile_stream << new_separator;
									(*columns_it)->out(ofile_stream);
								}
								ofile_stream << endl;
							}

							n_filtered += 1;
						}

						line_number += 1;
					}
				}
			} else if (oevar_imp_filter) {
				// only oevar_imp filter
				while ((line_length = reader.read_line()) > 0) {
					line = *reader.line;

					for (columns_it = input_columns.begin(); columns_it != input_columns.end(); (*columns_it)->char_value = auxiliary::strtok(&line, data_separator), columns_it++);

					d_value = oevar_imp_column->get_numeric_value();
					if (auxiliary::fcmp(d_value, oevar_imp_filter_value, EPSILON) == 1) {
						columns_it = output_columns.begin();
						if (columns_it != output_columns.end()) {
							(*columns_it)->out(ofile_stream);
							while (++columns_it != output_columns.end()) {
								ofile_stream << new_separator;
								(*columns_it)->out(ofile_stream);
							}
							ofile_stream << endl;
						}

						n_filtered += 1;
					}

					line_number += 1;
				}
			} else {
				// no filters
				while ((line_length = reader.read_line()) > 0) {
					line = *reader.line;

					for (columns_it = input_columns.begin(); columns_it != input_columns.end(); (*columns_it)->char_value = auxiliary::strtok(&line, data_separator), columns_it++);

					columns_it = output_columns.begin();
					if (columns_it != output_columns.end()) {
						(*columns_it)->out(ofile_stream);
						while (++columns_it != output_columns.end()) {
							ofile_stream << new_separator;
							(*columns_it)->out(ofile_stream);
						}
						ofile_stream << endl;
					}

					n_filtered += 1;

					line_number += 1;
				}
			}
		} catch (ofstream::failure &e) {
			throw FormatterException("Formatter", "format( double , char , int& , int& )", __LINE__, 13, o_gwafile_name);
		}

		try {
			ofile_stream.close();
		} catch (ofstream::failure &e) {
			throw FormatterException("Formatter", "format( double , char , int& , int& )", __LINE__, 12, o_gwafile_name);
		}

		n_total = line_number - 2;

		if (line_length == 0) {
			throw FormatterException("Formatter", "format( double , char , int& , int& )", __LINE__, 7, line_number, gwafile->get_descriptor()->get_name());
		}
	} catch (DescriptorException &e) {
		FormatterException new_e(e);
		e.add_message("Formatter", "format( double , char , int& , int& )", __LINE__, 14, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (ColumnException &e) {
		FormatterException new_e(e);
		e.add_message("Formatter", "format( double , char , int& , int& )", __LINE__, 14, gwafile->get_descriptor()->get_name());
		throw new_e;
	} catch (FormatterException &e) {
		e.add_message("Formatter", "format( double , char , int& , int& )", __LINE__, 14, gwafile->get_descriptor()->get_name());
		throw;
	}
}
