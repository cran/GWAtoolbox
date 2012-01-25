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

.plot <- function(plots) {
	if ((!is.null(plots)) && is.vector(plots) && (length(plots) > 0)) {
		for (i in seq(1, length(plots), 1)) {
			plot <- plots[[i]]
			
			title <- plot[[1]]
			file <- plot[[2]]
			histograms <- plot[[3]]
			boxplots <- plot[[4]]
			qqplots <- plot[[5]]
			
			histograms_number <- 0
			boxplots_number <- 0
			qqplots_number <- 0
			
			if (!is.null(histograms) && is.vector(histograms)) {
				histograms_number <- length(histograms)
			}
			
			if (!is.null(boxplots) && is.vector(boxplots)) {
				boxplots_number <- length(boxplots)
			}
			
			if (!is.null(qqplots) && is.vector(qqplots)) {
				qqplots_number <- length(qqplots)
			}
			
			if (histograms_number == 2) {
				png(paste(file, ".png", sep=""), width = 1000, height = 600)
				def.par <- par(no.readonly = TRUE)
				layout(matrix(c(1,2), 2, 1, byrow = TRUE), heights = c(1, 1), widths = c(1, 1))
				
				par(mar = c(5, 3, 3, 1))
				plot(histograms[[1]], col = 2, main = paste(title, "\n", histograms[[1]][[8]]), xlab = "", axes = TRUE)
				par(mar = c(3, 3, 1, 1))
				plot(histograms[[2]], col = 2, main = histograms[[2]][[8]], xlab = "", axes = TRUE)
				
				par(def.par)
				dev.off()
			} else if ((histograms_number == 1) && (boxplots_number == 0)) {
				png(paste(file, ".png", sep = ""), width = 1000, height = 600)
				def.par <- par(no.readonly = TRUE)
				
				par(mar = c(3, 3, 1, 1))
				plot(histograms[[1]], col = 2, main = title, xlab = "", axes = TRUE)
				
				par(def.par)
				dev.off()				
			} else if ((histograms_number == 1) && (boxplots_number == 1)) {
				png(paste(file, ".png", sep = ""), width = 1000, height = 600)
				def.par <- par(no.readonly = TRUE)
				layout(matrix(c(1,2), 2, 1, byrow = TRUE), heights = c(1, 1), widths = c(1, 1))
				
				par(mar = c(0, 3, 1, 1))
				plot(histograms[[1]], col = 2, main = title, xlab = "", axes = FALSE)
				axis(2)
				
				quantiles <- round(boxplots[[1]][[2]], 3)
				
				par(mar = c(3, 3, 1, 1), cex = 0.7)
				bxp(boxplots[[1]][[1]], horizontal = TRUE, main = "", axes = FALSE, boxfill = 3)
				axis(1, at = quantiles, lab = quantiles)
				
				par(def.par)
				dev.off()				
			} else if ((histograms_number == 1) && (boxplots_number == 2)) {
				png(paste(file, ".png", sep=""), width = 1000, height = 600)
				def.par <- par(no.readonly = TRUE)
				layout(matrix(c(1,2,3), 3, 1, byrow = TRUE), heights = c(1, 1, 1), widths = c(1, 1, 1))
				
				par(mar = c(5, 3, 1, 1))
				
				low_break = min(histograms[[1]]$breaks)
				max_break = max(histograms[[1]]$breaks)
				
				if (abs(low_break) > abs(max_break)) {
					x <- c(low_break, abs(low_break))
				} else {
					x <- c(-max_break, max_break)
				}
				
				plot(histograms[[1]], col = 2, main = title, xlab = "", axes = TRUE, xlim=x)
				
				for (j in seq(1, boxplots_number, 1)) {
					quantiles <- round(boxplots[[j]][[2]], 3)
					par(mar = c(3, 3, 1, 1), cex = 0.7)
					
					low_whisker <- boxplots[[j]][[1]]$stats[1, 1]
					up_whisker <- boxplots[[j]][[1]]$stats[5, 1]
					
					if (length(boxplots[[j]][[1]]$out) == 0) {
						x <- c(low_whisker, up_whisker)
					} else {   
						outliers <- boxplots[[j]][[1]]$out[!is.infinite(boxplots[[j]][[1]]$out) & !is.nan(boxplots[[j]][[1]]$out) & !is.na(boxplots[[j]][[1]]$out)]
						
						if (length(outliers) == 0) {
							min_x <- low_whisker
							max_x <- up_whisker
						} else {
							min_x <- min(outliers)
							max_x <- max(outliers)
						}
						
						if (abs(min_x) > abs(max_x)) {
							x <- c(min_x, abs(min_x))
							quantiles <- append(quantiles, round(x, 3))
						} else {
							x <- c(-max_x, max_x)
							quantiles <- append(quantiles, round(x, 3))         
						}
					}
					
					suppressWarnings(bxp(boxplots[[j]][[1]], horizontal = TRUE, main = boxplots[[j]][[4]], axes = FALSE, boxfill = 3, ylim = x))
					axis(1, at = quantiles, lab = quantiles)  
				}
				
				par(def.par)
				dev.off()
			} else if (qqplots_number == 1) {
				png(paste(file, ".png", sep = ""), width = 500, height = 500)
				def.par <- par(no.readonly = TRUE)
				
				len <- length(qqplots[[1]])
				
				xaxis <- qqplots[[1]][[len - 1]]
				yaxis <- qqplots[[1]][[len]]
				
				name <- qqplots[[1]][[1]]
				color <- qqplots[[1]][[2]]
				lambda <- qqplots[[1]][[3]]
				points <- qqplots[[1]][[4]]
				x <- qqplots[[1]][[5]]
				y <- qqplots[[1]][[6]]
				
				text <- c()
				colors <- c()
				
				plot(x, y, col = color, xlab = expression(paste("Expected distribution, ", -log[10]('p-value'))), ylab = expression(paste("Observed distribution, ", -log[10]('p-value'))), main = "", pch = 20, cex = 1, xlim = c(0, max(xaxis)), ylim = c(0, max(yaxis)))
				
				for (j in seq(len, 1, -6)) {
					name = qqplots[[1]][[j - 5]]
					color = qqplots[[1]][[j - 4]]
					lambda = qqplots[[1]][[j - 3]]
					points = qqplots[[1]][[j - 2]]
					x = qqplots[[1]][[j - 1]]
					y = qqplots[[1]][[j]]
					
					text = c(text, as.expression(substitute(paste(a, "; N=", c, "; ", lambda, "=", b, sep = ""), list(a = name, b = format(round(lambda, 2), nsmall=2), c = points))))
					colors = c(colors, color)     
				}
				
				if (len >= 12) {
					for (j in seq(7, len, 6)) {
						name = qqplots[[1]][[j]]
						color = qqplots[[1]][[j + 1]]
						lambda = qqplots[[1]][[j + 2]]
						points = qqplots[[1]][[j + 3]]
						x = qqplots[[1]][[j + 4]]
						y = qqplots[[1]][[j + 5]]
						
						points(x, y, col = color, pch = 20, cex = 1)
					}
				}
				
				abline(0, 1, lty = 2)
				legend("bottomright", text, col = "black", pch = 21, pt.bg = colors, pt.cex=1.6, cex = 0.8, inset = 0.05, bg = "transparent")
				
				par(def.par)
				dev.off()
			}
		}
	}
}