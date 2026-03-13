library("ggplot2")
library("dplyr")
library(rlang)

pll_freq <- 150000000
xosc_freq <- 12000000
rosc_freq <- 10887000
lposc_freq <- 20000
name <- "baseline_w_ref_7-7ohms"
df <- read.csv(paste(name, ".csv", sep=""))
df <- df %>%
  mutate(
    legend_group = case_when(
      expe_num %in% c(0:5)   ~ "PLL",
      expe_num %in% c(6:11)   ~ "XOSC",
      expe_num %in% c(12:17)   ~ "ROSC",
      expe_num %in% c(18:23)   ~ "LPOSC"
    )
  )
p <- ggplot(df, aes(x = current_timestamp, y = power_sample, color=legend_group, group=legend_group)) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(0, 85), n.breaks=15) +
	labs(title = "Pico 2 power usage trace running benchmarks according to\nprocessor clock", x = "Timestamp in seconds", y = "Power usage in mW") + 
  scale_color_manual(
		name = "Processor clock",
    values = c(
      "PLL"   = "black",
      "XOSC"  = "purple",
      "ROSC"  = "brown",
      "LPOSC" = "dark green"
    )
  )
ggsave(paste(name, "-power", ".pdf", sep=""), plot=p)

df %>%
	group_by(legend_group) %>%
	summarise(median = median(power_sample, na.rm = TRUE))
