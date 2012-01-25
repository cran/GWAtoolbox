#
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
#

.plot_combined_boxplots <- function(plots) {
	paths <- NULL
	
	if ((!is.null(plots)) && is.vector(plots) && (length(plots) > 0)) {	
		names <- c()
		for (i in seq(1, length(plots), 1)) {
			plots_per_file <- plots[[i]]
			if (!is.null(plots_per_file) && (length(plots_per_file) > 0)) {
				for (j in seq(1, length(plots_per_file), 1)) {
					plot <- plots_per_file[[j]]
					if (!is.null(plot) && (length(plot) == 5)) {
						names <- union(names, plot[[2]])
					}
				}
			}
		}
		
		if (!is.null(names) && (length(names) > 0)) {
			for (i in seq(1, length(names), 1)) {			
				common_boxplots <- c()
				widths <- c()
				
				for (j in seq(1, length(plots), 1)) {
					plots_per_file <- plots[[j]]
					if (!is.null(plots_per_file) && (length(plots_per_file) > 0)) {
						for (n in seq(1, length(plots_per_file), 1)) {
							plot <- plots_per_file[[n]]
							if (!is.null(plot) && (length(plot) == 5) && (names[[i]] == plot[[2]])) {
								title <- plot[[1]]
								file_boxplots <- plot[[4]]
								if (!is.null(file_boxplots) && (length(file_boxplots) > 0)) {
									for (m in seq(1, length(file_boxplots), 1)) {
										file_boxplot <- file_boxplots[[m]]									
										if (!is.null(file_boxplot) && (length(file_boxplot) == 4)) {
											common_boxplots <- append(common_boxplots, list(file_boxplot[[1]]))
											widths <- c(widths, file_boxplot[[3]])
										}
									}
								}
							}
						}
					}	
				}
					
				if (!is.null(common_boxplots) && (length(common_boxplots) > 0)) {
					if (!is.null(widths) && (length(widths) == length(common_boxplots)) && 
							(length(widths[is.na(widths) || is.nan(widths) || is.infinite(widths)]) == 0)) {
						common_boxplots <- common_boxplots[order(widths)]
						widths <- widths[order(widths)]
					} else {
						widths <- rep(1, length(common_boxplots))
					}
					
					bxp_obj <- list(stats=matrix(nrow=5, ncol=0), n=numeric(), conf=matrix(nrow=2, ncol=0), out=numeric(), group=numeric(), names=numeric())
					
					for (j in seq(1, length(common_boxplots), 1)) {
						bxp_obj$stats <- cbind(bxp_obj$stats, common_boxplots[[j]]$stats)
						bxp_obj$n <- c(bxp_obj$n, common_boxplots[[j]]$n)
						bxp_obj$conf <- cbind(bxp_obj$conf, common_boxplots[[j]]$conf)
						bxp_obj$out <- c(bxp_obj$out, common_boxplots[[j]]$out)
						bxp_obj$group <- c(bxp_obj$group, rep(length(bxp_obj$n), length(common_boxplots[[j]]$out)))
						bxp_obj$names <- c(bxp_obj$names, common_boxplots[[j]]$names)
					}
					
					paths <- c(paths, paste(names[[i]], ".png", sep=""))
					png(tail(paths, 1), height=600, width=1200)
					par(mar=c(8,5,2,1), las=3)
					suppressWarnings(bxp(bxp_obj, width=widths, main=title, frame.plot=FALSE))
					abline(0,0, lty=2)
					dev.off()
				}	
			}
		}
	}
	
	return(paths)
}