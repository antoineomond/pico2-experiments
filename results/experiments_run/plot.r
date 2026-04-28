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
}
pll_f <- function(df_input) {
	df_input <- df_input %>%
		filter(clock_source %in% c("PLL")) %>%
		filter(vreg_output %in% c("1.10V", "0.90V")) %>%
		filter(clock_freq.y%/%1000000 %in% c(150, 15))
}
clock_colors <- c(
	"PLL"   = "black",
	"XOSC"  = "purple",
	"ROSC"  = "brown",
	"LPOSC" = "dark green"
)
x_power_median <- 60
y_power_median_offset <- 2
y_energy_offset <- 20
x_clock_freq <- 33
y_max <- 85
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
baseline_f(df)

# power
power_summary <- df %>%
	group_by(clock_source, vreg_output) %>%
	summarise(
		power_median = median(power_sample, na.rm = TRUE)
		#clock_freq = freqs[unique(legend_group)]
	)
p1 <- ggplot(df, aes(x = current_timestamp, y = power_sample, color=clock_source, group=clock_source)) +
	geom_line(na.rm = TRUE) +
	geom_text(data = power_summary, aes(x=x_power_median, y = power_median+y_power_median_offset, label = paste(round(power_median,2), "mW")), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
	geom_hline(data = power_summary, aes(yintercept = power_median, color = clock_source), linetype = "dashed") +
	#geom_text(data = power_summary, aes(x=x_clock_freq, y = power_median+y_power_median_offset, label = paste(round(clock_freq/1000000, 2), clock_freq_unit)), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
	scale_y_continuous(limits=c(0, y_max), n.breaks=15) +
	labs(title = "", x = "Timestamp in seconds", y = "Power usage in mW") +
	scale_color_manual(name = "Processor clock:", values = clock_colors)

# energy
energy_consumption <- df %>%
	filter(benchmark_name == "mat_mul_double", !is.na(energy_sample)) %>%
	group_by(iteration_num, expe_num) %>%
	slice_tail(n = 1) %>%   # last value per iteration/expe_num
	ungroup() %>%
	# compute avg_energy and energy_sample
	group_by(expe_num) %>%
	summarise(clock_source = clock_source, avg_energy = mean(energy_sample), std_energy = sd(energy_sample), .groups = "drop") %>%
	distinct()

#write.csv(energy_consumption, "energy_consumption.csv") 

p2 <- ggplot(energy_consumption, aes(x = clock_source, y = avg_energy, fill=clock_source)) +
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
