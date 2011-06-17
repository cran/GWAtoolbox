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


compare <- function(script_file, pval_threshold, maf_threshold) {
	script_file <- as.character(script_file)
	pval_threshold <- as.double(pval_threshold)
	maf_threshold <- as.double(maf_threshold)

	result = .Call("CMP_compare", script_file, pval_threshold, maf_threshold)
}