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

#include "../../include/plots/Plot.h"

const unsigned int Plot::STR_BUFFER_SIZE = 8000;

Plot::Plot() : name(NULL), title(NULL) {

}

Plot::~Plot() {
	histograms.clear();
	boxplots.clear();
	qqplots.clear();

	free(name);
	free(title);

	name = NULL;
	title = NULL;
}

void Plot::add_histogram(Histogram* histogram) throw (PlotException) {
	if (histogram == NULL) {
		throw PlotException("Plot", "add_histogram( Histogram* )", __LINE__, 0, "histogram");
	}

	histograms.push_back(histogram);
}

void Plot::add_boxplot(Boxplot* boxplot) throw (PlotException) {
	if (boxplot == NULL) {
		throw PlotException("Plot", "add_boxplot( Boxplot* )", __LINE__, 0, "boxplot");
	}

	boxplots.push_back(boxplot);
}

void Plot::add_qqplot(Qqplot* qqplot) throw (PlotException) {
	if (qqplot == NULL) {
		throw PlotException("Plot", "add_qqplot( Qqplot* )", __LINE__, 0, "qqplot");
	}

	qqplots.push_back(qqplot);
}

void Plot::set_name(const char* name) throw (PlotException) {
	if (name == NULL) {
		throw PlotException("Plot", "set_name( const char* )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Plot", "set_name( const char* )", __LINE__, 1, "name");
	}

	free(this->name);
	this->name = NULL;
	this->name = (char*)malloc((strlen(name) + 1) * sizeof(char));
	if (this->name == NULL) {
		throw PlotException("Plot", "set_name( const char* )", __LINE__, 2, (strlen(name) + 1) * sizeof(char));
	}

	strcpy(this->name, name);
}

void Plot::set_formatted_name(const char* name, ... ) throw (PlotException) {
	va_list arguments;
	int status = 0;

	if (name == NULL) {
		throw PlotException("Plot", "set_formatted_name( const char* , ... )", __LINE__, 0, "name");
	}

	if (strlen(name) <= 0) {
		throw PlotException("Plot", "set_formatted_name( const char* , ... )", __LINE__, 1, "name");
	}

	free(this->name);
	this->name = NULL;
	this->name = (char*)malloc(STR_BUFFER_SIZE * sizeof(char));
	if (this->name == NULL) {
		throw PlotException("Plot", "set_formatted_name( const char* , ... )", __LINE__, 2, STR_BUFFER_SIZE * sizeof(char));
	}

	va_start(arguments, name);
	status = vsprintf(this->name, name, arguments);
	if (status < 0) {
		free(this->name);
		this->name = NULL;
		throw PlotException("Plot", "set_formatted_name( const char*, ... )", __LINE__, 4);
	}
	va_end(arguments);
}

const char* Plot::get_name() {
	return name;
}

void Plot::set_title(const char* title) throw (PlotException) {
	if (title == NULL) {
		throw PlotException("Plot", "set_title( const char* )", __LINE__, 0, "title");
	}

	if (strlen(title) <= 0) {
		throw PlotException("Plot", "set_title( const char* )", __LINE__, 1, "title");
	}

	free(this->title);
	this->title = NULL;
	this->title = (char*)malloc((strlen(title) + 1) * sizeof(char));
	if (this->title == NULL) {
		throw PlotException("Plot", "set_title( const char* )", __LINE__, 2, (strlen(title) + 1) * sizeof(char));
	}

	strcpy(this->title, title);
}

void Plot::set_formatted_title(const char* title, ... ) throw (PlotException) {
	va_list arguments;
	int status = 0;

	if (title == NULL) {
		throw PlotException("Plot", "set_formatted_title( const char* , ... )", __LINE__, 0, "title");
	}

	if (strlen(title) <= 0) {
		throw PlotException("Plot", "set_formatted_title( const char* , ... )", __LINE__, 1, "title");
	}

	free(this->title);
	this->title = NULL;
	this->title = (char*)malloc(STR_BUFFER_SIZE * sizeof(char));
	if (this->title == NULL) {
		throw PlotException("Plot", "set_formatted_title( const char* , ... )", __LINE__, 2, STR_BUFFER_SIZE * sizeof(char));
	}

	va_start(arguments, title);
	status = vsprintf(this->title, title, arguments);
	if (status < 0) {
		free(this->name);
		this->name = NULL;
		throw PlotException("Plot", "set_formatted_title( const char* , ... )", __LINE__, 4);
	}
	va_end(arguments);
}

const char* Plot::get_title() {
	return title;
}

int Plot::histograms_count() {
	return histograms.size();
}

int Plot::boxplots_count() {
	return boxplots.size();
}

int Plot::qqplots_count() {
	return qqplots.size();
}

vector<Histogram*>* Plot::get_histograms() {
	return &histograms;
}

vector<Boxplot*>* Plot::get_boxplots() {
	return &boxplots;
}

vector<Qqplot*>* Plot::get_qqplots() {
	return &qqplots;
}
