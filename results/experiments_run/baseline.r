library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("results.csv")
df <- df %>%
  mutate(
    legend_group = case_when(
      expe_num %in% c(0:5)   ~ "PLL",
      expe_num %in% c(6:11)   ~ "XOSC",
      expe_num %in% c(12:17)   ~ "ROSC",
      expe_num %in% c(18:23)   ~ "LPOSC"
    )
  )
p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=legend_group, group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(0, 16), n.breaks=15) +
	#geom_hline(yintercept = median(df[df$expe_num == 0,]$current_sample, na.rm = TRUE), color = "red") +
	#facet_wrap(~ x_range) +
	labs(title = "Running all 5 benchmarks one after the other. \nFor each target frequency, the closest configurations with the lowest \nand highest vreg are selected.", x = "Timestamp in seconds", y = "Current sample in mA") + 
  scale_color_manual(
		name = "Configuration",
    values = c(
      "PLL"   = "black",
      "XOSC"  = "purple",
      "ROSC"  = "brown",
      "LPOSC" = "dark green"
    )
  )
ggsave("baseline.pdf", plot=p)

