#ifndef GZIPWRITER_H_
#define GZIPWRITER_H_

#include "Writer.h"
#include "../../zlib/zlib.h"

using namespace std;

class GzipWriter : public Writer {
private:
	gzFile outfile;

	char* buffer;

public:
	static const unsigned int DEFAULT_BUFFER_SIZE;

	GzipWriter(unsigned int buffer_size = DEFAULT_BUFFER_SIZE) throw (WriterException);
	virtual ~GzipWriter();

	void open() throw (WriterException);
	void close() throw (WriterException);
	void write(const char* format, ...) throw (WriterException);
};

#endif
