#ifndef WRITER_H_
#define WRITER_H_

#include "WriterException.h"
#include <cstdarg>
#include <cstring>

using namespace std;

class Writer {
protected:
	char* file_name;

	Writer();

public:
	virtual ~Writer();

	void set_file_name(const char* file_name) throw (WriterException);
	const char* get_file_name();

	virtual void open() throw (WriterException) = 0;
	virtual void close() throw (WriterException) = 0;
	virtual void write(const char* format, ...) throw (WriterException) = 0;
};

#endif
