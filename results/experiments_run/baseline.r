library("ggplot2")
library("dplyr")
library(rlang)
library(patchwork)

pll_freq <- 150000000
xosc_freq <- 12000000
rosc_freq <- 10887000
lposc_freq <- 20000
name <- "baseline_w_ref_7-7ohms"
df <- read.csv(paste(name, ".csv", sep=""))

# power
df_by_legend_group <- df %>%
  mutate(
    legend_group = case_when(
      expe_num %in% c(0:5)   ~ "PLL (150MHz)",
      expe_num %in% c(6:11)   ~ "XOSC (12MHz)",
      expe_num %in% c(12:17)   ~ "ROSC (11MHz)",
      expe_num %in% c(18:23)   ~ "LPOSC"
    )
  ) %>%
	mutate(legend_group = factor(legend_group, levels = c("PLL (150MHz)", "XOSC (12MHz)", "ROSC (11MHz)")))

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
  scale_color_manual(
		name = "Processor clock",
    values = c(
      "PLL (150MHz)"   = "black",
      "XOSC (12MHz)"  = "purple",
      "ROSC (11MHz)"  = "brown",
      "LPOSC" = "dark green"
    )
  )

energy_consumption <- df %>%
  filter(expe_num %in% c(5, 11, 17), !is.na(energy_sample)) %>%
  group_by(expe_num) %>%
  slice_tail(n = 1) %>%
  mutate(expe_name = recode(expe_num,
	 `5`  = "PLL",
	 `11` = "XOSC",
	 `17` = "ROSC")) %>%
	mutate(expe_name = factor(expe_name, levels = c("PLL", "XOSC", "ROSC"))) %>%
  ungroup()

# energy
p2 <- ggplot(energy_consumption, aes(x = expe_name, y = energy_sample, fill=expe_name)) +
  geom_bar(stat = "identity", width = 0.2) +
	geom_text(aes(label = round(energy_sample), y = energy_sample + 20)) +
  labs(x = "Processor clock",
	  y = "Total energy consumption in mJ",
	  title = ""
  ) +
  scale_fill_manual(name = "Processor clock",
    values = c(
      "PLL" = "black",
      "XOSC" = "purple",
      "ROSC" = "brown"
    )
  ) +
	theme(aspect.ratio = 4/1, legend.position = "none")

ggsave("baseline.pdf", plot=p1 + p2 + plot_layout(guides = 'collect'))

#df %>%
#	group_by(legend_group) %>%
#	summarise(median = median(power_sample, na.rm = TRUE))
