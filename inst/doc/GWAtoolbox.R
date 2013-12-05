### R code from vignette source 'GWAtoolbox.Rnw'

###################################################
### code chunk number 1: GWAtoolbox.Rnw:770-772
###################################################
t <- read.table("gwa_data_example_1.txt", header = T, nrow = 1, sep = " ")
colnames(t)


###################################################
### code chunk number 2: GWAtoolbox.Rnw:775-777
###################################################
t <- read.table("gwa_data_example_2.tbl", header = T, nrow = 1, sep = "\t")
colnames(t)


###################################################
### code chunk number 3: GWAtoolbox.Rnw:782-783
###################################################
cat(readLines("GWASQC_script.txt"), sep = "\n")


###################################################
### code chunk number 4: GWAtoolbox.Rnw:786-788
###################################################
library(GWAtoolbox)
gwasqc("GWASQC_script.txt")


###################################################
### code chunk number 5: GWAtoolbox.Rnw:852-855
###################################################
W <- kusk_check("GWASQC_script.txt", worst = c(50), plot = TRUE)
points(W$sk50[W$ku50 > 5], W$ku50[W$ku50 > 5], pch = 22, bg = 2, cex = 2)
text(W$sk50[W$ku50 > 5], W$ku50[W$ku50 > 5], labels = W$study[W$ku50 > 5], cex = 1, pos = 4)


###################################################
### code chunk number 6: GWAtoolbox.Rnw:881-882
###################################################
Z <- dispersion_check("GWASQC_script.txt", plot=TRUE)


###################################################
### code chunk number 7: GWAtoolbox.Rnw:884-885
###################################################
Z


###################################################
### code chunk number 8: GWAtoolbox.Rnw:887-888
###################################################
text(Z$median_n, Z$mean_se, labels=Z$study, pos=c(2,4,2,1,2))


###################################################
### code chunk number 9: GWAtoolbox.Rnw:890-891
###################################################
Z <- dispersion_check("GWASQC_script.txt", sample_sizes=c(1200, 200, 1000, 500, 700), plot=TRUE)


###################################################
### code chunk number 10: GWAtoolbox.Rnw:893-894
###################################################
Z


###################################################
### code chunk number 11: GWAtoolbox.Rnw:896-897
###################################################
text(Z$median_n, Z$mean_se, labels=Z$study, pos=c(2,4,2,1,2))


