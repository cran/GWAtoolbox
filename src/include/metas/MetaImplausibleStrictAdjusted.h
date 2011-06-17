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

#ifndef METAIMPLAUSIBLESTRICTADJUSTED_H_
#define METAIMPLAUSIBLESTRICTADJUSTED_H_

#include "MetaImplausibleStrict.h"
#include <cmath>
#include "gsl/gsl_math.h"

class MetaImplausibleStrictAdjusted: public MetaImplausibleStrict {
private:
	double threshold;

public:
	MetaImplausibleStrictAdjusted(double less, double greater, double threshold, int heap_size = HEAP_SIZE) throw (MetaException);
	virtual ~MetaImplausibleStrictAdjusted();
	void put(char* value) throw (MetaException);
	void set_threshold(double threshold);
};

#endif
