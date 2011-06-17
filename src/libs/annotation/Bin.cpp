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

#include "../../include/annotation/Bin.h"

Bin::Bin() : start(INT_MAX), end(0) {

}

Bin::~Bin() {
	snps.clear();
	genes.clear();
}

void Bin::set_snp(Snp* snp) {
	snps.push_back(snp->marker);

	if (snp->position < start) {
		start = snp->position;
	}

	if (snp->position > end) {
		end = snp->position;
	}
}
