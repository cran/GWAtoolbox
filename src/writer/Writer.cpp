#include "include/Writer.h"

Writer::Writer() : file_name(NULL) {

}

Writer::~Writer() {
	free(file_name);
	file_name = NULL;
}

void Writer::set_file_name(const char* file_name) throw (WriterException) {
	free(this->file_name);
	this->file_name = NULL;
	this->file_name = (char*)malloc((strlen(file_name) + 1) * sizeof(char));
	if (this->file_name == NULL) {
		throw WriterException("Writer", "set_file_name( const char* )", __LINE__, 2, (strlen(file_name) + 1) * sizeof(char));
	}
	strcpy(this->file_name, file_name);
}

const char* Writer::get_file_name() {
	return file_name;
}
