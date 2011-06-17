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

snp2gene <- function(genes, snps, output, delimiter, deviations, append) {
	genes <- as.character(genes)
	snps <- as.character(snps)
	output <- as.character(output)
	append <- as.logical(append)
	delimiter <- as.character(delimiter)

	deviations <- as.vector(deviations, mode="numeric")
	deviations <- unique(deviations)
	deviations <- deviations[!is.na(deviations) & !is.infinite(deviations) & (deviations >= 0)]
	deviations <- as.vector(deviations, mode="integer")

	result = .Call("ANOT_snp_to_gene", genes, snps, output, append, delimiter, deviations)
}