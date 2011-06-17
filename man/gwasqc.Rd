\name{gwasqc}
\alias{gwasqc}
\title{Quality Control Of GWA Data}
\description{Performs the quality control of data from Genome-Wide Association Studies (GWAS).}
\usage{gwasqc(script)}
\arguments{
	\item{script}{
		Name of a textual input file with processing instructions. 
		The file should contain the names and locations of all GWAS data files to be processed 
		along with basic information from each individual study, and instructions for the quality check.
	}
}
\section{Specifying The Input Data Files}{
	The names of the GWAS data files are specified in the input script with the command \bold{PROCESS} (one line per file). 
	A different directory path can be specified for each file.
	
	Example:
	\tabular{l}{
		PROCESS   input_file_1.txt\cr
		PROCESS   /dir_1/dir_2/input_file_2.csv	
	}
	The QC is applied first to \file{input_file_1.txt} and then to \file{input_file_2.csv}.
}

\section{Field Separator}{
	The field (column) separator can be different for each GWAS data file. 
	\code{gwasqc()} automatically detects the separator field for each input file \emph{based on the first 10 rows}. 
	However, the user has the possibility to specify the separator manually for each individual file using the command \bold{SEPARATOR}. 
	The supported arguments and related separators are listed below:
	\tabular{ll}{
		Argument\tab Separator\cr
		COMMA\tab \emph{comma}\cr
		TAB\tab \emph{tabulation}\cr
		WHITESPACE\tab \emph{whitespace}\cr
		SEMICOLON\tab \emph{semicolon}
	}

	Example:
	\tabular{l}{
		PROCESS      input_file_1.txt\cr
		SEPARATOR    TAB\cr              
		PROCESS      input_file_2.csv\cr
		PROCESS      input_file_3.txt 
	}
	For the input file \file{input_file_1.txt} the field separator is determined automatically by the program but, 
	for the input files \file{input_file_2.csv} and \file{input_file_3.txt} the separator is manually set to tabulation by the user.
}

\section{Missing Values}{
	By default, \code{gwasqc()} assumes that missing values are labeled as \emph{NA}. 
	However, the label for missing value can be specified manually by the user with the command \bold{MISSING}.

	Example:
	\tabular{l}{
		MISSING     -\cr
		PROCESS     input_file_1.txt\cr 
		MISSING     NA\cr               
		PROCESS     input_file_2.csv 
	}
	The \emph{hyphen} symbol identifies missing values in the input file \file{input_file_1.txt} and \emph{NA} identifies 
	missing values in the input file \file{input_file_2.txt}.
}

\section{Column Names}{
	In the table below, the complete list of the default column names for the GWAS data file is reported. 
	These names identify uniquely the items in the GWAS data file.
	\tabular{ll}{
		Default column name(s)\tab Description\cr
		MARKER\tab Marker name\cr
		CHR\tab Chromosome number or name\cr
		POSITION\tab Marker position\cr
		ALLELE1, ALLELE2\tab Coded and non-coded alleles\cr
		FREQLABEL\tab Allele frequency for the coded allele\cr
		STRAND\tab Strand\cr
		IMPUTED\tab Label value indicating if the marker\cr
		\tab was imputed (1) or genotyped (0)\cr
		IMP_QUALITY\tab Imputation quality statistics; this can be\cr	
		\tab different depending on the software used\cr 
		\tab for imputation: MACH's \emph{Rsq}, IMPUTE's \emph{properinfo}, ...\cr
		EFFECT\tab Effect size\cr
		STDERR\tab Standard error\cr
		PVALUE\tab P-value\cr
		HWE_PVAL\tab Hardy-Weinberg equilibrium p-value\cr
		CALLRATE\tab Genotype callrate\cr
		N\tab Sample size\cr
		USED_FOR_IMP\tab Label value indicating if a marker\cr
		\tab was used for imputation (1) or not (0)
	}
	Given that different names can be provided for each GWAS data file, \code{gwasqc()} allows to redefine the default values for 
	every input file in the input script. 
	The redefinition command consists of the default column name followed by the new column name. 
	To redefine the default column names for \emph{coded} and \emph{non-coded} alleles, the command \bold{ALLELE} followed 
	by two new column names is used.
	
	Example 1:

	Let's assume to have two input files, \file{input_file_1.txt} and \file{input_file_2.txt}. 
	In the \file{input_file_1.txt}, the column names for effect size and standard error are \emph{beta} and \emph{SE}, respectively. 
	In the \file{input_file_2.txt}, the column name for the effect size is the same as in \file{input_file_1.txt}, but the column name 
	for the standard error is \emph{STDERR}. 
	The correct column redefinition is as follows:
	\tabular{l}{
		EFFECT     beta\cr
		STDERR     SE\cr
		PROCESS    input_file_1.txt\cr
		STDERR     STDERR\cr
		PROCESS    input_file_2.csv
	}
	There is no need to redefine the \bold{EFFECT} field.

	Example 2:

	Consider an input file, \file{input_file_1.txt}, with the following names for ALLELE1 and ALLELE2: \emph{myRefAllele} and \emph{myNonRefAllele}.
	The new column definition is applied as follows:
	\tabular{l}{
		ALLELE    myRefAllele myNonRefAllele\cr
		PROCESS   input_file_1.txt
	}
}

