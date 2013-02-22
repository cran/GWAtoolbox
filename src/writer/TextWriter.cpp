#include "include/TextWriter.h"

const unsigned int TextWriter::DEFAULT_BUFFER_SIZE = 16777216;

TextWriter::TextWriter(unsigned int buffer_size) throw (WriterException) : buffer(NULL) {
	buffer = (char*)malloc((buffer_size + 1u) * sizeof(char));
	if (buffer == NULL) {
		throw WriterException("TextWriter", "TextWriter( unsigned int )", __LINE__, 2, (buffer_size + 1u) * sizeof(char));
	}

	buffer[0] = '\0';
}

TextWriter::~TextWriter() {
	free(buffer);
	buffer = NULL;
}

void TextWriter::open() throw (WriterException) {
	if (ofile_stream.is_open()) {
		close();
	}

	ofile_stream.clear();
	ofile_stream.open(file_name, ios::binary);

	if (ofile_stream.fail()) {
		throw WriterException("TextWriter", "open()", __LINE__, 3, file_name);
	}
}

void TextWriter::close() throw (WriterException) {
	if (ofile_stream.is_open()) {
		ofile_stream.clear();
		ofile_stream.close();

		if (ofile_stream.fail()) {
			throw WriterException("TextWriter", "close()", __LINE__, 5, file_name);
		}
	}
}

void TextWriter::write(const char* format, ...) throw (WriterException) {
	va_list arguments;

	va_start(arguments, format);
	if (vsprintf(buffer, format, arguments) < 0) {
		throw WriterException("TextWriter", "write( const char* )", __LINE__, 4, file_name);
	}
	va_end(arguments);

	ofile_stream << buffer;

	if (ofile_stream.fail()) {
		throw WriterException("TextWriter", "write( const char* )", __LINE__, 4, file_name);
	}
}
