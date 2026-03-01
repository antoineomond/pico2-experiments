library("ggplot2")
library("dplyr")
library(rlang)

freq <- 12000
tol <- 0.01
df <- read.csv("lposc_freqs.csv")
#df <- df[order(df$clock_freq), ]
#df <- df[df$clock_freq > freq & df$clock_freq < freq+freq*tol & df$range != "fa6", ]
#
#df[df$vreg == min(df$vreg),]
#df[df$vreg == max(df$vreg),]

#df[df$vco_freq == min(df$vco_freq),]
#df[df$vco_freq == max(df$vco_freq),]

df[df$clock_freq == min(df$clock_freq),]
