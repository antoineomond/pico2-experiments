library("ggplot2")
library("dplyr")
library(rlang)

freq <- 30000
tol <- 0.01
df <- read.csv("rosc_freqs.csv")
df <- df[order(df$clock_freq), ]
df <- df[df$clock_freq > freq-freq*tol & df$clock_freq < freq+freq*tol & df$range != "fa6", ]
#df <- df[df$clock_freq > freq-freq*tol & df$clock_freq < freq+freq*tol, ]

df[df$vreg == 5, ]
#df[df$vreg == max(df$vreg),]

#df[df$vco_freq < 960,]
#df[df$vco_freq == max(df$vco_freq),]

#df[df$clock_freq == min(df$clock_freq),]
