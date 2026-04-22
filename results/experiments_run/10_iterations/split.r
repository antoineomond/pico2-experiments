name <- "results"
df <- read.csv(paste(name, ".csv", sep=""))
#df <- df[df$iteration_num <= 6, ]

write.csv(df[df$expe_num >= 0 & df$expe_num <= 17, ], "baseline.csv")
pll <- df[df$expe_num >= 18 & df$expe_num <= 41, ]
pll$expe_num <- pll$expe_num - 18
write.csv(pll, "minimums-pll.csv")
rosc <- df[df$expe_num >= 42 & df$expe_num <= 65, ]
rosc$expe_num <- rosc$expe_num - 42
write.csv(rosc, "minimums-rosc.csv")
xosc <- df[df$expe_num >= 66 & df$expe_num <= 89, ]
xosc$expe_num <- xosc$expe_num - 66
write.csv(xosc, "minimums-xosc.csv")
lposc_default <- df[df$expe_num >= 90 & df$expe_num <= 113, ]
lposc_default$expe_num <- lposc_default$expe_num - 90
write.csv(lposc_default, "minimums_lposc_default_freq.csv")
lposc_max <- df[df$expe_num >= 114 & df$expe_num <= 137, ]
lposc_max$expe_num <- lposc_max$expe_num - 114
write.csv(lposc_max, "minimums_lposc_max_freq.csv")
lposc_min <- df[df$expe_num >= 138 & df$expe_num <= 161, ]
lposc_min$expe_num <- lposc_min$expe_num - 138
write.csv(lposc_min, "minimums_lposc_min_freq.csv")
#write.csv(df[df$expe_num >= 42 & df$expe_num <= 65, ], "minimums-rosc.csv")
#write.csv(df[df$expe_num >= 66 & df$expe_num <= 89, ], "minimums-xosc.csv")
#write.csv(df[df$expe_num >= 90 & df$expe_num <= 113, ], "minimums_lposc_default_freq.csv")
#write.csv(df[df$expe_num >= 114 & df$expe_num <= 137, ], "minimums_lposc_max_freq.csv")
#write.csv(df[df$expe_num >= 138 & df$expe_num <= 161, ], "minimums_lposc_min_freq.csv")
