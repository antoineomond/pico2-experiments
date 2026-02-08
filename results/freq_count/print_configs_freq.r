library("ggplot2")
library("dplyr")
library(rlang)

freq <- 10000
tol <- 100
df <- read.csv("rosc_freqs.csv")
df <- df[order(df$clock_freq), ]
df <- df[df$clock_freq > freq & df$clock_freq < freq+tol, ]

df[df$vreg == min(df$vreg),]
df[df$vreg == max(df$vreg),]

