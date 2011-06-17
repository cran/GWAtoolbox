\name{snp2gene}
\alias{snp2gene}
\title{SNPs to genes mapping.}
\description{For provided SNPs finds all genes in specified deviation (e.g. 0, +/- 50000, +/- 100000, ... ).}
\usage{snp2gene(genes, snps, output, delimiter, deviations, append)}
\arguments{
	\item{genes}{The name of an input file with genes.}
	\item{snps}{The name of an input file with SNPs.}
	\item{output}{The name of an output file.}
	\item{delimiter}{Delimiter for the names of genes, that are within the same deviation.}
	\item{deviations}{Vector with deviation values (e.g. c(0, 50000, 100000)).}
	\item{append}{If true, then writes all columns from an input file into a result file.}
}
\note{Under development.}
\author{Daniel Taliun, Christian Fuchsberger, Cristian Pattaro}
\keyword{misc}
\keyword{utilities}
\keyword{package}