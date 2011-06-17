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
 * Class Evaluable is a super class for a comparison operators.
 * The comparison operators are used in filters.
 *
 */

#ifndef EVALUABLE_H_
#define EVALUABLE_H_

#include "gsl/gsl_math.h"

#define EPSILON		0.00000001

class Evaluable {
public:
	virtual ~Evaluable() = 0;
	virtual bool evaluate(double value) = 0;
};

#endif
