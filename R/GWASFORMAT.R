#
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
#

gwasformat <- function(script, logfile) {
	if (missing(script)) {
		stop("The input script argument is missing.")
	}
	
	if (missing(logfile)) {
		stop("The log file argument is missing.")
	}
	
	if (is.character(script)) {
		script <- gsub("^\\s+|\\s+$", "", script)
		if (nchar(script) <= 0) {
			stop("The input script argument must be a non-blank character string.");
		}
	} else {
		stop("The input script argument must be a character string.")
	}
	
	if (is.character(logfile)) {
		logfile <- gsub("^\\s+|\\s+$", "", logfile)
		if (nchar(logfile) <= 0) {
			stop("The log file argument must be a non-blank character string.");
		}
	} else {
		stop("The log file argument must be a character string.")
	}
	
	file_separator <- ""
	if (.Platform$OS.type == "windows") {
		file_separator <- "\\"
	} else {
		file_separator <- .Platform$file.sep
	}
	
	cat("Processing input script... ")
	start_time <- proc.time()
	
	descriptors_cpp <- .Call("process_script", script, file_separator)
	if (length(descriptors_cpp) <= 0) {
		stop("No GWAS input files to process.")
	}
	
	descriptors_robj <- vector()
	for (i in seq(1, length(descriptors_cpp), 1)) {
		descriptor_robj <- .Call("Descriptor2Robj", descriptors_cpp[[i]])
		.Call("delete_descriptor", descriptors_cpp[[i]])
		descriptors_robj <- append(descriptors_robj, list(descriptor_robj))
	}
	descriptors_cpp <- NULL
	
	elapsed_time <- proc.time() - start_time
	cat("Done (", elapsed_time[3], " sec).\n", sep="")
	
	cat("Processing...\n")
	start_time <- proc.time()
	
	result <- vector()
	for (i in seq(1, length(descriptors_robj), 1)) {
		cat("  ", descriptors_robj[[i]][[4]], "\n", sep="")
		local_descriptor <- .Call("Robj2Descriptor", descriptors_robj[[i]])
		stats <- .Call("perform_formatting", local_descriptor)
		.Call("delete_descriptor", local_descriptor)
		local_descriptor <- NULL
		result <- append(result, list(stats))
	}
	
	cat("Formatting date: ", date(), "\n", file = logfile, sep="", append = FALSE)
	for (i in seq(1, length(result), 1)) {
		cat("File '", result[[i]]$name, "':\n", file = logfile, sep = "", append = TRUE)
		if (is.null(result[[i]]$inflation_factor_value)) {
			cat("\tGENOMIC CONTROL was not enabled\n", file = logfile, sep = "", append = TRUE)
		} else if (is.null(result[[i]]$inflation_factor_stats)) {
			cat("\tINFLATION FACTOR was set to ", result[[i]]$inflation_factor_value, "\n", file = logfile, sep = "", append = TRUE)
		} else {
			cat("\tINFLATION FACTOR is equal to ", result[[i]]$inflation_factor_value, "\n", file = logfile, sep = "", append = TRUE)
			cat("\tINFLATION FACTOR was calculated based on ", result[[i]]$inflation_factor_stats[2], " out of ", result[[i]]$inflation_factor_stats[3], " markers\n", file = logfile, sep = "", append = TRUE)
			cat("\tINFLATION FACTOR was calculated in ", result[[i]]$inflation_factor_stats[1], " seconds\n", file = logfile, sep = "", append = TRUE)
		}
		cat("\tFORMATTED FILE contains ", result[[i]]$formatting_stats[2], " out of ", result[[i]]$formatting_stats[3], " markers\n", file = logfile, sep = "", append = TRUE)
		cat("\tFORMATTED FILE was produced in ", result[[i]]$formatting_stats[1], " seconds\n", file = logfile, sep = "", append = TRUE)
	}
	
	elapsed_time <- proc.time() - start_time
	cat("Done (", elapsed_time[3], " sec).\n", sep="")
} 