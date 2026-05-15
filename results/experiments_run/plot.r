library("ggplot2")
library("ggrepel")
library("dplyr")
library(rlang)
library(patchwork)
library(stringr)
options(dplyr.print_max = 1e9, pillar.width = Inf)
baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z)\\.", "\\1 | ", lvls)) }
no_filter_f <- function(df_input) {
	return(list(df_input, c(""), "result", baseline_mapfunc))
}
baseline_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("PLL", "XOSC", "ROSC")) %>%
		filter(vreg_output %in% c("1.10V")) %>%
		filter(clock_freq %/% 1000000 %in% c(150, 12, 11))
	return(list(df_input, c("PLL", "XOSC", "ROSC"), "baseline", baseline_mapfunc))
}
pll_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("PLL")) %>%
		filter(vreg_output %in% c("1.10V", "0.90V")) %>%
		filter(clock_freq %/% 1000000 %in% c(150, 15))
	return(list(df_input, c("PLL (baseline)", "PLL (lowest f)", "PLL (lowest v)", "PLL (lowest f v)"), "minimums-pll", baseline_mapfunc))
}
rosc_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("ROSC")) %>%
		filter(vreg_output %in% c("1.10V", "0.80V")) %>%
		filter(clock_freq %/%1000000 %in% c(11, 2, 3, 1))
	return(list(df_input, c("ROSC (baseline)", "ROSC (lowest f)", "ROSC (lowest v)", "ROSC (lowest f v)"), "minimums-rosc", baseline_mapfunc))
}
xosc_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("XOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(clock_freq %/%1000000 %in% c(12))
	return(list(df_input, c("XOSC (baseline)", "XOSC (1.00V)", "XOSC (0.90V)", "ROSC (0.80V) (lowest v)"), "minimums-xosc", baseline_mapfunc))
}
lposc_dft_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("LPOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(lposc_trim %in% c(0x020))
	return(list(df_input, c("LPOSC 1.10V (baseline)", "LPOSC (1.00V)", "LPOSC (0.90V)", "LPOSC (0.80V) (lowest v)"), "minimums-lposc_default", baseline_mapfunc))
}
lposc_max_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("LPOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(lposc_trim %in% c(0x0f0))
	return(list(df_input, c("LPOSC 1.10V (baseline)", "LPOSC (1.00V)", "LPOSC (0.90V)", "LPOSC (0.80V) (lowest v)"), "minimums-lposc_max", baseline_mapfunc))
}
lposc_min_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("LPOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(lposc_trim %in% c(0x000))
	return(list(df_input, c("LPOSC 1.10V (baseline)", "LPOSC (1.00V)", "LPOSC (0.90V)", "LPOSC (0.80V) (lowest v)"), "minimums-lposc_min", baseline_mapfunc))
}
MHz <- 1000000
kHz <- 1000
args <- commandArgs(trailingOnly = TRUE)
folder <- args[1] 
last_benchmark <- "mat_mul_double"
name <- paste(folder, "results", sep="")
df <- read.csv(paste(name, ".csv", sep=""))
df <- df %>%
  mutate(config_row = expe_num + 1)
print(paste(folder, "configurations.csv", sep=""))
parameters <- read.csv(paste(folder, "configurations.csv", sep=""))
df <- df %>%
	left_join(
		parameters %>% mutate(config_row = row_number()),
		by = "config_row"
	) %>%
	select(-config_row)

