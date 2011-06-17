# Copyright © 2011 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
#
# This file is part of GWAtoolbox.
#
# GWAtoolbox is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GWAtoolbox is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GWAtoolbox.  If not, see <http://www.gnu.org/licenses/>.


dispersion_check <- function(script, sample_sizes = NULL, plot = TRUE) {
	script <- as.character(script)

#   filelist <- scan(script, what="character", quiet=TRUE)
#	f <- length(filelist)
#	if (f <= 0) {
#		stop("File '", script, "' is empty.\n")
#	}
	
	file_separator <- NA
	if (.Platform$OS.type == "windows") {
		file_separator <- "\\"
	}
	else {
		file_separator <- .Platform$file.sep
	}
	
	metal <- scan(script, what="character", sep="\n", quiet=T)
	
	prefix <- metal[grepl("^\\s*PREFIX\\s+[[:graph:]]+", metal, ignore.case=T)]
	if (length(prefix) <= 0) {
		stop("File '", script, "' does not contain valid PREFIX command.\n")
	} 
	else if (length(prefix) > 1) {
		stop("File '", script, "' contains more than two valid PREFIX commands.\n")
	}
	r <- regexpr("PREFIX\\s+[[:graph:]]+", prefix, ignore.case=T)
	prefix <- substr(prefix, r, r + attr(r, "match.length") - 1)
	prefix <- strsplit(prefix, split="\\s+")[[1]][2]
	
	process <- metal[grepl("^\\s*PROCESS\\s+([[:graph:]]+)(\\s+[[:graph:]]+)?", metal, ignore.case=T)]
	if (length(process) <= 1) {
		stop("File '", script,"' does not contain any valid PROCESS command.\n")
	}
	r <- regexpr("PROCESS\\s+([[:graph:]]+)(\\s+[[:graph:]]+)?", process, ignore.case=T)
	process <- substr(process, r, r + attr(r, "match.length") - 1)
	
	files <- data.frame(full_name = rep(NA, length(process)), short_name=rep(NA, length(process)), output_name=rep(NA, length(process)))
	for (i in 1:length(process)) {
		tokens <- strsplit(process[i], split="\\s+")[[1]]
		
		files$full_name[i] <- tokens[2]
		if (length(tokens) >= 3) {
			files$short_name[i] <- tokens[3]
			files$output_name[i] <- paste(prefix, files$short_name[i], ".csv", sep="")
		}
		else {
			files$output_name[i] <- tail(strsplit(files$full_name[i], split=file_separator, fixed=T)[[1]], 1)
			if (grepl("(\\.csv|\\.txt|\\.html|\\.htm)$", files$output_name[i], ignore.case=T)) {
				files$output_name[i] <- sub("(\\.csv|\\.txt|\\.html|\\.htm)$", "", files$output_name[i], ignore.case=T)
			}
			files$output_name[i] <- paste(prefix, files$output_name[i], ".csv", sep="")	
		}
	}

	if (!is.null(sample_sizes)) {
		if (!is.vector(sample_sizes, mode="numeric")) {
			stop("Argument 'sample_sizes' must be a numeric vector of positive values.")
		}
		sample_sizes<-sample_sizes[!is.na(sample_sizes) & sample_sizes > 0]
#		if (length(sample_sizes) != f) {
		if (length(sample_sizes) != nrow(files)) {
			stop("The length of vector 'sample_sizes' must be equal to the number of studies in 'summary_file'.")
		}
	}

	if (!is.logical(plot)) {
		stop("Argument 'plot' must be a logical.")
	}

    names <- c("study", "mean_se", "median_n");
	data <- data.frame(study=NA, mean_se=NA, median_n=NA)
	
	if (!is.null(sample_sizes)) {
		for (i in 1:nrow(files)) {
			x <- read.table(files$output_name[i], sep=";", header=T, stringsAsFactors=F)
			if (!is.na(files$short_name[i])) {
				data[i, 1] <- files$short_name[i]
			}
			else {
				data[i, 1] <- files$output_name[i]
			}
#		for (i in 1:f) {
#			x <- read.table(filelist[i], sep=";", header=T, stringsAsFactors=F)
#			data[i, 1] <- filelist[i]	
			data[i, 2] <- x$STDERR[4]
			data[i, 3] <- sample_sizes[i]
		}
	}
	else {
		for (i in 1:nrow(files)) {
			x <- read.table(files$output_name[i], sep=";", header=T, stringsAsFactors=F)
			if (!is.na(files$short_name[i])) {
				data[i, 1] <- files$short_name[i]
			}
			else {
				data[i, 1] <- files$output_name[i]
			}
#		for (i in 1:f) {
#			x <- read.table(filelist[i], sep=";", header=T, stringsAsFactors=F)
#			data[i, 1] <- filelist[i]
			data[i, 2] <- x$STDERR[4]
			data[i, 3] <- x$N[8]
		}
	}

	if (plot) {
		par(mfrow = c(1, 1))
		plot(data$median_n, data$mean_se, xlab = "Sample Size", ylab = "Mean of SE(beta)", bg = "orange", pch = 22, cex = 2)
	}

	return(data)
}