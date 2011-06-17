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

#include "../../include/annotation/Snp.h"

Snp::Snp() : marker(NULL), position(0), used(false), prime(false) {

}

Snp::~Snp() {

}

void Snp::set_marker(const char* marker) {
	int length = strlen(marker);
	char* new_marker = NULL;

	if (length > 0) {
		new_marker = (char*)malloc((length + 1) * sizeof(char));
		if (new_marker != NULL) {
			free(this->marker);
			this->marker = new_marker;
			strcpy(this->marker, marker);
		}
	}
}
