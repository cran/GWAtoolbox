\name{gwasformat}
\alias{gwasformat}
\title{Formatting of GWAS data files.}
\description{Renames and re-orders columns, sets tabulation as field (column) separator, calculates inflation factors and applies genomic control in GWAS data files.}
\usage{gwasformat(script, logfile)}
\arguments{
	\item{script}{
		Name of a textual input file with processing instructions. 
		The file should contain the names and locations of all GWAS data files to be processed 
		along with basic information from each individual study.
	}
	\item{logfile}{
		Name of a log file with processing output. 
		The output contains calculated inflation factors, total number of markers and number of filtered markers.
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
	The formatting is applied first to \file{input_file_1.txt} and then to \file{input_file_2.csv}.
}
\section{Field Separator}{
	The field (column) separator can be different for each GWAS data file and during the formatting it is changed to \emph{tabulation}.
	\code{gwasformat()} automatically detects the original separator field for each input file \emph{based on the first 10 rows}. 
	However, the user has the possibility to specify the original separator manually for each individual file using the command \bold{SEPARATOR}. 
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
		SEPARATOR    COMMA\cr              
		PROCESS      input_file_2.csv\cr
		PROCESS      input_file_3.txt 
	}
	For the input file \file{input_file_1.txt} the field separator is determined automatically by the program but, 
	for the input files \file{input_file_2.csv} and \file{input_file_3.txt} the separator is manually set to comma by the user.
	After the formatting all three files will have tabulation as new field separator.
}
\section{Renaming Columns}{
	The original column names in the GWAS data files are renamed using the command \bold{RENAME} in the input script.
	The command is followed by two words: the first one corresponds to the original column name, and the second one corresponds to the new column name.
	The column names can't contain tabulation or space characters.

	Example:
	
	Let's assume to have three input files: \file{input_file_1.txt}, \file{input_file_2.csv} and \file{input_file_3.txt}.
	The files have column \emph{marker}, which should be renamed.
	The new column name should be \emph{SNPID} for \file{input_file_1.txt}, and \emph{rsId} for \file{input_file_2.csv} and \file{input_file_3.txt}.
	The correct column renaming is as follows:
	\tabular{l}{
		RENAME   marker SNPID\cr
		PROCESS  input_file_1.txt\cr
		RENAME   marker rsId\cr
		PROCESS  input_file_2.csv\cr
		PROCESS  input_file_3.txt
	}
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
		\tab was used for imputation (1) or not (0)\cr
		AVPOSTPROB\tab Average posterior probability for imputed marker allele dosage
	}
	Given that different names can be provided for each GWAS data file, \code{gwasformat()} allows to redefine the default values for 
	every input file in the input script. 
	The redefinition command consists of the default column name followed by the present column name. 
	To redefine the default column names for \emph{coded} and \emph{non-coded} alleles, the command \bold{ALLELE} followed 
	by two present column names is used.
	If the present column name was renamed to the new column name with the command \emph{RENAME}, then the new column name must be used in the redefinition command.
	
	Example 1:

	Let's assume to have two input files, \file{input_file_1.txt} and \file{input_file_2.csv}. 
	In the \file{input_file_1.txt}, the column names for P-value and standard error are \emph{pval} and \emph{SE}, respectively. 
	In the \file{input_file_2.csv}, the column name for the P-value is the same as in \file{input_file_1.txt}, but the column name 
	for the standard error is \emph{STDERR}. 
	The correct column redefinition is as follows:
	\tabular{l}{
		PVALUE     pval\cr
		STDERR     SE\cr
		PROCESS    input_file_1.txt\cr
		STDERR     STDERR\cr
		PROCESS    input_file_2.csv
	}
	There is no need to redefine the \bold{PVALUE} field.
	Alternatively, if the column \emph{pval} in \file{input_file_1.txt} and \file{input_file_2.csv} needs to be renamed to \emph{p-value}, then the input script is as follows:
	\tabular{l}{
		RENAME     pval p-value\cr
		PVALUE     p-value\cr
		PROCESS    input_file_1.txt\cr
		STDERR     STDERR\cr
		PROCESS    input_file_2.csv
	}

	Example 2:

	Consider an input file, \file{input_file_1.txt}, with the following names for ALLELE1 and ALLELE2: \emph{myRefAllele} and \emph{myNonRefAllele}.
	The new column definition is applied as follows:
	\tabular{l}{
		ALLELE    myRefAllele myNonRefAllele\cr
		PROCESS   input_file_1.txt
	}
}
\section{Columns Ordering}{
	By default the \code{gwasformat()} doesn't change the original ordering of columns in the input file.
	This behaviour can be modified for every input file in the input script using the command \bold{ORDER} as specified below:
	\tabular{ll}{
		Argument\tab Description\cr
		OFF\tab The original ordering of columns is preserved\cr
		ON\tab Columns are re-ordered following the alphabetical ordering\cr
		ON column_1 column_2 ... column_n\tab Columns are re-ordered following the specified \cr
		\tab order \emph{column_1 column_2 ... column_n}
	}
	
	Example:
	
	Let's assume to have three input files: \file{input_file_1.txt}, \file{input_file_2.csv} and \file{input_file_3.txt}.
	Each file contains columns \emph{marker}, \emph{chromosome} and \emph{bp} in the order as they are listed.
	The following input script renames the column \emph{marker} to \emph{SNPID} and switches the ordering mode for every input file: 
	\tabular{l}{
		RENAME   marker SNPID\cr
		MARKER   SNPID\cr
		CHR      chromosome\cr
		POSITION bp\cr
		ORDER    ON chromosome bp SNPID\cr
		PROCESS  input_file_1.txt\cr
		ORDER    OFF\cr
		PROCESS  input_file_2.csv\cr
		ORDER    ON\cr
		PROCESS  input_file_3.txt
	}
	For the input file \file{input_file_1.txt} the columns are re-ordered to: \emph{chromosome, bp, SNPID}.
	For the input file \file{input_file_2.csv} the original ordering of columns is preserved: \emph{SNPID, chromosome, bp}.
	For the input file \file{input_file_3.txt} the columns are re-ordered following the alphabetical ordering: \emph{bp, chromosome, SNPID}.
}	
\section{Case Sensitivity}{
	By default the \code{gwasformat()} assumes that column names in the input files are case insensitive. 
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
\section{Specifying Filters}{
	The \code{gwasformat()} filters SNPs based on minor allele frequency(MAF) and imputation quality.
	The default thresholds are listed below:
	\tabular{ll}{
		Default column name\tab Default thresholds\cr
		FREQLABEL\tab > 0.01\cr
		IMP_QUALITY\tab > 0.3
	}
	The default values can be redefined using the command \bold{HQ_SNP} for every input file in the input script. 
	The command is followed by two values: the first one corresponds to the threshold for the minor allele frequency, 
	and the second one corresponds to the threshold for the imputation quality.
	
	Example 1:

	If we want to filter SNPs with MAF > 0.03 and with imputation quality > 0.4, we would add the following lines to the input script:
	\tabular{l}{
		HQ_SNP     0.03 0.4\cr
		PROCESS    input_file_1.txt
	}
	
	Example 2:
	
	If we want to disable filtering, we would change the input script as follows:
	\tabular{l}{
		HQ_SNP     0 0\cr
		PROCESS    input_file_1.txt
	}
}
\section{Inflation Factor and Genomic Control}{
	By default the \code{gwasformat} doesn't calculate the inflation factor and doesn't apply the genomic control.
	This behaviour can be modified for every input file in the input script using the command \bold{GC}/\bold{GENOMICCONTROL} as specified below:
	\tabular{ll}{
		Argument\tab Description\cr
		OFF\tab The inflation factor is not calculated\cr
		\tab and genomic control is not applied\cr
		ON\tab The inflation factor is calculated.\cr
		\tab Values in \emph{PVALUE} and \emph{STDERR} columns\cr
		\tab are corrected and saved to the new columns\cr
		\tab \emph{PVALUE_gc} and \emph{STDERR_gc}, accordingly.\cr 
		\tab Has no effect if \emph{PVALUE} column is not present.\cr
		numeric value\tab The inflation factor is assumed to be\cr 
		\tab equal to the specified \emph{numeric value}.\cr
		\tab Values in \emph{PVALUE} and \emph{STDERR} columns\cr
		\tab are corrected and saved to the new columns\cr
		\tab \emph{PVALUE_gc} and \emph{STDERR_gc}, accordingly.
	}
	If the inflation factor value is less than 1.0, then the genomic control is not applied.
	
	Example:
	\tabular{l}{
		GC      ON\cr
		PROCESS input_file_1.txt\cr
		GC      OFF\cr
		PROCESS input_file_2.csv\cr
		GC      1.1\cr
		PROCESS input_file_3.txt
	}
}
\section{Effective Sample Size}{
	By default, the \code{gwasformat()} computes the effective sample size based on \emph{IMP_QUALITY} and \emph{N} columns.
	The computed values are saved to the new column \emph{N_effective}.
}
\section{Output File Name}{
	The output file names are created by adding a prefix to the input file names.	
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
\section{The Output Files}{
	\code{gwasformat} produces one formatted (renamed/re-ordered columns, genomic control correction and etc.) copy of every original GWA data file.
	The formatting history information, containing calculated inflation factors and number of filtered markers, is saved to the log file under the provided \emph{logfile} name.  
}
\author{Daniel Taliun, Christian Fuchsberger, Cristian Pattaro}
\keyword{misc}
\keyword{utilities}
\keyword{plot}
\keyword{package}
\examples{
	\dontshow{
	# change the workspace
	currentWd <- getwd()
	newWd <- paste(system.file(package="GWAtoolbox"), "doc", sep="/")
	setwd(newWd)
	}
	
	# name of an input script
	script <- "GWASFORMAT_script.txt"
	
	# name of a logfile
	logfile <- "gwasformat_log.txt"
	
	# load GWAtoolbox library
	library(GWAtoolbox)
	
	# show contents of the input script
	file.show(script, title=script)
	
	\dontshow{options(device.ask.default = FALSE)}
	
	# run gwasformat() function
	gwasformat(script, logfile)
	
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