#
# Copyright © 2012 Daniel Taliun, Christian Fuchsberger and Cristian Pattaro. All rights reserved.
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

harmonize_id <- function(input, output, map, id = "markername", alleles = c("ref_allele", "non_ref_allele"), sep = "\t", drop = FALSE, gzip = TRUE) {
	if (missing(input)) {
		stop("The input file name is missing.")
	}	
	
	if (missing(output)) {
		stop("The output file name is missing.")
	}
	
	if (missing(map)) {
		stop("The map file name is missing.")
	}
	
	if (is.character(input)) {
		if (length(input) <= 0) {
			stop("The input file name is empty.")
		} else if (length(input) > 1) {
			stop("The input file name has multiple values.")
		}
		input <- gsub("^\\s+|\\s+$", "", input)
		if (nchar(input) <= 0) {
			stop("The input file name must be a non-blank character string.")
		}
	} else {
		stop("The input file name must be a character string.")
	}
	
	if (is.character(output)) {
		if (length(output) <= 0) {
			stop("The output file name is empty.")
		} else if (length(output) > 1) {
			stop("The output file name has multiple values.")
		}
		output <- gsub("^\\s+|\\s+$", "", output)
		if (nchar(output) <= 0) {
			stop("The output file name must be a non-blank character string.")
		}
	} else {
		stop("The output file name must be a character string.")
	}
	
	if (is.character(map)) {
		if (length(map) <= 0) {
			stop("The map file name is empty.")
		} else if (length(map) > 1) {
			stop("The map file name has multiple values.")
		}
		map <- gsub("^\\s+|\\s+$", "", map)
		if (nchar(map) <= 0) {
			stop("The map file name must be a non-blank character string.")
		}
	} else {
		stop("The map file name must be a character string.")
	}
	
	if (is.character(id)) {
		if (length(id) <= 0) {
			stop("The SNP ID column name is empty.")
		} else if (length(id) > 1) {
			stop("The SNP ID column name has multiple values.")
		}
		id <- gsub("^\\s+|\\s+$", "", id)
		if (nchar(id) <= 0) {
			stop("The SNP ID column name must be a non-blank character string.")
		}
	} else {
		stop("The SNP ID column name must be a character string.")
	}
	
	if (is.character(alleles)) {
		if (length(alleles) != 2) {
			stop("The allele column names must be a character vector of length 2.")	
		}
			
		alleles[1] <- gsub("^\\s+|\\s+$", "", alleles[1])
		if (nchar(alleles[1]) <= 0) {
			stop("The reference allele column name must be a non-blank character string.")
		}
			
		alleles[2] <- gsub("^\\s+|\\s+$", "", alleles[2])
		if (nchar(alleles[2]) <= 0) {
			stop("The non-reference allele column name must be a non-blank character string.")
		}
			
	} else {
		stop("The allele column names must be a character vector.")
	}
	
	if (is.character(sep)) {
		if (length(sep) <= 0) {
			stop("The fied separator character is empty.")
		} else if (length(sep) > 1) {
			stop("The fied separator character has multiple values.")
		}
		if (nchar(sep) != 1) {
			stop("The fied separator must be a single character.")
		}
	} else {
		stop("The fied separator must be a character.")
	}
	
	if (is.logical(drop)) {
		if (length(drop) <= 0) {
			stop("Argument 'drop' is empty.")
		} else if (length(drop) > 1) {
			stop("Argument 'drop' has multiple values.")
		}
	} else {
		stop("Argument 'drop' must be a logical.")
	}
	
	if (is.logical(gzip)) {
		if (length(gzip) <= 0) {
			stop("Argument 'gzip' is empty.")
		} else if (length(gzip) > 1) {
			stop("Argument 'gzip' has multiple values.")
		}
	} else {
		stop("Argument 'gzip' must be a logical.")
	}
	
	result <- .Call("perform_id_harmonization", input, output, map, id, alleles, sep, drop, gzip)
}