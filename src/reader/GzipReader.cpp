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

#include "include/GzipReader.h"

const unsigned int GzipReader::DEFAULT_BUFFER_SIZE = 16777216;
const unsigned int GzipReader::TOP_ROWS_NUMBER = 10;

GzipReader::GzipReader(unsigned int buffer_size) throw (ReaderException) : Reader(&buffer),
	opened(false), buffer_size(buffer_size), buffer(NULL) {

	buffer = (char*)malloc((buffer_size + 1) * sizeof(char));
	if (buffer == NULL) {
		throw ReaderException("GzipReader", "GzipReader( int )", __LINE__, 2, (buffer_size + 1) * sizeof(char));
	}

	buffer[0] = '\0';
}

GzipReader::~GzipReader() {
	buffer_size = 0;

	free(buffer);
	buffer = NULL;
}

void GzipReader::open() throw (ReaderException) {
	close();

	infile = gzopen(file_name, "rb");
	if (infile == NULL) {
		throw ReaderException("GzipReader", "open()", __LINE__, 3, file_name);
	}

	opened = true;
}

void GzipReader::close() throw (ReaderException) {
	if (opened) {
		int gzerrno = 0;

		gzerrno = gzclose(infile);
		if (gzerrno != Z_OK) {
			throw ReaderException("GzipReader", "open()", __LINE__, 5, file_name);
		}

		opened = false;
	}
}

int GzipReader::read_line() throw (ReaderException) {
	int i = 0;
	int c = 0;

	while ((i < buffer_size) && ((c = gzgetc(infile)) >= 0)) {
		buffer[i] = (char)c;

		if (buffer[i] == '\n') {
			buffer[i] = '\0';
			return i;
		} else if (buffer[i] == '\r') {
			buffer[i] = '\0';
			if ((c = gzgetc(infile)) >= 0) {
				if ((char)c != '\n') {
					c = gzungetc(c, infile);
				}
			}
			return i;
		}

		i += 1;
	}

	buffer[i] = '\0';

	if ((c < 0) && (gzeof(infile) < 1)) {
		throw ReaderException("GzipReader", "int read_line()", __LINE__, 4, file_name);
	}

	return (i == 0 ? -1 : i);
}

void GzipReader::reset() throw (ReaderException) {
	if (gzseek(infile, 0L, SEEK_SET) < 0) {
		throw ReaderException("GzipReader", "reset()", __LINE__, 6, file_name);
	}
}

bool GzipReader::eof() {
	return gzeof(infile) > 0;
}

bool GzipReader::sof() {
	return gztell(infile) == 0;
}

bool GzipReader::is_open() {
	return opened;
}

bool GzipReader::is_compressed() {
	return true;
}

int GzipReader::get_buffer_size() {
	return buffer_size;
}

void GzipReader::detect_field_separators(char* header_separator, char* data_separator) throw (ReaderException) {
	const unsigned int separators_number = 4;
	char separators[separators_number] = {',', '\t', ' ' , ';'};

	int header_tokens_number[separators_number];
	int row_tokens_number[separators_number];

	bool header_was_read = false;
	bool data_was_read = false;
	unsigned int rows_read = 0;

	int line_length = 0;
	int tokens_number = 0;
	int char_position = 0;
	unsigned int i = 0, j = 0;
	int header_separator_index = -1;
	int data_separator_index = -1;
	unsigned int matches = 0;

	if (header_separator == NULL) {
		throw ReaderException("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 0, "header_separator");
	}

	if (data_separator == NULL) {
		throw ReaderException("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 0, "data_separator");
	}

	if (opened) {
		if (gzclose(infile) != Z_OK) {
			throw ReaderException("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 5, file_name);
		}
		opened = false;
	}

	if ((infile = gzopen(file_name, "rb")) == NULL) {
		throw ReaderException("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 3, file_name);
	}
	opened = true;

	i = 0;
	while (i < separators_number) {
		header_tokens_number[i] = 0;
		row_tokens_number[i] = 0;
		i += 1;
	}

	try {
		while ((!header_was_read) && ((line_length = read_line()) >= 0)) {
			if (line_length > 0) {
				header_was_read = true;

				i = 0;
				while (i < separators_number) {
					tokens_number = 0;
					char_position = 0;
					while (char_position < line_length) {
						if ((*line)[char_position] == separators[i]) {
							tokens_number += 1;
						}
						char_position += 1;
					}
					header_tokens_number[i] = tokens_number;
					i += 1;
				}
			}
		}

		while ((!data_was_read) && ((line_length = read_line()) >= 0)) {
			if (line_length > 0) {
				data_was_read = true;

				i = 0;
				while (i < separators_number) {
					tokens_number = 0;
					char_position = 0;
					while (char_position < line_length) {
						if ((*line)[char_position] == separators[i]) {
							tokens_number += 1;
						}
						char_position += 1;
					}
					row_tokens_number[i] = tokens_number;
					i += 1;
				}
			}
		}

		while ((rows_read < TOP_ROWS_NUMBER) && ((line_length = read_line()) >= 0)) {
			if (line_length > 0) {
				i = 0;
				while (i < separators_number) {
					tokens_number = 0;
					char_position = 0;
					while (char_position < line_length) {
						if ((*line)[char_position] == separators[i]) {
							tokens_number += 1;
						}
						char_position += 1;
					}

					if ((row_tokens_number[i] != tokens_number) && (row_tokens_number[i] != numeric_limits<int>::min())) {
						row_tokens_number[i] = numeric_limits<int>::min();
					}
					i += 1;
				}
				rows_read += 1;
			}
		}
	} catch (ReaderException &e) {
		e.add_message("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 7, (const char*)file_name);
		throw;
	}

	if (gzclose(infile) != Z_OK) {
		throw ReaderException("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 5, file_name);
	}
	opened = false;

	buffer[0] = '\0';

	if (header_was_read) {
		if (data_was_read) {
			i = 0;
			while (i < separators_number) {
				j = 0;
				while (j < separators_number) {
					if ((header_tokens_number[i] == row_tokens_number[j]) && (header_tokens_number[i] > 0)) {
						header_separator_index = i;
						data_separator_index = j;
						matches += 1;
					}
					j += 1;
				}
				i += 1;
			}
		} else {
			i = 0;
			while (i < separators_number) {
				if (header_tokens_number[i] > 0) {
					header_separator_index = i;
					data_separator_index = i;
					matches += 1;
				}
				i += 1;
			}
		}

		if (matches < 1) {
			i = 0;
			while (i < separators_number) {
				if ((header_tokens_number[i] != 0) || (row_tokens_number[i] != 0)) {
					throw ReaderException("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 8, file_name);
				}
				i += 1;
			}
		} else if (matches > 1) {
			throw ReaderException("GzipReader", "detect_field_separators( char*, char* )", __LINE__, 8, file_name);
		} else {
			*header_separator = separators[header_separator_index];
			*data_separator = separators[data_separator_index];
		}
	}
}
