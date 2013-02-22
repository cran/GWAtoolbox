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

#include "include/TextReader.h"

const unsigned int TextReader::DEFAULT_BUFFER_SIZE = 16777216;
const unsigned int TextReader::TOP_ROWS_NUMBER = 10;
const unsigned int TextReader::ROWS_SAMPLE_SIZE = 30;
const unsigned int TextReader::ROWS_SAMPLE_COUNT = 15;

TextReader::TextReader(unsigned int buffer_size) throw (ReaderException) : Reader(&buffer),
	buffer_size(buffer_size), buffer(NULL) {
	if (buffer_size <= 0) {
		throw ReaderException("TextReader", "TextReader( int )", __LINE__, 1, "buffer_size");
	}

	buffer = (char*)malloc((buffer_size + 1) * sizeof(char));
	if (buffer == NULL) {
		throw ReaderException("TextReader", "TextReader( int )", __LINE__, 2, (buffer_size + 1) * sizeof(char));
	}

	buffer[0] = '\0';
}

TextReader::~TextReader() {
	buffer_size = 0;

	free(buffer);
	buffer = NULL;
}


void TextReader::open() throw (ReaderException) {
	if (ifile_stream.is_open()) {
		close();
	}

	ifile_stream.clear();
	ifile_stream.open(file_name, ios::binary);

	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "open()", __LINE__, 3, file_name);
	}
}

void TextReader::close() throw (ReaderException) {
	if (ifile_stream.is_open()) {
		ifile_stream.clear();
		ifile_stream.close();

		if (ifile_stream.fail()) {
			throw ReaderException("TextReader", "open()", __LINE__, 5, file_name);
		}
	}

	buffer[0] = '\0';
}

int TextReader::read_line() throw (ReaderException) {
	int i = 0;
	int c = 0;

	while ((i < buffer_size) && ((c = ifile_stream.get()) != ifstream::traits_type::eof())) {
		buffer[i] = (char)c;

		if (buffer[i] == '\n') {
			buffer[i] = '\0';
			return i;
		} else if (buffer[i] == '\r') {
			buffer[i] = '\0';
			if ((c = ifile_stream.peek()) != ifstream::traits_type::eof()) {
				if ((char)c == '\n') {
					ifile_stream.ignore();
				}
			}
			return i;
		}

		i += 1;
	}

	buffer[i] = '\0';

	if (!ifile_stream.eof() && ifile_stream.fail()) {
		throw ReaderException("TextReader", "int read_line()", __LINE__, 4, file_name);
	}

	return (i == 0 ? -1 : i);
}

void TextReader::reset() throw (ReaderException) {
	if (ifile_stream.is_open()) {
		ifile_stream.clear();
		ifile_stream.seekg((streampos)0, ifstream::beg);

		if (ifile_stream.fail()) {
			throw ReaderException("TextTableReader", "reset()", __LINE__, 6, file_name);
		}
	}
}

bool TextReader::eof() {
	return ifile_stream.eof();
}

bool TextReader::sof() {
	return ifile_stream.tellg() == std::ifstream::pos_type(0);
}

bool TextReader::is_open() {
	return ifile_stream.is_open();
}

bool TextReader::is_compressed() {
	return false;
}

int TextReader::get_buffer_size() {
	return buffer_size;
}

