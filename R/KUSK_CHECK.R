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

kusk_check <- function(script, worst = c(50, 75, 90, 99, 100), plot = TRUE) {
	script <- as.character(script)

	if (!is.vector(worst, mode="numeric")) {
		stop("Argument 'worst' must be a non-emtpy vector of integer values in (100, 99, 90, 75, 50).")
	}
	worst <- unique(worst)
	worst <- worst[!is.na(worst) & (worst %in% c(100, 99, 90, 75, 50))]
	w <- length(worst) 
	if (w <= 0) {
		stop("Argument 'worst' must be a non-emtpy vector of integer values in (100, 99, 90, 75, 50).")	
	}
	worst <- sort(worst, decreasing=TRUE)

	if (!is.logical(plot)) {
		stop("Argument 'plot' must be a logical.")
	}

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
	
	names <- c("study", c( rbind( paste(rep("sk", w), worst, sep="") , paste(rep("ku", w), worst, sep="") ))) 
	data <- data.frame(rbind(rep(NA, 2 * w + 1)), stringsAsFactors=F)
	names(data) <- names

	for (i in 1:nrow(files)) {
		x <- read.table(files$output_name[i], sep=";", header=T, stringsAsFactors=F)
		if (!is.na(files$short_name[i])) {
			data[i, 1] <- files$short_name[i]	
		}
		else {
			data[i, 1] <- files$output_name[i]	
		}
#	for (i in 1:f) {
#		x <- read.table(filelist[i], sep=";", header=T, stringsAsFactors=F)
#		data[i, 1] <- filelist[i]
		for (j in 1:w) {
			if (worst[j] == 100) {
				data[i, 2*j] <- x$STD_EFFECT_1[9]
				data[i, 2*j + 1] <- x$STD_EFFECT_1[10] 
			}
			else if (worst[j] == 99) {
				data[i, 2*j] <- x$STD_EFFECT_0.99[9]
				data[i, 2*j + 1] <- x$STD_EFFECT_0.99[10] 
			}
			else if (worst[j] == 90) {
				data[i, 2*j] <- x$STD_EFFECT_0.95[9]
				data[i, 2*j + 1] <- x$STD_EFFECT_0.95[10] 
			}
			else if (worst[j] == 75) {
				data[i, 2*j] <- x$STD_EFFECT_0.75[9]
				data[i, 2*j + 1] <- x$STD_EFFECT_0.75[10] 
			} 
			else if (worst[j] == 50) {
				data[i, 2*j] <- x$STD_EFFECT_0.5[9]
				data[i, 2*j + 1] <- x$STD_EFFECT_0.5[10] 
	        }
		}
	}

	if (plot) {
		max.sk <- max(abs(data[, seq(2, 2 * w, 2)]), na.rm=T)
		max.ku <- max(abs(data[, seq(2, 2 * w, 2) + 1]), na.rm=T)

		if (w == 1) {
			par(mfrow = c(1, 1))
		}
		else if (w > 1) {       
			par(mfrow = c(w %/% 2 + w %% 2, 2))
		}

		for (i in 1:w) {
			plot(data[, 2 * i], data[, 2 * i + 1], type = "n", xlim = c(-max.sk, max.sk), ylim = c(-max.ku, max.ku), main = paste(worst[i], "%", sep=""), xlab = "Skewness", ylab = "Kurtosis")
			abline(h=0, v=0, lwd = 2)
			points(data[, 2 * i], data[, 2 * i + 1], bg="orange", pch = 22, cex = 2)		               			
		}

		par(mfrow = c(1, 1))		
	}

	return(data)
}