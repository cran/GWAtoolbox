#include "include/GzipWriter.h"

const unsigned int GzipWriter::DEFAULT_BUFFER_SIZE = 16777216;

GzipWriter::GzipWriter(unsigned int buffer_size) throw (WriterException) : buffer(NULL) {
	buffer = (char*)malloc((buffer_size + 1u) * sizeof(char));
	if (buffer == NULL) {
		throw WriterException("GzipWriter", "GzipWriter( unsigned int )", __LINE__, 2, (buffer_size + 1u) * sizeof(char));
	}

	buffer[0] = '\0';
}

GzipWriter::~GzipWriter() {
	free(buffer);
	buffer = NULL;
}

void GzipWriter::open() throw (WriterException) {
	outfile = gzopen(file_name, "wb");
	if (outfile == NULL) {
		throw WriterException("GzipWriter", "open()", __LINE__, 3, file_name);
	}
}

void GzipWriter::close() throw (WriterException) {
	int gzerrno = 0;

	gzerrno = gzclose(outfile);
	if (gzerrno != Z_OK) {
		throw WriterException("GzipWriter", "close()", __LINE__, 5, file_name);
	}
}

void GzipWriter::write(const char* format, ...) throw (WriterException) {
	va_list arguments;

	va_start(arguments, format);
	if (vsprintf(buffer, format, arguments) < 0) {
		throw WriterException("GzipWriter", "write( const char* )", __LINE__, 4, file_name);
	}
	va_end(arguments);

	if (gzputs(outfile, buffer) < 0) {
		throw WriterException("GzipWriter", "write( cosnt char* )", __LINE__, 4, file_name);
	}
}
