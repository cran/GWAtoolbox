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

snp2geneHS <- function(chromosome, snps, ld, hotspots, genes, prefix) {
	chromosome <- as.character(chromosome)
	snps <- as.character(snps)
	ld <- as.character(ld)
	hotspots <- as.character(hotspots)
	genes <- as.character(genes)
	prefix <- as.character(prefix)

	result = .Call("ANOT_snp_to_gene_hs", chromosome, snps, ld, hotspots, genes, prefix)
}