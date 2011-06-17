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
		throw PlotException(1, "Plot", "add_histogram( ... )", "histogram", "NULL");
	}

	histograms.push_back(histogram);
}

void Plot::add_boxplot(Boxplot* boxplot) throw (PlotException) {
	if (boxplot == NULL) {
		throw PlotException(1, "Plot", "add_boxplot( ... )", "boxplot", "NULL");
	}

	boxplots.push_back(boxplot);
}

void Plot::add_qqplot(Qqplot* qqplot) throw (PlotException) {
	if (qqplot == NULL) {
		throw PlotException(1, "Plot", "add_qqplot( ... )", "qqplot", "NULL");
	}

	qqplots.push_back(qqplot);
}

void Plot::set_name(const char* name) throw (PlotException) {
	if (name == NULL) {
		throw PlotException(1, "Plot", "set_name( ... )", "name", "NULL");
	}

	if (strlen(name) <= 0) {
		throw PlotException(2, "Plot", "set_name( ... )", "name", (const char*)NULL);
	}

	free(this->name);
	this->name = NULL;
	this->name = (char*)malloc((strlen(name) + 1) * sizeof(char));
	if (this->name == NULL) {
		throw PlotException(0, "Plot", "set_name( ... )", "name", name);
	}

	strcpy(this->name, name);
}

const char* Plot::get_name() {
	return name;
}

void Plot::set_title(const char* title) throw (PlotException) {
	if (title == NULL) {
		throw PlotException(1, "Plot", "set_title( ... )", "title", "NULL");
	}

	if (strlen(title) <= 0) {
		throw PlotException(2, "Plot", "set_title( ... )", "title", (const char*)NULL);
	}

	free(this->title);
	this->title = NULL;
	this->title = (char*)malloc((strlen(title) + 1) * sizeof(char));
	if (this->title == NULL) {
		throw PlotException(0, "Plot", "set_title( ... )", "title", title);
	}

	strcpy(this->title, title);
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
