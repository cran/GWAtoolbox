\name{kgenes}
\alias{kgenes}
\title{SNPs to k nearest genes mapping.}
\description{For provided SNPs finds k nearest genes to the left and right.}
\usage{kgenes(genes, snps, output, k, append, multicolumn)}
\arguments{
	\item{genes}{The name of an input file with genes.}
	\item{snps}{The name of an input file with SNPs.}
	\item{output}{The name of an output file.}
	\item{k}{The maximum number of nearest genes to report.}
	\item{append}{If true, then writes all columns from an input file into a result file.}
	\item{multicolumn}{If true, then for every gene and distance a separate column is created.}
}
\note{Under development.}
\author{Daniel Taliun, Christian Fuchsberger, Cristian Pattaro}
\keyword{misc}
\keyword{utilities}
\keyword{package}