#for(expe in c(baseline_f, pll_f, rosc_f, xosc_f, lposc_dft_f, lposc_max_f, lposc_min_f)) {
for(expe in c(no_filter_f)) {
	res <- expe(df)
	df_expe <- res[[1]]
	level_names <- res[[2]]
	pdf_name <- res[[3]]
	mapfunc <- res[[4]]
	df_expe <- df_expe %>%
		filter(conf_num == 0 | conf_num %% 2 != 0)
	
	# power
	power_summary <- df_expe %>%
		#group_by(clock_source, pll_vco_freq, clock_freq) %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		summarise(power_median = median(power_sample, na.rm = TRUE))
	unit <- if (max(df_expe$clock_freq, na.rm = TRUE) < MHz) "kHz" else "MHz"
	div <- if (max(df_expe$clock_freq, na.rm = TRUE) < MHz) kHz else MHz
	#df_expe$gp <- interaction(df_expe$clock_source, paste(round(df_expe$pll_vco_freq/div, 1), "MHz", sep=""), paste(round(df_expe$clock_freq/div, 1), unit, sep=""))
	df_expe$gp <- interaction(df_expe$clock_source, df_expe$vreg_output, paste(round(df_expe$clock_freq/div, 1), unit, sep=""))
	
	lvls <- levels(df_expe$gp)
	res_mapping <- setNames(mapfunc(lvls), lvls)
	df_expe$clock_source <- factor(df_expe$clock_source, levels = c("PLL", "XOSC", "ROSC", "LPOSC"))
	df_expe$vreg_output <- factor(df_expe$vreg_output, levels = c("1.25V", "1.20V", "1.15V", "1.10V", "1.05V", "1.00V", "0.95V", "0.90V", "0.85V", "0.80V"))
	#df_expe$gp <- factor(df_expe$gp, levels = unique(df_expe$gp[order(df_expe$clock_source, df_expe$pll_vco_freq, -df_expe$clock_freq)]))
	df_expe$gp <- factor(df_expe$gp, levels = unique(df_expe$gp[order(df_expe$clock_source, df_expe$vreg_output, -df_expe$clock_freq)]))
	
	mtimestamp <- max(df_expe$current_timestamp, na.rm = TRUE)
	p1 <- ggplot(df_expe , aes(x = current_timestamp, y = power_sample, color=gp, group=gp)) + 
		geom_line(na.rm = TRUE) +
		geom_hline(data = power_summary, aes(yintercept = power_median), linetype = "dashed") +
		geom_label_repel(data = power_summary, aes(x=mtimestamp*1.05, y = power_median, label = paste(round(power_median,2), "mW")), hjust = "left", show.legend = FALSE, inherit.aes = FALSE, direction = "y") +
		scale_x_continuous(expand = expansion(mult = c(0, 0.3))) +
		scale_y_continuous(n.breaks=15) +
		labs(x = "Timestamp in seconds", y = "Power usage in mW") +
		scale_color_discrete(name = "Configuration:", labels = res_mapping) +
		guides(color = guide_legend(nrow = 2, byrow = TRUE)) 

	# energy
	energy_consumption <- df_expe %>%
		filter(benchmark_name == last_benchmark, !is.na(energy_sample)) %>%
		group_by(iteration_num, expe_num) %>%
		slice_tail(n = 1) %>%   # last value per iteration/expe_num
		ungroup() %>%
		# compute avg_energy and energy_sample
		group_by(expe_num) %>%
		summarise(clock_source = clock_source, vreg_output = vreg_output, clock_freq = clock_freq, gp = gp, avg_energy = mean(energy_sample), std_energy = sd(energy_sample), avg_time = mean(current_timestamp), std_time = sd(current_timestamp), .groups = "drop") %>%
		distinct()
	p2 <- ggplot(energy_consumption , aes(x = gp, y = avg_energy, fill=gp)) +
		geom_bar(stat = "identity", width = 0.2) +
		geom_errorbar(aes(ymin = avg_energy - std_energy, ymax = avg_energy + std_energy), width = 0.2) +
		geom_text(aes(label = round(avg_energy), y = avg_energy, nudge_y = 50)) +
		labs(x = "Processor clock", y = "Total energy consumption in mJ", title = "") +
		scale_x_discrete(labels = res_mapping) +
		scale_fill_discrete(labels = res_mapping) +
		theme(aspect.ratio = 1.75/1, legend.position = "none", axis.text.x = element_text(angle = 45, size = 8, hjust = 1))

	p2 <- p2 + guides(color = "none", fill = "none", linetype = "none")
	combined_plot <- (p1 + p2) + 
	plot_layout(guides = "collect") & 
	theme(legend.position = "top")

	power_summary$gp <- interaction(power_summary$clock_source, power_summary$vreg_output, paste(round(power_summary$clock_freq/div, 1), unit, sep=""))
	write.csv(power_summary, paste(folder, "power_summary.csv", sep=""))
	write.csv(energy_consumption, paste(folder, "energy_consumption.csv", sep=""))
	ggsave(paste(folder, pdf_name, ".pdf", sep=""), plot=combined_plot, width = 14)
}
