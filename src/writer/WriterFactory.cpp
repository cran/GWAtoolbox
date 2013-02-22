#include "include/WriterFactory.h"

const char* WriterFactory::TEXT = "TEXT";
const char* WriterFactory::GZIP = "GZIP";

WriterFactory::WriterFactory() {

}

WriterFactory::~WriterFactory() {

}

Writer* WriterFactory::create(const char* type) throw (WriterException) {
	if (auxiliary::strcmp_ignore_case(type, TEXT) == 0) {
		return new TextWriter();
	} else if (auxiliary::strcmp_ignore_case(type, GZIP) == 0) {
		return new GzipWriter();
	} else {
		throw WriterException("WriterFactor", "create( const char* )", __LINE__, 6, type);
	}
}
