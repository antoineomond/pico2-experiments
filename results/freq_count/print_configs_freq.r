library("ggplot2")
library("dplyr")
library(rlang)
options(dplyr.print_max = 1e9, width = 200)
MHz <- 1000000

print_freq <- function() {
	freq <- 30000
	tol <- 0.01
	#df <- read.csv("rosc_freqs.csv")
	df <- read.csv("pll_freqs.csv")
	df <- df[order(df$clock_freq), ]
	#df <- df[df$clock_freq > freq-freq*tol & df$clock_freq < freq+freq*tol & df$range != "fa6", ]
	df <- df[df$clock_freq > freq-freq*tol & df$clock_freq < freq+freq*tol, ]

	#df[df$vreg == 5, ]
	#df[df$vreg == max(df$vreg),]

	#df[df$vco_freq < 960,]
	df[df$vco_freq == max(df$vco_freq),]
	df[df$vco_freq == min(df$vco_freq),]

	#df[df$clock_freq == min(df$clock_freq),]
} 

show_freq_range <- function() {
	df <- read.csv("pll_freqs.csv")
	df <- df[order(df$clock_freq), ]
	for(target_freq in c(200, 150, 140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10)) {
		print(df[df$clock_freq == target_freq*1000, ])
	}

	#df[df$clock_freq == min(df$clock_freq),]
} 

show_max_freq_range_rosc <- function() {
	df <- read.csv("max_freq_vreg_rosc/freqs.csv")
	df <- df[order(df$clock_freq), ]
	tol <- 0.10
	df <- df %>%
		group_by(clock_source,vreg,divider,range,freqa,freqb) %>%
		mutate(
			mean_clk = mean(clock_freq, na.rm = TRUE),
			sd_clk = sd(clock_freq, na.rm = TRUE),
			n = n()
		)
	max_freq <- df %>%
		group_by(vreg) %>%
		slice_max(clock_freq, n = 1)
	#aggregate(clock_freq ~ vreg, data = df, FUN = max)
	write.csv(max_freq, "max_freq_vreg_rosc/max_freq.csv")
	
	#for(vreg in c(6,7,8,9,10,11,12,13,14)) {
	#	df_vreg <- df[df$vreg == vreg,]
	#	max_freq <-  df_vreg[df_vreg$clock_freq == max(df_vreg$clock_freq), ]
	#}
}


show_freq_range_rosc <- function() {
	df <- read.csv("max_freq_vreg_rosc/freqs.csv")
	df <- df[order(df$clock_freq), ]
	tol <- 0.01
	df <- df %>%
		group_by(clock_source,vreg,divider,range,freqa,freqb) %>%
		mutate(
			mean_clk = mean(clock_freq, na.rm = TRUE),
			sd_clk = sd(clock_freq, na.rm = TRUE),
			n = n()
		)
	for(target_freq in c(240)) {
		freq <- target_freq*1000
		print(df[df$iteration == 0 & df$clock_freq > freq-freq*tol & df$clock_freq < freq+freq*tol, ])
	}

	#df[df$clock_freq == min(df$clock_freq),]
} 

print_freq_low_vreg <- function() {
	df <- read.csv("max_freq_vreg_rosc/freqs.csv")
	targets <- c(250, 240, 230, 220, 210, 200, 190, 180, 170, 160, 150, 140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20)
	tol <- 0.03
	tol_str <- paste(tol*100, "%",sep="")
	result <- bind_rows(lapply(targets, function(t) {
  df %>%
    mutate(
      target = t*1000,
      distance = abs(clock_freq - t*1000),
			tolerance = tol_str
    ) %>%
    arrange(vreg, distance) %>%
		filter(clock_freq <= target + target*tol & clock_freq >= target - target*tol) %>%
		slice(1)
	}))
	print(tol_str)
	print(result)
	write.csv(result, "max_freq_vreg_rosc/160-250MHz.csv")
}

print_freq_low_vreg()
