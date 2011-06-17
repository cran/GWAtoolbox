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
 * Date: 27-05-2010
 *
 * Description:
 * Class AnalyzerException represents the specific excepions for the class Analyzer.
 *
 */

#ifndef ANALYZEREXCEPTION_H_
#define ANALYZEREXCEPTION_H_

#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

class AnalyzerException: public exception {
private:
	static const char* MESSAGES[];

	const char* method;
	const char* argument;
	char* value;

	int message;

public:
	AnalyzerException(int message);
	AnalyzerException(int message, const char* value);
	AnalyzerException(int message, int value);
	AnalyzerException(int message, const char* method, const char* argument, const char* value);
	AnalyzerException(int message, const char* method, const char* argument, int value);
	virtual ~AnalyzerException() throw();
	virtual const char* what() const throw();
};

#endif
