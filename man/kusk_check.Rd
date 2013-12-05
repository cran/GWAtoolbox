\name{kusk_check}
\alias{kusk_check}
\title{Comparison Of Skewness and Kurtosis of Effect between GWAS studies.}
\description{Compare the distributions of effect estimates between GWA studies.}
\usage{kusk_check(script, worst = c(50, 75, 90, 99, 100), plot = TRUE)}
\arguments{
	\item{script}{
		Name of a textual input file with processing instructions. 
		The file should contain the names and locations of all GWAS data files to be processed 
		along with basic information from each individual study, and instructions for the quality check.
	}
	\item{worst}{A vector consisting of any of integer numbers  50, 75, 95, 99, and 100.}
	\item{plot}{Logical value indicating if the diagnostic plot should be produced.}
}
\details{
	The \emph{GWAtoolbox} allows automatic comparison of skewness and kurtosis of effect size distribution between GWA stuties.
	The \code{gwasqc()} function estimates the corresponding skewness and kurtosis values during the quality check and includes them into the \emph{CSV} reports. 
	Then, the auxiliary \code{kusk_check()} function can be used to export this information to the data frame and to produce the diagnostic plots.
	As input it requires the same script as \code{gwasqc()} function and assumes that all the \emph{CSV} reports are in the current working directory.
	Additionally, an optional list consisting of any of integer numbers  50, 75, 95, 99, and 100 can be specified.
	The numbers correspond to the percentage of SNPs to be considered as representing the null distribution.
	
}
\author{Cristian Pattaro, Daniel Taliun, Christian Fuchsberger}
\keyword{misc}
\keyword{utilities}
\keyword{package}
\examples{
	\dontshow{
	# change the workspace
	currentWd <- getwd()
	newWd <- paste(system.file(package="GWAtoolbox"), "doc", sep="/")
	setwd(newWd)
	}
	\dontrun{
	# name of an input script
	script <- "GWASQC_script.txt"

	# load GWAtoolbox library
	library(GWAtoolbox)

	# show contents of the input script
	file.show(script, title=script)
	}
	\dontshow{options(device.ask.default = FALSE)}
	\dontrun{
	# run gwasqc() function
	gwasqc(script)
	
	# run kusk_check() function
	W <- kusk_check("GWASQC_script.txt", worst = c(50), plot = TRUE)
	points(W$sk50[W$ku50 > 5], W$ku50[W$ku50 > 5], pch = 22, bg = 2, cex = 2)
	text(W$sk50[W$ku50 > 5], W$ku50[W$ku50 > 5], lab = W$study[W$ku50 > 5], cex = 1, pos = 4)
	}
	\dontshow{
	# restore previous workspace
	setwd(currentWd)
	}
	
    \dontshow{
	# all input and output files are located in the subdirectory "doc" of the installed GWAtoolbox package
	message <- c("\n", rep("#", 40), "\n")
	message <- c(message, "\nAll input and output files of this example are located in directory:\n", newWd, "\n")
	message <- c(message, "\n", rep("#", 40),"\n")
	cat(message, sep="")
	}
}