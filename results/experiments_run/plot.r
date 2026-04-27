library("ggplot2")
library("ggrepel")
library("dplyr")
library(rlang)
library(patchwork)
library(stringr)
#options(dplyr.print_max = 1e9, pillar.width = Inf)
clock_colors <- c(
	"PLL"   = "black",
	"XOSC"  = "purple",
	"ROSC"  = "brown",
	"LPOSC" = "dark green"
)
freqs <- c(
	"PLL"   = 150000000,
	"XOSC"  = 12000000,
	"ROSC"  = 11100000,
	"PLL (lowest f)"    = 15429000, 
	"PLL (lowest v)"    = 150000000, 
	"PLL (lowest f v)"  = 15428000,
	"ROSC (lowest f)"   = 2937000, 
	"ROSC (lowest v)"   = 3927000, 
	"ROSC (lowest f v)" = 1051000,
	"XOSC (baseline)" = 12000000,
	"XOSC (1.00V)" = 12000000,
	"XOSC (0.90V)" = 12000000,
	"XOSC (0.80V) (lowest v)" = 12000000,
	"LPOSC 1.10V (baseline)" = 29000000,
	"LPOSC 1.00V"  = 30000000,
	"LPOSC 0.90V"   = 31000000,
	"LPOSC 0.80V"  = 33000000,
	"LPOSC 1.10V (baseline)" = 33000000,
	"LPOSC 1.00V"  = 34000000,
	"LPOSC 0.90V"   = 35000000,
	"LPOSC 0.80V"  = 36000000,
	"LPOSC 1.10V (baseline)" = 20000000,
	"LPOSC 1.00V"  = 21000000,
	"LPOSC 0.90V"   = 22000000,
	"LPOSC 0.80V"  = 23000000
)
x_power_median <- 60
y_power_median_offset <- 2
y_energy_offset <- 20
x_clock_freq <- 33
y_max <- 85
clock_freq_unit <- "MHz"
folder = "energy_benchmarks/"
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

df <- df %>% 
	filter(expe_num <= 17)
write.csv(df, "filtered.csv")

# power
power_summary <- df %>%
	group_by(clock_source) %>%
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

#energy_consumption

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
