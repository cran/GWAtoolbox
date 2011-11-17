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

#ifndef BITARRAYEXCEPTION_H_
#define BITARRAYEXCEPTION_H_

#include "../../exception/include/Exception.h"

class BitArrayException : public Exception {
private:
	static const int MESSAGE_TEMPLATES_NUMBER;
	static const char* MESSAGE_TEMPLATES[];

protected:
	const char* get_message_template(int message_template_index);

public:
	BitArrayException();
	BitArrayException(int message_template_index, ... );
	BitArrayException(const char* class_name, const char* method_name, int source_line);
	BitArrayException(const char* class_name, const char* method_name, int source_line, int message_template_index, ... );
	BitArrayException(const Exception& exception);

	virtual ~BitArrayException() throw();
};

#endif
