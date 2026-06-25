library("dplyr")
library(gridExtra)
options(dplyr.print_max = 1e9, pillar.width = Inf)

MHz <- 1000000
kHz <- 1000
folder <- "lposc_rosc_timings/" 
last_benchmark <- "noop"
name <- paste(folder, "results", sep="")
df <- read.csv(paste(name, ".csv", sep=""))
df <- df %>%
  mutate(config_row = expe_num + 1)
parameters <- read.csv(paste(folder, "configurations.csv", sep=""))
df <- df %>%
	left_join(
		parameters %>% mutate(config_row = row_number()),
		by = "config_row"
	) %>%
	select(-config_row)
df <- df %>%
	filter(!is.na(timing_sample))

unit <- ifelse(df$clock_freq < MHz, "kHz", "MHz")
div  <- ifelse(df$clock_freq < MHz, kHz, MHz)
#df$gp <- interaction(df$clock_source, paste(round(df$pll_vco_freq/div, 1), "MHz", sep=""), paste(round(df$clock_freq/div, 1), unit, sep=""))
df$gp <- interaction(df$clock_source, df$vreg_output, paste(round(df$clock_freq/div, 1), unit, sep=""))
lvls <- levels(df$gp)
df$clock_source <- factor(df$clock_source, levels = c("PLL", "XOSC", "ROSC", "LPOSC"))
df$vreg_output <- factor(df$vreg_output, levels = c("1.25V", "1.20V", "1.15V", "1.10V", "1.05V", "1.00V", "0.95V", "0.90V", "0.85V", "0.80V"))
#df$gp <- factor(df$gp, levels = unique(df$gp[order(df$clock_source, df$pll_vco_freq, -df$clock_freq)]))
df$gp <- factor(df$gp, levels = unique(df$gp[order(df$clock_source, df$vreg_output, -df$clock_freq)]))

noop_time <- 10000000
timings <- df %>%
	filter(vreg_output == "1.10V") %>%
	group_by(iteration_num, expe_num) %>%
	slice_tail(n = 1) %>%   # last value per iteration/expe_num
	ungroup() %>%
	# compute avg_energy and energy_sample
	group_by(expe_num) %>%
	#summarise(clock_source = clock_source, vreg_output = vreg_output, clock_freq = clock_freq, gp = gp, avg_timings = mean(timing_sample), std_timings = sd(timing_sample), std_perc = round(sd(timing_sample)*100/mean(timing_sample), 2), perc_diff = (noop_time-mean(timing_sample))*100/noop_time, .groups = "drop") %>%
	summarise(clock_source = clock_source, vreg_output = vreg_output, clock_freq = clock_freq, gp = gp, avg_timings = mean(timing_sample), std_timings = sd(timing_sample), std_perc = round(sd(timing_sample)*100/mean(timing_sample), 2), perc_diff = round(abs((noop_time-mean(timing_sample))*100/noop_time), 2), .groups = "drop") %>%
	select(-vreg_output, -expe_num, -gp, -std_timings) %>%
	rename(
		"Clock source" = clock_source,
		"Clock frequency (Hz)" = clock_freq,
		"Mean time (us)" = avg_timings,
		"Relative std (%)" = std_perc,
		"Difference from 10s (%)" = perc_diff
	) %>%
	distinct()

pdf_name <- "timings"
pdf(paste(folder, pdf_name, "_table.pdf", sep=""), width = 8, height = 2)
grid.table(timings, rows = NULL)