\section{Case Sensitivity}{
	By default the \code{gwasqc()} assumes that column names in the input files are case insensitive. 
	For example, the column names \emph{STDERR}, \emph{StdErr}, and \emph{STDErr} are all perfectly equivalent.
	This behaviour can be modified for every input file in the input script using the command \bold{CASESENSITIVE}, 
	that controls case sensitivity for the column names, as specified below:
	\tabular{ll}{
		Argument\tab Description\cr
		0\tab Column names in the input file\cr
		\tab are case insensitive (default)\cr
		1\tab Column names in the input file\cr
		\tab are case sensitive
	}

	Example:
	\tabular{l}{
		CASESENSITIVE   1\cr
		PROCESS         input_file_1.txt\cr
		CASESENSITIVE   0\cr
		PROCESS         input_file_2.csv 
	}	
}

\section{Filter for Implausible Values}{
	Often, there is the necessity to identify implausible values, to exclude unreliable results from the meta-analysis. 
	Implausible values can happen due to data sparseness, errors in the data handling, or other causes.

	\code{gwasqc()} identifies SNPs with suspicious statistics (p-value, standard error, etc.) by applying appropriate threshold values.
	After the data processing, a detailed report including the number of SNPs with implausible statistics and the nature of the problem is produced.
	In addition, suspicious SNPs are excluded from the calculation of the summary statistics on data quality.

	The default filter thresholds are listed below:
        \tabular{ll}{
		Default column name\tab Default thresholds\cr
		STDERR\tab [0, 100000]\cr
		IMP_QUALITY\tab (0, 1.5)\cr
		PVALUE\tab (0, 1)\cr
		FREQLABEL\tab (0, 1)\cr
		HWE_PVAL\tab (0, 1)\cr
		CALLRATE\tab (0, 1)
	}
	The user has the option to modify the thresholds to account for specific needs.
	The new thresholds can be specified after the redefinition of the column name.

	Example:

	Assume that the input file \file{input_file_1.txt} has a standard error column called \emph{STDERR} and 
	that the corresponding column in the input file \file{input_file_2.csv} is called \emph{SE}. 
	In addition, the imputation quality column is defined as \emph{oevar_imp} in both files.
	The following script shows how the user can re-define the column names while applying different plausibility filters:
	\tabular{l}{
		STDERR        STDERR 0 80000\cr
		IMP_QUALITY   oevar_imp 0 1\cr
		PROCESS       input_file_1.txt\cr
		STDERR        SE 0 100000\cr
		PROCESS       input_file_2.csv
	}
	The file \file{input_file_1.txt} has new [0, 80000] thresholds for the standard error column and new (0, 1) thresholds for the imputation quality. 
	For the file \file{input_file_2.csv} the thresholds of [0, 100000] will be applied to the standard error column, 
	while for the imputation quality column the same filters as for the \file{input_file_1.txt} will be applied.	
}

\section{High Quality Filters}{
	SNPs with low imputation quality and with too small minor allele frequency (MAF) could make spuriously small p-values happen.
	Checking for the presence of cryptic relatedness or hidden population sub-structure through the estimation of the inflation 
	factor \emph{lambda} can be important, but one needs to identify the SNPs that could artificially increase the \emph{lambda} value.
	\code{gwasqc()} identifies the 'high quality' SNPs by means of filters on the imputation quality and on the MAF.
	Summary statistics are calculated on the 'high quality' SNPs only.
	The default thresholds are listed below:
	\tabular{ll}{
		Default column name\tab Default thresholds\cr
		FREQLABEL\tab > 0.01\cr
		IMP_QUALITY\tab > 0.3
	}
        The default values can be redefined using the command \bold{HQ_SNP} for every input file in the input script. 
	The command is followed by two values: the first one corresponds to the threshold for the minor allele frequency, 
	and the second one corresponds to the threshold for the imputation quality.

	Example:

	If we want to define 'high quality' SNPs those with MAF > 0.03 and with imputation quality > 0.4, we would add the following lines to the input script:
	\tabular{l}{
		HQ_SNP     0.03 0.4\cr
		PROCESS    input_file_1.txt
	}	
}

