\name{harmonize_id}
\alias{harmonize_id}
\title{The harmonization of SNP identifiers.}
\description{Function harmonizes SNP identifiers in the input file according to the provided mapping table.}
\usage{harmonize_id(input, output, map, id = "markername", 
		alleles = c("ref_allele", "non_ref_allele"), 
		sep = "\t", drop = FALSE, gzip = TRUE)}
\arguments{
	\item{input}{
		Name of an input file.
	}
	\item{output}{
		Name of an output file.
	}
	\item{map}{
		Name of the file with ID mapping table.
	}
	\item{id}{
		Name of the column containing SNP identifiers.
	}
	\item{alleles}{
		Vector with column names containing reference and non-reference alleles.
	}
	\item{sep}{
		The field separator character in the specified input file.
	}
	\item{drop}{
		If TRUE, then all unresolved variants are dropped from the output file.
	}
	\item{gzip}{
		TRUE if output file is in gzip format.
	}
}
\author{Daniel Taliun, Christian Fuchsberger, Cristian Pattaro}
\keyword{misc}
\keyword{utilities}
\keyword{package}