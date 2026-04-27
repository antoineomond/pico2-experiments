library("ggplot2")
library("ggrepel")
library("dplyr")
library(rlang)
library(patchwork)
library(stringr)
options(dplyr.print_max = 1e9, pillar.width = Inf)
#folder = "with_subsystems/"
folder = "debug/energy_benchmarks/"
#for (expe in c("minimums-pll.r")) {
for (expe in c("baseline.r", "minimums-pll.r", "minimums-rosc.r", "minimums-xosc.r", "minimums_lposc_default_freq.r", "minimums_lposc_min_freq.r", "minimums_lposc_max_freq.r")) {
	source(expe)
	#source("baseline.r")
	#source("minimums-pll.r")
	#source("minimums-rosc.r")
	#source("minimums-rosc-vregs.r")
	#source("minimums-xosc.r")
	#source("same_freqs-max_vco-def_vreg.r")
	#source("same_freqs-max_vco-min_vreg.r")
	#source("same_freqs-min_vco-min_vreg.r")
	#source("minimums_lposc_default_freq.r")
	#source("minimums_lposc_min_freq.r")
	#source("minimums_lposc_max_freq.r")

	# power
	legend_lookup <- tibble(
		expe_num = 0:max_expe_num,
		legend_group = rep(confs, each = 6)
	)
	df_by_legend_group <- df %>%
		left_join(legend_lookup, by = "expe_num") %>%
		mutate(legend_group = factor(legend_group, levels = confs))
	power_summary <- df_by_legend_group %>%
		group_by(legend_group) %>%
		summarise(
			power_median = median(power_sample, na.rm = TRUE),
			clock_freq = freqs[unique(legend_group)]
		)

	#df_by_legend_group <- df_by_legend_group %>% filter(expe_num >= 6 & expe_num < 12)
	#p1 <- ggplot(df_by_legend_group, aes(x = current_timestamp, y = power_sample, color=factor(expe_num), group=factor(expe_num))) +
	p1 <- ggplot(df_by_legend_group, aes(x = current_timestamp, y = power_sample, color=legend_group, group=legend_group)) +
		geom_line(na.rm = TRUE) +
		geom_text(data = power_summary, aes(x=x_power_median, y = power_median+y_power_median_offset, label = paste(round(power_median,2), "mW")), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
		geom_text(data = power_summary, aes(x=x_clock_freq, y = power_median+y_power_median_offset, label = paste(round(clock_freq/1000000, 2), clock_freq_unit)), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
		scale_y_continuous(limits=c(0, y_max), n.breaks=15) +
		geom_hline(data = power_summary, aes(yintercept = power_median, color = legend_group), linetype = "dashed") +
		labs(title = "", x = "Timestamp in seconds", y = "Power usage in mW") +
		scale_color_manual(name = "Processor clock:", values = clock_colors)

	# energy
	energy_consumption <- df %>%
		filter(expe_num %in% seq(5, max_expe_num, by=6), !is.na(energy_sample)) %>%
		# Associate expe name to expe_num
		group_by(expe_num) %>%
		mutate(expe_name = recode(expe_num, !!!names)) %>%
		mutate(expe_name = factor(expe_name, levels = confs)) %>%
		ungroup() %>%
		# Get accumulated energy for each expe
		group_by(iteration_num, expe_num) %>%
		filter(!is.na(energy_sample)) %>%
		slice_tail(n = 1) %>%   # last value per iteration/expe_num
		ungroup() %>%
		# compute avg_energy and energy_sample
		group_by(expe_num) %>%
		summarise(expe_name = expe_name, avg_energy = mean(energy_sample), std_energy = sd(energy_sample), .groups = "drop") %>%
		distinct()

	#energy_consumption

	p2 <- ggplot(energy_consumption, aes(x = expe_name, y = avg_energy, fill=expe_name)) +
		geom_bar(stat = "identity", width = 0.2) +
		geom_errorbar(aes(ymin = avg_energy - std_energy, ymax = avg_energy + std_energy), width = 0.2) +
		geom_text(aes(label = round(avg_energy), y = avg_energy + y_energy_offset)) +
		labs(x = "Processor clock",
			y = "Total energy consumption in mJ",
			title = ""
		) +
		#scale_x_discrete(labels = function(x) str_wrap(x, width = 7)) +
		scale_fill_manual(name = "Processor clock", values = clock_colors) +
		theme(aspect.ratio = 3/1, legend.position = "none", axis.text.x = element_text(angle = 45, size = 8, hjust = 1))
	
	p2 <- p2 + guides(color = "none", fill = "none", linetype = "none")
	
	combined_plot <- (p1 + p2) + 
  plot_layout(guides = "collect") & 
  theme(legend.position = "top")

	ggsave(paste(name, ".pdf", sep=""), plot=combined_plot)
}
