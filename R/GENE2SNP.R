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

gene2snp <- function(genes, snps, output, append, multicolumn) {
	genes <- as.character(genes)
	snps <- as.character(snps)
	output <- as.character(output)
	append <- as.logical(append)
	multicolumn <- as.logical(multicolumn)
	
	result = .Call("ANOT_gene_to_snp", genes, snps, output, append, multicolumn)
}