unsigned long int TextReader::estimate_lines_count() throw (ReaderException) {
	long int file_length = 0;
	int header_length = 0;
	int c = 0;

	int denominator = ROWS_SAMPLE_SIZE * 100;
	double estimated_location = 0.0;
	long int file_position = 0;
	int chars_count = 0;
	int lines_count = 0;

	multiset<int> line_lengths;
	multiset<int>::iterator line_lengths_it;
	vector<double> sample_means;
	double sample_mean = 0.0;
	double mean = 0.0;

	unsigned int estimated_lines_count = 0;

	if (ifile_stream.is_open()) {
		ifile_stream.close();
	}

	ifile_stream.clear();
	ifile_stream.open(file_name, ios::binary);
	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 3, file_name);
	}

	ifile_stream.seekg((streampos)0, ifstream::end);
	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 6, file_name);
	}

	file_length = ifile_stream.tellg();
	if (file_length < 0) {
		ifile_stream.clear();
		ifile_stream.close();
		if (ifile_stream.fail()) {
			throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 5, file_name);
		}

		return 0;
	}

	ifile_stream.seekg((streampos)0, ifstream::beg);
	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 6, file_name);
	}

	while ((c = ifile_stream.get()) != ifstream::traits_type::eof()) {
		header_length += 1;
		if ((char)c == '\n') {
			break;
		} else if ((char)c == '\r') {
			if ((c = ifile_stream.peek()) != ifstream::traits_type::eof()) {
				if ((char)c == '\n') {
					ifile_stream.ignore();
				}
			}
			break;
		}
	}

	if ((!ifile_stream.eof()) && ifile_stream.fail()) {
		throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 4, file_name);
	}

	if (header_length < file_length) {
		srand(time(NULL));

		for (unsigned int i = 0; i < ROWS_SAMPLE_COUNT; i++) {
			for (unsigned int j = 0; j < ROWS_SAMPLE_SIZE; j++) {
				estimated_location = (rand() % denominator) / (double)denominator;
				file_position = (long int)(file_length * estimated_location);

				ifile_stream.seekg((streampos)file_position, ifstream::beg);
				if (ifile_stream.fail()) {
					throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 6, file_name);
				}

				while ((c = ifile_stream.get()) != ifstream::traits_type::eof()) {
					if ((char)c == '\n') {
						break;
					} else if ((char)c == '\r') {
						if ((c = ifile_stream.peek()) != ifstream::traits_type::eof()) {
							if ((char)c == '\n') {
								ifile_stream.ignore();
							}
						}
						break;
					}
				}

				while ((c = ifile_stream.get()) != ifstream::traits_type::eof()) {
					chars_count += 1;
					if ((char)c == '\n') {
						line_lengths.insert(chars_count);
						chars_count = 0;
						break;
					} else if ((char)c == '\r') {
						if ((c = ifile_stream.peek()) != ifstream::traits_type::eof()) {
							if ((char)c == '\n') {
								ifile_stream.ignore();
							}
						}
						line_lengths.insert(chars_count);
						chars_count = 0;
						break;
					}
				}

				if (ifile_stream.eof()) {
					ifile_stream.clear();

					if (chars_count > 0) {
						line_lengths.insert(chars_count);
						chars_count = 0;
					}
				} else if (ifile_stream.fail()) {
					throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 4, file_name);
				}
			}

			lines_count = line_lengths.size();

			if (lines_count > 0) {
				line_lengths_it = line_lengths.begin();
				while (line_lengths_it != line_lengths.end()) {
					sample_mean += (*line_lengths_it);
					line_lengths_it++;
				}
				sample_mean = sample_mean / lines_count;
				sample_means.push_back(sample_mean);
				sample_mean = 0.0;
				line_lengths.clear();
			}
		}

		for (unsigned int i = 0; i < sample_means.size(); i++) {
			mean += sample_means.at(i);
		}
		mean = mean / sample_means.size();

		estimated_lines_count =  (long int)ceil((file_length - header_length) / mean);
	}

	ifile_stream.clear();
	ifile_stream.close();
	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "unsigned int estimate_lines_count()", __LINE__, 5, file_name);
	}

	return estimated_lines_count;
}

void TextReader::detect_field_separators(char* header_separator, char* data_separator) throw (ReaderException) {
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
		throw ReaderException("TextReader", "detect_field_separators( char*, char* )", __LINE__, 0, "header_separator");
	}

	if (data_separator == NULL) {
		throw ReaderException("TextReader", "detect_field_separators( char*, char* )", __LINE__, 0, "data_separator");
	}

	if (ifile_stream.is_open()) {
		ifile_stream.close();
	}

	ifile_stream.clear();
	ifile_stream.open(file_name, ios::binary);
	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "detect_field_separators( char*, char* )", __LINE__, 3, file_name);
	}

	ifile_stream.seekg((streampos)0, ifstream::beg);
	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "detect_field_separators( char*, char* )", __LINE__, 6, file_name);
	}

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
		e.add_message("TextReader", "detect_field_separators( char*, char* )", __LINE__, 7, (const char*)file_name);
		throw;
	}

	ifile_stream.clear();
	ifile_stream.close();
	if (ifile_stream.fail()) {
		throw ReaderException("TextReader", "detect_field_separators( char*, char* )", __LINE__, 5, file_name);
	}

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
					throw ReaderException("TextReader", "detect_field_separators( char*, char* )", __LINE__, 8, file_name);
				}
				i += 1;
			}
		} else if (matches > 1) {
			throw ReaderException("TextReader", "detect_field_separators( char*, char* )", __LINE__, 8, file_name);
		} else {
			*header_separator = separators[header_separator_index];
			*data_separator = separators[data_separator_index];
		}
	}
}
