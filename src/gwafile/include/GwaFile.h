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

#ifndef GWAFILE_H_
#define GWAFILE_H_

#include "../../descriptor/include/Descriptor.h"
#include "GwaFileException.h"

class GwaFile {
private:
	Descriptor* descriptor;

	bool casesensitive;
	bool high_verbosity;
	bool order_on;
	bool gc_on;
	double inflation_factor;
	char header_separator;
	char data_separator;
	unsigned long int estimated_size;

public:
	GwaFile(Descriptor* descriptor) throw (GwaFileException);
	GwaFile(Descriptor* descriptor, void (GwaFile::*check_functions[])(Descriptor*), unsigned int check_functions_number) throw (GwaFileException);
	virtual ~GwaFile();

	Descriptor* get_descriptor();
	bool is_case_sensitive();
	bool is_high_verbosity();
	bool is_order_on();
	bool is_gc_on();
	double get_inflation_factor();
	char get_header_separator();
	char get_data_separator();
	unsigned long int get_estimated_size();

	void check_filters(Descriptor* descriptor) throw (GwaFileException);
	void check_thresholds(Descriptor* descriptor) throw (GwaFileException);
	void check_prefix(Descriptor* descriptor) throw (GwaFileException);
	void check_casesensitivity(Descriptor* descriptor) throw (GwaFileException);
	void check_verbosity_level(Descriptor* descriptor) throw (GwaFileException);
	void check_missing_value(Descriptor* descriptor) throw (GwaFileException);
	void check_separators(Descriptor* descriptor) throw (GwaFileException);
	void check_filesize(Descriptor* descriptor) throw (GwaFileException);
	void check_order(Descriptor* descriptor) throw (GwaFileException);
	void check_genomiccontrol(Descriptor* descriptor) throw (GwaFileException);

};

#endif
