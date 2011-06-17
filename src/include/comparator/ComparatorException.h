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
 * Date: 03-06-2010
 *
 * Description:
 * Class ComparatorException represents the specific excepions for the class Comparator.
 *
 */

#ifndef COMPARATOREXCEPTION_H_
#define COMPARATOREXCEPTION_H_

#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

class ComparatorException: public exception {
private:
	static const char* MESSAGES[];

	const char* method;
	const char* argument;
	char* value;

	int message;

public:
	ComparatorException(int message);
	ComparatorException(int message, const char* value);
	ComparatorException(int message, int value);
	ComparatorException(int message, const char* method, const char* argument, const char* value);
	ComparatorException(int message, const char* method, const char* argument, int value);
	virtual ~ComparatorException() throw();
	virtual const char* what() const throw();
};

#endif
