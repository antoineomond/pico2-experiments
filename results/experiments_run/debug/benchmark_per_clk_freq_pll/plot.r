library("ggplot2")
library("ggrepel")
library("dplyr")
library(rlang)
library(patchwork)
library(stringr)
options(dplyr.print_max = 1e9, pillar.width = Inf)
folder = ""

name <- paste(folder, "mat_mul_float", sep="")
confs <- c("PLL 150", "PLL 125", "PLL 100", "PLL 75", "PLL 50")
names = c(
	"1"  = "PLL 150",
	"2"  = "PLL 125",
	"3"  = "PLL 100",
	"4"  = "PLL 75",
	"5"  = "PLL 50"
)
clock_colors <- c(
	"PLL 150" = "black",
	"PLL 125" = "purple",
	"PLL 100" = "brown",
	"PLL 75"  = "dark green",
	"PLL 50"  = "orange"
) 
freqs <- c(
	"PLL 150" = 150000000,
	"PLL 125" = 125000000,
	"PLL 100" = 100000000,
	"PLL 75"  = 75000000,
	"PLL 50"  = 50000000
)
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 5
x_power_median <- 60
y_power_median_offset <- 2
y_energy_offset <- 20
x_clock_freq <- 33
y_max <- 85
clock_freq_unit <- "MHz"

#df_by_legend_group <- df_by_legend_group %>% filter(expe_num >= 6 & expe_num < 12)
#p1 <- ggplot(df_by_legend_group, aes(x = current_timestamp, y = power_sample, color=factor(expe_num), group=factor(expe_num))) +
p1 <- ggplot(df, aes(x = current_timestamp, y = power_sample, color=expe_num, group=expe_num)) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(0, y_max), n.breaks=15) +
	labs(title = "", x = "Timestamp in seconds", y = "Power usage in mW")

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
