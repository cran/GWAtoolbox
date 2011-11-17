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

gwasqc <- function(script) {
	if (missing(script)) {
		stop("The input script argument is missing.")
	}
	
	if (is.character(script)) {
		script <- gsub("^\\s+|\\s+$", "", script)
		if (nchar(script) <= 0) {
			stop("The input script argument must be a non-blank character string.");
		}
	} else {
		stop("The input script argument must be a character string.")
	}
	
	file_separator <- ""
	if (.Platform$OS.type == "windows") {
		file_separator <- "\\"
	} else {
		file_separator <- .Platform$file.sep
	}
		
	resource_path <- paste(.path.package("GWAtoolbox"), "extdata/", sep="/")
	
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
		cat("  ", descriptors_robj[[i]][[4]], " ", sep="")
		local_descriptor <- .Call("Robj2Descriptor", descriptors_robj[[i]])
		plots <- .Call("perform_quality_check", local_descriptor, resource_path)
		.Call("delete_descriptor", local_descriptor)
		local_descriptor <- NULL
		.plot(plots[[3]])
		plots[[3]] <- NULL
		result <- append(result, list(plots))
		cat("(", format(plots[[4]][1], digits=2), " sec, ", format(plots[[4]][2], digits=2), " Mb)\n", sep="")
	}
	
	elapsed_time <- proc.time() - start_time
	cat("Done (", elapsed_time[3], " sec).\n", sep="")
	
	cat("Combining results... ")
	start_time <- proc.time()
	
	html_reports <- matrix(data=character(0), nrow=length(result), ncol=2)
	combined_boxplots <- vector()
	for (i in seq(1, length(result), 1)) {
		html_reports[i, 1] <- result[[i]][[1]]
		html_reports[i, 2] <- result[[i]][[2]]
		combined_boxplots <- append(combined_boxplots, list(result[[i]][[3]]))
	}
	boxplot_names <- .plot_combined_boxplots(combined_boxplots)
	.Call("print_combined_reports", html_reports, boxplot_names, resource_path, file_separator);
	
	elapsed_time <- proc.time() - start_time
	cat("Done (", elapsed_time[3], " sec).\n", sep="")
}