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

gwasqc <- function(script) {
	file_separator = ""

	if (.Platform$OS.type == "windows") {
		file_separator = "\\"
	}
	else {
		file_separator = .Platform$file.sep
	}

	script = .Call("QC_process_script", as.character(script), file_separator)

	resource_path = paste(.path.package("GWAtoolbox"), "extdata/", sep="/")

	files_number = script[[1]]
	files = script[[2]]
	file = 1

	while (file <= files_number) {
		plots = .Call("QC_process_file", files, file, resource_path)

		if ((!is.null(plots)) && (length(plots) > 0)) {
			cat("- Plotting...\n")

			plots_number = length(plots)
			progress=txtProgressBar(min = 0, max = plots_number, style = 3)
   
			time = system.time(
			for (i in seq(1, plots_number, 1)) { 
				plot = plots[[i]]
				histograms_number = 0
				boxplots_number = 0
				qqplots_number = 0    

				plot_title = plot[[1]]
				plot_file_name = plot[[2]]

				if (!is.null(plot[[3]])) {
					histograms_number = length(plot[[3]])
				}

				if (!is.null(plot[[4]])) {
					boxplots_number = length(plot[[4]])
				}

				if (!is.null(plot[[5]])) {
					qqplots_number = length(plot[[5]])
				}

				cat(paste(c("\r", rep.int(" ", getOption("width"))), collapse = ""))
				cat("\r     ", plot_file_name, ".png\n", sep="")
				flush.console()
				setTxtProgressBar(progress, i)

				if (histograms_number == 2) {
					png(paste(plot_file_name, ".png", sep=""), width = 1000, height = 600)
					def.par = par(no.readonly = TRUE)
					layout(matrix(c(1,2), 2, 1, byrow = TRUE), heights = c(1, 1), widths = c(1, 1))
                   
					par(mar = c(5, 3, 3, 1))
					plot(plot[[3]][[1]], col = 2, main = paste(plot_title, "\n", plot[[3]][[1]][[8]]), xlab = "", axes = TRUE)
					par(mar = c(3, 3, 1, 1))
					plot(plot[[3]][[2]], col = 2, main = plot[[3]][[2]][[8]], xlab = "", axes = TRUE)

					par(def.par)
					dev.off()
				}
				else if ((histograms_number == 1) && (boxplots_number == 0)) {
					png(paste(plot_file_name, ".png", sep = ""), width = 1000, height = 600)
					def.par = par(no.readonly = TRUE)

					par(mar = c(3, 3, 1, 1))
					plot(plot[[3]][[1]], col = 2, main = plot_title, xlab = "", axes = TRUE)

					par(def.par)
					dev.off()
				}
				else if ((histograms_number == 1) && (boxplots_number == 1)) {
					png(paste(plot_file_name, ".png", sep = ""), width = 1000, height = 600)
					def.par = par(no.readonly = TRUE)
					layout(matrix(c(1,2), 2, 1, byrow = TRUE), heights = c(1, 1), widths = c(1, 1))

					par(mar = c(0, 3, 1, 1))
					plot(plot[[3]][[1]], col = 2, main = plot_title, xlab = "", axes = FALSE)
					axis(2)
     
					quantiles = round(plot[[4]][[1]][[2]], 3)
     
					par(mar = c(3, 3, 1, 1), cex = 0.7)
					bxp(plot[[4]][[1]][[1]], horizontal = TRUE, main = "", axes = FALSE, boxfill = 3)
					axis(1, at = quantiles, lab = quantiles)

					par(def.par)
					dev.off()
				}
				else if ((histograms_number == 1) && (boxplots_number == 2)) {
					png(paste(plot_file_name, ".png", sep=""), width = 1000, height = 600)
					def.par = par(no.readonly = TRUE)
					layout(matrix(c(1,2,3), 3, 1, byrow = TRUE), heights = c(1, 1, 1), widths = c(1, 1, 1))

					par(mar = c(5, 3, 1, 1))

					low_break = min(plot[[3]][[1]]$breaks)
					max_break = max(plot[[3]][[1]]$breaks)

					if (abs(low_break) > abs(max_break)) {
						x = c(low_break, abs(low_break))
					}
					else {
						x = c(-max_break, max_break)
					}

					plot(plot[[3]][[1]], col = 2, main = plot_title, xlab = "", axes = TRUE, xlim=x)

					for (j in seq(1, boxplots_number, 1)) {
						quantiles = round(plot[[4]][[j]][[2]], 3)
						par(mar = c(3, 3, 1, 1), cex = 0.7)

						low_whisker = plot[[4]][[j]][[1]]$stats[1, 1]
						up_whisker = plot[[4]][[j]][[1]]$stats[5, 1]

						if (length(plot[[4]][[j]][[1]]$out) == 0) {
							x = c(low_whisker, up_whisker)
						}
						else {      
							min_out = min(plot[[4]][[j]][[1]]$out)
							max_out = max(plot[[4]][[j]][[1]]$out)      
	
							min_x = min(min_out, max_out, low_whisker, up_whisker)
							max_x = max(min_out, max_out, low_whisker, up_whisker)

							if (abs(min_x) > abs(max_x)) {
								x = c(min_x, abs(min_x))
								quantiles = append(quantiles, round(abs(min_x), 3))
							}
							else {
								x = c(-max_x, max_x)
								quantiles = append(quantiles, round(-max_x, 3))         
							}
						}

						bxp(plot[[4]][[j]][[1]], horizontal = TRUE, main = plot[[4]][[j]][[1]][[7]], axes = FALSE, boxfill = 3, ylim = x)
						axis(1, at = quantiles, lab = quantiles)  
					}

					par(def.par)
					dev.off()
				}
				else if (qqplots_number == 1) {
					png(paste(plot_file_name, ".png", sep = ""), width = 500, height = 500)
					def.par = par(no.readonly = TRUE)

					len = length(plot[[5]][[1]])
          
					xaxis = plot[[5]][[1]][[len - 1]]
					yaxis = plot[[5]][[1]][[len]]

					name = plot[[5]][[1]][[1]]
					color = plot[[5]][[1]][[2]]
					lambda = plot[[5]][[1]][[3]]
					points = plot[[5]][[1]][[4]]
					x = plot[[5]][[1]][[5]]
					y = plot[[5]][[1]][[6]]

					text = c()
					colors = c()

					plot(x, y, col = color, xlab = expression(paste("Expected distribution, ", -log[10]('p-value'))), ylab = expression(paste("Observed distribution, ", -log[10]('p-value'))), main = "", pch = 20, cex = 1, xlim = c(0, max(xaxis)), ylim = c(0, max(yaxis)))

					for (j in seq(len, 1, -6)) {
						name = plot[[5]][[1]][[j - 5]]
						color = plot[[5]][[1]][[j - 4]]
						lambda = plot[[5]][[1]][[j - 3]]
						points = plot[[5]][[1]][[j - 2]]
						x = plot[[5]][[1]][[j - 1]]
						y = plot[[5]][[1]][[j]]

						text = c(text, as.expression(substitute(paste(a, "; N=", c, "; ", lambda, "=", b, sep = ""), list(a = name, b = format(round(lambda, 2), nsmall=2), c = points))))
						colors = c(colors, color)     
					}

					if (len >= 12) {
						for (j in seq(7, len, 6)) {
							name = plot[[5]][[1]][[j]]
							color = plot[[5]][[1]][[j + 1]]
							lambda = plot[[5]][[1]][[j + 2]]
							points = plot[[5]][[1]][[j + 3]]
							x = plot[[5]][[1]][[j + 4]]
							y = plot[[5]][[1]][[j + 5]]

							points(x, y, col = color, pch = 20, cex = 1)
						}
					}

					abline(0, 1, lty = 2)
					legend("bottomright", text, col = "black", pch = 21, pt.bg = colors, pt.cex=1.6, cex = 0.8, inset = 0.05, bg = "transparent")

					par(def.par)
					dev.off()
				}
			}
			)

			cat(paste(c("\r", rep.int(" ", getOption("width"))), collapse=""))
			cat("\r  Done. (", time[3], " sec)\n", sep="")
			flush.console()
			close(progress)
		}
		else {
			cat("  No plots to plot.\n")
		}

		file = file + 1	 
	}

	cat("Comparison of effect distribution between files\n")
	plots = .Call("QC_common_plot", files, resource_path)

	if (!is.null(plots)) {
		cat("- Plotting... \n")
		progress<-txtProgressBar(min=0, max=length(plots) / 3, style=3)		

		time = system.time(
		for (i in seq(1, length(plots), 4)) {
			cat(paste(c("\r", rep.int(" ", getOption("width"))), collapse=""))
			cat("\r     ", "boxplots_", plots[[i]], ".png\n", sep="")
			flush.console()
			setTxtProgressBar(progress, (i + 2) / 3)

			png(paste("boxplots_", plots[[i]], ".png", sep=""), height=600, width=1200)    
			par(mar=c(8,5,2,1), las=3)
			bxp(plots[[i+3]], width=plots[[i+2]], main=plots[[i+1]], frame.plot=FALSE)
			abline(0,0, lty=2)
			dev.off()
		}
		)

		cat(paste(c("\r", rep.int(" ", getOption("width"))), collapse=""))
		cat("\r  Done. (", time[3], " sec)\n", sep="")
		flush.console()
	}
	else {
		cat("  No boxplots to plot.\n")
	}

	files = .Call("QC_delete", files)
}