\section{Plotting Filter}{
	The plotting filter is used to select appropriate data for the various summary plots. 
	The filter has two threshold levels and each of them is applied dependently on the plot type and column. 
	The default threshold values are listed below:
	\tabular{lll}{
		Default column name\tab 1st level thresholds\tab 2nd level thresholds\cr
		FREQLABEL\tab > 0.01\tab > 0.05\cr
		IMP_QUALITY\tab > 0.3\tab > 0.6\cr
	}
	The default thresholds for the coded allele frequency and imputation quality can be redefined accordingly with 
	the commands \bold{MAF} and \bold{IMP} for each input file.

	Example:
	\tabular{l}{
		MAF        0.02 0.03\cr
		IMP        0.3 0.5\cr
		PROCESS    input_file_1.txt
	}
	A new plotting filter is set for the input file \file{input_file_1.txt}.
	There is a first level of filters which selects SNPs with MAF > 0.02 and the imputation quality > 0.3, and a second, higher, 
	level filter which selects SNPs with MAF > 0.03 and imputation quality > 0.5.
}

\section{Output File Name}{
	For both text and graphic output files, the output file names are created by adding a prefix to the input file names.	
	The prefix is specified with the command \bold{PREFIX}. 
	
	Example:
	\tabular{l}{
		PREFIX       res_\cr
		PROCESS      input_file_1.txt\cr
		PROCESS      input_file_2.csv\cr
		PREFIX       result_\cr
		PROCESS      input_file_3.tab\cr
	}
	All the output files corresponding to the input files \file{input_file_1.txt} and \file{input_file_2.csv} will be prefixed with \emph{res_}; 
	the output files corresponding to the input file \file{input_file_3.tab} will be prefixed with \emph{result_}.
}

\section{Verbosity Level}{
	The command \bold{VERBOSITY} allows to control the number of output figures, as described below:	
	\tabular{ll}{
		Argument\tab Description\cr
		1\tab The default and the lowest verbosity level.\cr
		2\tab The highest verbosity level.
	}

	Example:
	\tabular{l}{
		VERBOSITY       2\cr
		PROCESS         input_file_1.txt\cr
		VERBOSITY       1\cr
		PROCESS         input_file_2.csv 
	}
}

\section{Number And Content Of Plots}{
	Number and content of the output plots depend on the setting of the plotting filter and on the available columns in the input file. 
	If some dependency is not satisfied because of missing columns or some filter setting, 
	then some plots could not be created or they could be truncated at different levels than expected.
	See the tutorial for the list of dependencies.

	The boxplots comparing \emph{EFFECT} distributions across studies allow the specification of a \bold{BOXPLOTWIDTH} that can be based 
	on one of the other available information (typically the sample size).
	As an argument, \bold{BOXPLOTWIDTH} requires one of the default column names. 
	If \bold{BOXPLOTWIDTH} is not specified all boxplots have the same width.

	It is also possible to specify labels for every input file, to be used in the plots in spite of the full file names, 
	which could be too long and, therefore, clutter the plots.

	Example:

	Let \emph{n_total} be the column name which identifies the sample size in the input file \file{input_file_1.txt}, 
	and \emph{samplesize} the corresponding name in \file{input_file_2.csv}.
	Consider the following input script:
	\tabular{l}{
		N              n_total\cr
		PROCESS        input_file_1.txt	first\cr
		N              samplesize\cr
		PROCESS        /dir_1/dir_2/input_file_2.csv	second\cr
		BOXPLOTWIDTH   N
	}
	The width of the boxplots will be based on the study sample sizes, which is reported with different names in the two input files.
	The labels "first" and "second" will be used to identify the two studies in the plots.
}

\section{The Output Files}{
	\code{gwasqc()} produces 4 types of files:
	\enumerate{
		\item Figures, including QQ-plots, histograms, and boxplots.
		\item One textual report file with \emph{.txt} extension.
		\item One comma-separated file with \emph{.csv} extension, that contains all the summary statistics for the high quality imputation data.
		\item One \emph{HTML} document, that combines both textual output and figures and allows a very easy and dynamic querying 
		of all the output in a hypertext browser.
	}
}

\author{Daniel Taliun, Christian Fuchsberger, Cristian Pattaro}
\keyword{misc}
\keyword{utilities}
\keyword{dplot}
\keyword{package}
\examples{
	\dontshow{
	# change the workspace
	currentWd <- getwd()
	newWd <- paste(system.file(package="GWAtoolbox"), "doc", sep="/")
	setwd(newWd)
	}

	# name of an input script
	script <- "GWASQC_script.txt"

	# load GWAtoolbox library
	library(GWAtoolbox)

	# show contents of the input script
	file.show(script, title=script)

	\dontshow{options(device.ask.default = FALSE)}

	# run gwasqc() function
	gwasqc(script)

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
