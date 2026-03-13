library("ggplot2")
library("dplyr")
library(rlang)
library(patchwork)

#source("baseline.r")
source("minimums.r")

# power
legend_lookup <- tibble(
  expe_num = 0:max_expe_num,
  legend_group = rep(
    confs,
    each = 6
  )
)
df_by_legend_group <- df %>%
	left_join(legend_lookup, by = "expe_num") %>%
	mutate(legend_group = factor(legend_group, levels = confs))
df_by_legend_group <- df_by_legend_group %>%
	group_by(legend_group) %>%
	mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
	ungroup()
	
p1 <- ggplot(df_by_legend_group, aes(x = current_timestamp, y = power_sample, color=legend_group, group=legend_group)) +
	geom_line(na.rm = TRUE) +
	geom_text(aes(x=43, y = power_median, label = round(power_median)), hjust = 1.1, vjust=-0.4, show.legend = FALSE) +
	scale_y_continuous(limits=c(0, 85), n.breaks=15) +
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
	geom_text(aes(label = round(energy_sample), y = energy_sample + 20)) +
  labs(x = "Processor clock",
	  y = "Total energy consumption in mJ",
	  title = ""
  ) +
  scale_fill_manual(name = "Processor clock", values = clock_colors) +
	theme(aspect.ratio = 4/1, legend.position = "none")

ggsave(paste(name, ".pdf", sep=""), plot=p1 + p2 + plot_layout(guides = 'collect'))

#df %>%
#	group_by(legend_group) %>%
#	summarise(median = median(power_sample, na.rm = TRUE))
