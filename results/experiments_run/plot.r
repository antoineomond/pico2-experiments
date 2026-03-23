library("ggplot2")
library("ggrepel")
library("dplyr")
library(rlang)
library(patchwork)
library(stringr)

#source("baseline.r")
source("minimums-pll.r")
#source("minimums-rosc.r")
#source("minimums-rosc-vregs.r")
#source("minimums-xosc.r")
#source("same_freqs-max_vco-def_vreg.r")
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
df_by_legend_group <- df_by_legend_group %>%
	group_by(legend_group) %>%
	mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
	mutate(clock_freq = freqs[legend_group]) %>%
	ungroup()

#df_by_legend_group <- df_by_legend_group %>% filter(expe_num >= 6 & expe_num < 12)
#p1 <- ggplot(df_by_legend_group, aes(x = current_timestamp, y = power_sample, color=factor(expe_num), group=factor(expe_num))) +
p1 <- ggplot(df_by_legend_group, aes(x = current_timestamp, y = power_sample, color=legend_group, group=legend_group)) +
	geom_line(na.rm = TRUE) +
	geom_text(aes(x=x_power_median, y = power_median+y_power_median_offset, label = paste(round(power_median,2), "mW")), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
	geom_text(aes(x=x_clock_freq, y = power_median+y_power_median_offset, label = paste(round(clock_freq/1000000, 2), "MHz")), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
	scale_y_continuous(limits=c(0, y_max), n.breaks=15) +
	geom_hline(aes(yintercept = power_median, color = legend_group, group = legend_group), linetype = "dashed") +
	labs(title = "", x = "Timestamp in seconds", y = "Power usage in mW") +
  scale_color_manual(name = "Processor clock", values = clock_colors)

# energy
energy_consumption <- df %>%
  filter(expe_num %in% seq(5, max_expe_num, by=6), !is.na(energy_sample)) %>%
  group_by(expe_num) %>%
  slice_tail(n = 1) %>%
  mutate(expe_name = recode(expe_num, !!!names)) %>%
	mutate(expe_name = factor(expe_name, levels = confs)) %>%
  ungroup()
p2 <- ggplot(energy_consumption, aes(x = expe_name, y = energy_sample, fill=expe_name)) +
  geom_bar(stat = "identity", width = 0.2) +
	geom_text(aes(label = round(energy_sample), y = energy_sample + y_energy_offset)) +
  labs(x = "Processor clock",
	  y = "Total energy consumption in mJ",
	  title = ""
  ) +
  #scale_x_discrete(labels = function(x) str_wrap(x, width = 7)) +
  scale_fill_manual(name = "Processor clock", values = clock_colors) +
	theme(aspect.ratio = 3/1, legend.position = "none", axis.text.x = element_text(angle = 45, size = 8, hjust = 1))

ggsave(paste("/home/aomond/research/projet_sensor_loic_2025/pico/paper_mcu/images/", name, ".pdf", sep=""), plot=p1 + p2 + plot_layout(guides = 'collect'))
