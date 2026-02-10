library("ggplot2")
library("dplyr")
library(rlang)

freq <- 11000
tol <- 0.01
df <- read.csv("rosc_freqs.csv")
df <- df[order(df$clock_freq), ]
df <- df[df$clock_freq > freq & df$clock_freq < freq+freq*tol & df$range != "fa6", ]

df[df$vreg == min(df$vreg),]
df[df$vreg == max(df$vreg),]
