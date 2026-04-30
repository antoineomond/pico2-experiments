library("ggplot2")
library("ggrepel")
library("dplyr")
library(rlang)
library(patchwork)
library(stringr)
options(dplyr.print_max = 1e9, pillar.width = Inf)
baseline_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("PLL", "XOSC", "ROSC")) %>%
		filter(vreg_output %in% c("1.10V")) %>%
		filter(clock_freq.y %in% c("150001000", "12000000", "11029000"))
	#return(df_input)
	return(list(df_input, c("PLL", "XOSC", "ROSC")))
}
level_names <- c("PLL", "XOSC", "ROSC")
pll_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("PLL")) %>%
		filter(vreg_output %in% c("1.10V", "0.90V")) %>%
		filter(clock_freq.y%/%1000000 %in% c(150, 15))
	return(list(df_input, c("PLL (baseline)", "PLL (lowest f)", "PLL (lowest v)", "PLL (lowest f v)")))
}
rosc_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("ROSC")) %>%
		filter(vreg_output %in% c("1.10V", "0.80V")) %>%
		filter(clock_freq.y%/%1000000 %in% c(11, 2, 3, 1))
	return(df_input)
}
xosc_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("XOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(clock_freq.y%/%1000000 %in% c(12))
	return(df_input)
}
lposc_dft_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("LPOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(lposc_trim %in% c(0x020))
	return(df_input)
}
lposc_max_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("LPOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(lposc_trim %in% c(0x0f0))
	return(df_input)
}
lposc_min_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("LPOSC")) %>%
		filter(vreg_output %in% c("1.10V", "1.00V", "0.90V", "0.80V")) %>%
		filter(lposc_trim %in% c(0x000))
	return(df_input)
}
clock_colors <- c(
	"PLL"   = "black",
	"XOSC"  = "purple",
	"ROSC"  = "brown",
	"LPOSC" = "dark green"
)
y_energy_offset <- 20
x_clock_freq <- 33
clock_freq_unit <- "MHz"
folder = ""
name <- paste(folder, "results", sep="")
df <- read.csv(paste(name, ".csv", sep=""))
df <- df %>%
  mutate(config_row = expe_num + 1)
parameters <- read.csv("configurations.csv") 
df <- df %>%
	left_join(
		parameters %>% mutate(config_row = row_number()),
		by = "config_row"
	) %>%
	select(-config_row)

#write.csv(df, "woeifjwf.csv")
write.csv(df, "merged.csv")
res <- pll_f(df)
df <- res[[1]]
level_names <- res[[2]]
#df <- baseline_f(df)

# power
power_summary <- df %>%
	group_by(clock_source, vreg_output, clock_freq.y) %>%
	summarise(
		power_median = median(power_sample, na.rm = TRUE)
		#clock_freq = freqs[unique(legend_group)]
	)
mtimestamp <- max(df$current_timestamp, na.rm = TRUE)
df$gp <- interaction(df$clock_source, df$vreg_output, paste(round(df$clock_freq.y/1000000, 1), "MHz", sep=""))
#levels(df$gp) <- level_names
p1 <- ggplot(df, aes(x = current_timestamp, y = power_sample, color=gp, group=gp)) +
	geom_line(na.rm = TRUE) +
	geom_hline(data = power_summary, aes(yintercept = power_median), linetype = "dashed") +
	geom_label_repel(data = power_summary, aes(x=mtimestamp*1.05, y = power_median, label = paste(round(power_median,2), "mW")), hjust = "left", show.legend = FALSE, inherit.aes = FALSE, direction = "y") +
	#geom_text(data = power_summary, aes(x=x_clock_freq, y = power_median+y_power_median_offset, label = paste(round(clock_freq/1000000, 2), clock_freq_unit)), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
	#scale_y_continuous(limits=c(0, y_max), n.breaks=15) +
	scale_x_continuous(expand = expansion(mult = c(0, 0.3))) +
	scale_y_continuous(n.breaks=15) +
	labs(fill="Processor clock:", title = "", x = "Timestamp in seconds", y = "Power usage in mW") +
	scale_color_discrete(name = "Configuration:", labels = level_names)
	#scale_color_manual(name = "Processor clock:")

# energy
energy_consumption <- df %>%
	filter(benchmark_name == "mat_mul_double", !is.na(energy_sample)) %>%
	group_by(iteration_num, expe_num) %>%
	slice_tail(n = 1) %>%   # last value per iteration/expe_num
	ungroup() %>%
	# compute avg_energy and energy_sample
	group_by(expe_num) %>%
	summarise(gp = gp, avg_energy = mean(energy_sample), std_energy = sd(energy_sample), .groups = "drop") %>%
	distinct()

#write.csv(energy_consumption, "energy_consumption.csv") 

p2 <- ggplot(energy_consumption, aes(x = gp, y = avg_energy, fill=gp)) +
	geom_bar(stat = "identity", width = 0.2) +
	geom_errorbar(aes(ymin = avg_energy - std_energy, ymax = avg_energy + std_energy), width = 0.2) +
	geom_text(aes(label = round(avg_energy), y = avg_energy + y_energy_offset)) +
	labs(x = "Processor clock",
		y = "Total energy consumption in mJ",
		title = ""
	) +
	#scale_x_discrete(labels = function(x) str_wrap(x, width = 7)) +
	#scale_fill_manual(name = "Processor clock") +
	scale_color_discrete(labels = level_names) +
	theme(aspect.ratio = 3/1, legend.position = "none", axis.text.x = element_text(angle = 45, size = 8, hjust = 1))

p2 <- p2 + guides(color = "none", fill = "none", linetype = "none")

combined_plot <- (p1 + p2) + 
plot_layout(guides = "collect") & 
theme(legend.position = "top")

ggsave(paste(name, ".pdf", sep=""), plot=combined_plot)
