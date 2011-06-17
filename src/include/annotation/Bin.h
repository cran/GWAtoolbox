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

#ifndef BIN_H_
#define BIN_H_

#include <vector>
#include <limits.h>
#include "Snp.h"

using namespace std;

class Bin {
public:
	int start;
	int end;

	vector<char*> snps;
	vector<char*> genes;

	Bin();
	virtual ~Bin();
	void set_start(int start);
	void set_end(int end);
	void set_snp(Snp* snp);
};

#endif
