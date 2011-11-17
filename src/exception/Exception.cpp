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

#include "include/Exception.h"

const int Exception::MESSAGE_BUFFER_SIZE = 8000;

Exception::Exception() : exception() {

}

Exception::Exception(int message_template_index, ... ): exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

Exception::Exception(const char* class_name, const char* method_name, int source_line): exception() {
	add_message(class_name, method_name, source_line);
}

Exception::Exception(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ): exception() {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

Exception::Exception(const Exception& exception) {
	list<message*>::const_iterator trace_it;
	message* msg = NULL;
	int text_length = 0;

	trace_it = exception.trace.begin();
	while (trace_it != exception.trace.end()) {
		msg = new message();

		if (((*trace_it)->text != NULL) && ((text_length = strlen((*trace_it)->text)) > 0)) {
			msg->text = (char*)malloc((text_length + 1) * sizeof(char));
			if (msg->text != NULL) {
				strcpy(msg->text, (*trace_it)->text);
			}
		}

		msg->class_name = (*trace_it)->class_name;
		msg->method_name = (*trace_it)->method_name;
		msg->source_line = (*trace_it)->source_line;

		trace.push_back(msg);

		trace_it++;
	}
}

void Exception::format_message_text(char** text, int message_template_index, va_list arguments) {
	const char* message_template = get_message_template(message_template_index);

	if (message_template != NULL) {
		if (*text != NULL) {
			free(*text);
			*text = NULL;
		}

		*text = (char*)malloc(MESSAGE_BUFFER_SIZE * sizeof(char));
		if (*text != NULL) {
			int message_status = 0;
			message_status = vsprintf(*text, message_template, arguments);
			if (message_status < 0) {
				free(*text);
				*text = NULL;
			}
		}
	}
}

const char* Exception::get_message_template(int message_template_index) {
	return NULL;
}

void Exception::add_message(int message_template_index, va_list arguments) {
	message* msg = new message();

	format_message_text(&(msg->text), message_template_index, arguments);

	trace.push_front(msg);
}

void Exception::add_message(const char* class_name, const char* method_name, int source_line, int message_template_index, va_list arguments) {
	message* msg = 	new message();

	format_message_text(&(msg->text), message_template_index, arguments);
	msg->class_name = class_name;
	msg->method_name = method_name;
	msg->source_line = source_line;

	trace.push_front(msg);
}

void Exception::add_message(int message_template_index, ... ) {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(message_template_index, arguments);
	va_end(arguments);
}

void Exception::add_message(const char* class_name, const char* method_name, int source_line) {
	message* msg = new message();

	msg->class_name = class_name;
	msg->method_name = method_name;
	msg->source_line = source_line;

	trace.push_front(msg);
}

void Exception::add_message(const char* class_name, const char* method_name, int source_line, int message_template_index, ... ) {
	va_list arguments;

	va_start(arguments, message_template_index);
	add_message(class_name, method_name, source_line, message_template_index, arguments);
	va_end(arguments);
}

Exception::~Exception() throw() {
	list<message*>::iterator trace_it;

	trace_it = trace.begin();
	while (trace_it != trace.end()) {
		if ((*trace_it)->text != NULL) {
			free((*trace_it)->text);
			(*trace_it)->text = NULL;
		}
		(*trace_it)->class_name = NULL;
		(*trace_it)->method_name = NULL;

		delete (*trace_it);

		trace_it++;
	}

	trace.clear();
}

const char* Exception::what() const throw() {
	stringstream string_stream;
	list<message*>::const_iterator trace_it;

	string_stream << setfill(' ');

	trace_it = trace.begin();
	while (trace_it != trace.end()) {
		if (((*trace_it)->text != NULL) && (strlen((*trace_it)->text) > 0)) {
			string_stream << (*trace_it)->text << endl;
		} else {
			string_stream << "Exception." << endl;
		}

		if (((*trace_it)->class_name != NULL) && (strlen((*trace_it)->class_name) > 0)) {
			string_stream << setw(12) << left << "Class name" << " : " << (*trace_it)->class_name << endl;
		}

		if (((*trace_it)->method_name != NULL) && (strlen((*trace_it)->method_name) > 0)) {
			string_stream << setw(12) << left << "Method name" << " : " << (*trace_it)->method_name << endl;
		}

		if ((*trace_it)->source_line > 0) {
			string_stream << setw(12) << left << "Source line" << " : " << (*trace_it)->source_line << endl;
		}

		trace_it++;
	}

	return string_stream.str().c_str();
}


