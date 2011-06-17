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

/*
 * Author: Daniel Taliun
 * Date: 14-12-2009
 *
 * Description:
 * Class FileException represents the specific excepions for the File entities.
 *
 */

#ifndef FILEEXCEPTION_H_
#define FILEEXCEPTION_H_

#include <exception>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

class FileException: public exception {
private:
	static const char* MESSAGES[];

	FileException* source;

	const char* file;
	const char* method;
	const char* argument;
	char* value;

	int message;

public:
	FileException(int message);
	FileException(int message, const char* file, const char* method, const char* argument, const char* value);
	virtual ~FileException() throw();
	void set_source(FileException& cause);
	FileException* get_source();
	virtual const char* what() const throw();
};

#endif

