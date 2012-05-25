\name{annotate}
\alias{annotate}
\title{SNPs annotation with regions (e.g. genes).}
\description{For provided SNPs finds all genes in specified deviation around them (e.g. 0, +/- 50000, +/- 100000, ... ).}
\usage{annotate(script)}
\arguments{
	\item{script}{
		Name of a textual input file with processing instructions. 
		The file should contain the names and locations of all GWAS data files to be annotated 
		along with basic information from each individual study.
	}
}
\details{
	Function \code{annotate()} annotates every marker in input files with regions (e.g. genes) that contain it or fall in a specified windows around it (e.g. +/-50kb, +/-100kb and etc).
	The arbitrary number of windows of various sizes can be specified in the input script.
	The regions with their chromosomal coordinates must be provided in a separate file.
	It is possible to annotate markers if only their names are available (e.g. rsId) in input files, or if there is a need to change chromosomal positions (e.g. if different version of human genome build should be used).
	In this case, their chromosomal positions must be provided in a separate map file.
}
\section{Specifying The Input Data Files}{
	The names of the GWAS data files are specified in the input script with the command \bold{PROCESS} (one line per file). 
	A different directory path can be specified for each file.
	
	Example:
	\tabular{l}{
		PROCESS   input_file_1.txt\cr
		PROCESS   /dir_1/dir_2/input_file_2.csv	
	}
	The annotation is applied first to \file{input_file_1.txt} and then to \file{input_file_2.csv}.
}
\section{Specifying The Regions Files}{
	The names of the regions (e.g. with genes) files are specified in the input script with the command \bold{REGIONS_FILE}.
	In the same script different regions files can be specified for different GWAS data files.
	Also different directory path can be specified for each regions file.

	Example:
	\tabular{l}{
	    REGIONS_FILE   genes_file_1.txt\cr
		PROCESS        input_file_1.txt\cr
		REGIONS_FILE   /dir_1/dir_2/genes_file_2.csv\cr
		PROCESS        input_file_2.csv\cr
		PROCESS        input_file_3.txt
	}
	The annotation is applied first to \file{input_file_1.txt} using regions from \file{genes_file_1.txt} file.
	Then, files \file{input_file_2.csv} and \file{input_file_3.txt} are annotated with regions in \file{genes_file_2.csv} file.
}
\section{Specifying The Map Files}{
	The names of the map files are specified in the input script with the command \bold{MAP_FILE}.
	In the same script different map files can be specified for different GWAS data files.
	Also different directory path can be specified for each map files.
	
	Example:
	\tabular{l}{
		MAP_FILE       map_file_1.txt\cr
		REGIONS_FILE   genes_file_1.txt\cr
		PROCESS        input_file_1.txt\cr
		MAP_FILE       /dir_1/dir_2/map_file_2.csv\cr
		REGIONS_FILE   /dir_1/dir_2/genes_file_2.csv\cr
		PROCESS        input_file_2.csv\cr
		PROCESS        input_file_3.txt
	}
	The annotation is applied first to \file{input_file_1.txt} using marker genomic positions in \file{map_file_1.txt} file and regions in \file{genes_file_1.txt} file.
	Then, files \file{input_file_2.csv} and \file{input_file_3.txt} are annotated with regions in \file{genes_file_2.csv} file using marker genomic positions in \file{map_file_2.csv}.
}
\section{Specifying Column Names in Input Data Files}{
	In the table below, the complete list of the default column names for the GWAS data file is reported. 
	These names identify uniquely the items in the GWAS data file.
	\tabular{ll}{
		Default column name(s)\tab Description\cr
		MARKER\tab Marker name\cr
		CHR\tab Chromosome number or name\cr
		POSITION\tab Marker position
	}
	Given that different names can be provided for each GWAS data file, \code{annotate()} allows to redefine the default values for 
	every input file in the input script. 
	The redefinition command consists of the default column name followed by the new column name.
	When the map file is specified using command \bold{MAP_FILE}, then \bold{CHR} and \bold{POSITION} columns in the GWAS data file are not required.
	
	Example:

	Let's assume to have two input files, \file{input_file_1.txt} and \file{input_file_2.csv}. 
	In the \file{input_file_1.txt}, the column names for marker name, chromosome name and position are \emph{SNPID}, \emph{CHR} and \emph{POS}, respectively. 
	In the \file{input_file_2.csv}, the column names for marker name is the same as in \file{input_file_1.txt}, but the column names 
	for the chromosome and position are \emph{chromosome} and \emph{position}, respectively. 
	The correct column redefinition is as follows:
	\tabular{l}{
		MARKER     SNPID\cr
		POSITION   POS\cr
		PROCESS    input_file_1.txt\cr
		CHR        chromosome\cr
		POSITION   position\cr
		PROCESS    input_file_2.csv
	}
	There are no need to define the \bold{CHR} field for the \file{input_file_1.txt}, since it matches the default name.
}
\section{Specifying Column Names in Regions Files}{	
	In the table below, the complete list of the default column names for the regions file is reported. 
	These names identify uniquely the items in the regions file.
	\tabular{ll}{
		Default column name(s)\tab Description\cr
		REGION_NAME\tab Region name (e.g. gene name)\cr
		REGION_CHR\tab Chromosome number or name\cr
		REGION_START\tab Region (e.g. gene) start position\cr
		REGION_END\tab Region (e.g.) end position
	}
	Given that different names can be provided for each regions file, \code{annotate()} allows to redefine the default values for 
	every regions file in the input script. 
	The redefinition command consists of the default column name followed by the present column name. 
	
	Example:
	
	Let's assume to have two map files, \file{region_file_1.txt} and \file{region_file_2.csv}. 
	In the \file{region_file_1.txt}, the column names for the region name, chromosome, start and end position are \emph{name}, \emph{chr}, \emph{REGION_START} and \emph{REGION_END}, respectively. 
	In the \file{region_file_2.csv}, the column name for the region name and chromosome are the same as in \file{regions_file_1.txt}, but the column names
	for the region start and end positions are \emph{start} and \emph{end}, respectively. 
	The correct column redefinition is as follows:
	\tabular{l}{
		REGIONS_FILE   genes_file_1.txt\cr
		REGION_NAME    name\cr
		REGION_CHR     chr\cr
		PROCESS        input_file_1.txt\cr
		REGIONS_FILE   genes_file_2.csv\cr
		REGION_START   start\cr
		REGION_END     end\cr
		PROCESS        input_file_2.csv
	}
	There is no need to define the \bold{REGION_START} and \bold{REGION_END} fields for \file{genes_file_1.txt} regions file.
	Also there is no need to redefine \bold{REGION_NAME} and \bold{REGION_CHR} fields for the \file{genes_file_2.csv} map file.	
}
\section{Specifying Column Names in Map Files}{
	In the table below, the complete list of the default column names for the map file is reported. 
	These names identify uniquely the items in the map file.
	\tabular{ll}{
		Default column name(s)\tab Description\cr
		MAP_MARKER\tab Marker name\cr
		MAP_CHR\tab Chromosome number or name\cr
		MAP_POSITION\tab Marker position
	}
	Given that different names can be provided for each map file, \code{annotate()} allows to redefine the default values for 
	every map file in the input script. 
	The redefinition command consists of the default column name followed by the present column name. 
	
	Example:
	
	Let's assume to have two map files, \file{map_file_1.txt} and \file{map_file_2.csv}. 
	In the \file{map_file_1.txt}, the column names for marker name, chromosome and position are \emph{name}, \emph{MAP_CHR} and \emph{pos}, respectively. 
	In the \file{map_file_2.csv}, the column name for the marker name and chromosome are the same as in \file{map_file_1.txt}, but the column name 
	for the marker position is \emph{map_pos}. 
	The correct column redefinition is as follows:
	\tabular{l}{
		MAP_FILE       map_file_1.txt\cr
		MAP_MARKER     name\cr
		MAP_POSITION   pos\cr
		REGIONS_FILE   genes_file_1.txt\cr
		PROCESS        input_file_1.txt\cr
		MAP_FILE       map_file_2.csv\cr
		MAP_POSITION   map_pos\cr
		REGIONS_FILE   genes_file_2.csv\cr
		PROCESS        input_file_2.csv
	}
	There is no need to define the \bold{MAP_CHR} field for both map files.
	Also there is no need to redefine \bold{MAP_MARKER} for the \file{genes_file_2.csv} map file.	
}
\section{Field Separator in Input Data Files}{
	The field (column) separator can be different for each GWAS data file.
	\code{annotate()} automatically detects the original separator field for each input file \emph{based on the first 10 rows}. 
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
}
\section{Field Separator in Regions Files}{
	The field (column) separator can be different for each regions file.
	\code{annotate()} automatically detects the original separator field for each regions file \emph{based on the first 10 rows}. 
	However, the user has the possibility to specify the original separator manually for each individual file using the command \bold{REGIONS_FILE_SEPARATOR}. 
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
		REGIONS_FILE             genes_file_1.txt\cr
		PROCESS                  input_file_1.txt\cr
		REGIONS_FILE             genes_file_2.csv\cr
		REGIONS_FILE_SEPARATOR   COMMA\cr
		PROCESS                  input_file_2.csv\cr
		REGIONS_FILE             genes_file_3.txt\cr
		PROCESS                  input_file_3.txt
	}
	For the regions file \file{genes_file_1.txt} the field separator is determined automatically by the program but, 
	for the regions files \file{genes_file_2.csv} and \file{genes_file_3.txt} the separator is manually set to comma by the user.
}
\section{Field Separator in Map Files}{
	The field (column) separator can be different for each map file.
	\code{annotate()} automatically detects the original separator field for each map file \emph{based on the first 10 rows}. 
	However, the user has the possibility to specify the original separator manually for each individual file using the command \bold{MAP_FILE_SEPARATOR}. 
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
		MAP_FILE             map_file_1.txt\cr		
		REGIONS_FILE         genes_file_1.txt\cr
		PROCESS              input_file_1.txt\cr
		MAP_FILE             map_file_2.csv\cr
		MAP_FILE_SEPARATOR   COMMA\cr
		REGIONS_FILE         genes_file_2.csv\cr
		PROCESS              input_file_2.csv\cr
		MAP_FILE             map_file_3.txt\cr
		PROCESS              input_file_3.txt
	}
	For the map file \file{map_file_1.txt} the field separator is determined automatically by the program but, 
	for the map files \file{map_file_2.csv} and \file{map_file_3.txt} the separator is manually set to comma by the user.
}
\section{Case Sensitivity}{
	By default the \code{annotate()} assumes that column names in the all specified files are case insensitive. 
	For example, the column names \emph{CHR}, \emph{Chr}, and \emph{chr} are all perfectly equivalent.
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
		MAP_FILE        map_file_1.txt\cr
		REGIONS_FILE    genes_file_1.txt\cr
		PROCESS         input_file_1.txt\cr
		CASESENSITIVE   0\cr
		MAP_FILE        map_file_2.csv\cr
		REGIONS_FILE    genes_file_2.csv\cr
		PROCESS         input_file_2.csv 
	}	
}
\section{Specifying Window Size For Annotation}{
	Every marker in the GWAS data file is annotated with the regions (e.g. genes) that fall in a particular window around it.
	\code{annotate()} allows to specify multiple window sizes using command \bold{REGIONS_DEVIATION}.
	Command \bold{REGIONS_DEVIATION} is followed by an arbitrary number of positive integers that specify window sizes around markers in base pairs.
	Each specified window size results in a new output column where all regions overlapping with this window are reported.
	The ouptut columns are ordered by window size starting with the smallest.
	Therefore, every new output column represents bigger window size and lists only those regions that were not reported previously.
	If \bold{REGIONS_DEVIATION} is not specified, then the default window sizes are 0, 100000 and 250000 (i.e. 0, +/-100kb and +/- 250kb around marker).
	If 0 is specified, then only regions that include the marker are reported.
	
	Example:
	\tabular{l}{
		REGIONS_FILE        genes_file_1.txt\cr
		REGIONS_DEVIATION   0 50000 100000\cr
		PROCESS             input_file_1.txt\cr
		REGIONS_DEVIATION   0 100000 250000 500000\cr
		PROCESS             input_file_2.csv
	}
	Every marker in \file{input_file_1.txt} will be annotated with regions that contains it or are within +/-50kb and +/-100kb windows around it.
	While every marker in \file{input_file_2.csv} will be annotated with regions that contains it or are within +/-100kb, +/-250kb and +/-500kb windows around it.
}
\section{Specifying Output Format}{
	Often GWAS data file contains many columns that are not required in the output files with annotation results.
	By default, in addition to columns with annotated regions, \code{annotate()} outputs only columns with marker name, chromosome name and position.
	This behaviour can be modified for every input file in the input script using the command \bold{REGIONS_APPEND}.
	The supported arguments are listed below:
	\tabular{ll}{
		Argument\tab Separator\cr
		OFF\tab Only the original columns with marker name,\cr
		\tab chromosome name and position are preserved.\cr
		\tab Columns with annotated regions are appended\cr
		\tab to the end.\cr
		ON\tab All the original columns are preserved and\cr
		\tab columns with annotated regions are appended\cr
		\tab to the end.
	}
	
	Example:
		\tabular{l}{
		REGIONS_FILE     genes_file_1.txt\cr
		REGIONS_APPEND   ON\cr
		PROCESS          input_file_1.txt\cr
		REGIONS_APPEND   OFF\cr
		PROCESS          input_file_2.csv\cr
	}
}
\section{Output File Name}{
	The output file names are created by adding a prefix to the input file names.	
	The prefix is specified with the command \bold{PREFIX}. 
	
	Example:
	\tabular{l}{
		REGIONS_FILE genes_file_1.txt\cr
		PREFIX       annotated_\cr
		PROCESS      input_file_1.txt\cr
		PROCESS      input_file_2.csv\cr
		PREFIX       annot_\cr
		PROCESS      input_file_3.tab\cr
	}
	All the output files corresponding to the input files \file{input_file_1.txt} and \file{input_file_2.csv} will be prefixed with \emph{annotated_}; 
	the output files corresponding to the input file \file{input_file_3.tab} will be prefixed with \emph{annot_}.
}
\author{Daniel Taliun, Christian Fuchsberger, Cristian Pattaro}
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
	
	# name of an input script
	script <- "ANNOTATE_script.txt"
	
	# load GWAtoolbox library
	library(GWAtoolbox)
	
	# show contents of the input script
	file.show(script, title=script)
	
	\dontshow{options(device.ask.default = FALSE)}
	
	# run annotate() function
	annotate(script)
	
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