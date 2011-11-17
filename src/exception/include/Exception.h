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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <list>

using namespace std;

class Exception: public exception {
private:
	static const int MESSAGE_BUFFER_SIZE;

	struct message {
		char* text;
		const char* class_name;
		const char* method_name;
		int source_line;
		message():text(NULL), class_name(NULL), method_name(NULL), source_line(numeric_limits<int>::min()){};
	};

	list<message*> trace;

	void format_message_text(char** text, int message_template_index, va_list arguments);

protected:
	void add_message(int message_template_index, va_list arguments);
	void add_message(const char* class_name, const char* method_name, int source_line, int message_template_index, va_list arguments);
	virtual const char* get_message_template(int message_template_index);

public:
	Exception();
	Exception(int message_template_index, ... );
	Exception(const char* class_name, const char* method_name, int source_line);
	Exception(const char* class_name, const char* method_name, int source_line, int message_template_index, ... );
	Exception(const Exception& exception);
	void add_message(int message_template_index, ... );
	void add_message(const char* class_name, const char* method_name, int source_line);
	void add_message(const char* class_name, const char* method_name, int source_line, int message_template_index, ... );
	virtual ~Exception() throw();
	virtual const char* what() const throw();
};

#endif
