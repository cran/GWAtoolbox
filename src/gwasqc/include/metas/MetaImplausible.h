/*
 * Copyright � 2011 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
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

#ifndef METAIMPLAUSIBLE_H_
#define METAIMPLAUSIBLE_H_

#include "MetaNumeric.h"

class MetaImplausible: public MetaNumeric {
private:
	double less;
	double greater;

	int n_less;
	int n_greater;

public:
	MetaImplausible(unsigned int heap_size = Meta::HEAP_SIZE) throw (MetaException);
	MetaImplausible(double less, double greater, unsigned int heap_size = Meta::HEAP_SIZE) throw (MetaException);
	virtual ~MetaImplausible();
	void put(char* value) throw (MetaException);
	void print(ostream& stream);
	void print_html(ostream& stream, char path_separator);
	void set_implausible(double less, double greater);
};

#endif
