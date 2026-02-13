library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("xosc_pairs_freqs.csv")
#df <- df %>%
#  mutate(
#    x_range = cut(
#      expe_num,
#      breaks = c(0, 15, 30),   # upper bound is exclusive by default
#      right = FALSE,              # [0,15), [15,30), ...
#      labels = c("non trimmed", "trimmed")
#    )
#  )
df <- df %>%
  mutate(
    legend_group = case_when(
      expe_num %in% c(0:5)   ~ "1.1V",
      expe_num %in% c(6:11)   ~ "1.05V",
      expe_num %in% c(12:17)   ~ "1.00V",
      expe_num %in% c(18:23)   ~ "0.95V",
      expe_num %in% c(24:29)   ~ "0.90V",
      expe_num %in% c(30:35)   ~ "0.85V",
      expe_num %in% c(36:41)   ~ "0.80V",
      expe_num %in% c(42:47)   ~ "0.75V",
    )
  )
#df <- df %>% filter(expe_num > 3)
p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=legend_group, group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(2, 16), n.breaks=15) +
	#geom_hline(yintercept = median(df[df$expe_num == 0,]$current_sample, na.rm = TRUE), color = "red") +
	#facet_wrap(~ x_range) +
	labs(title = "Running all 5 benchmarks one after the other. \nFor each target frequency, the closest configurations with the lowest \nand highest vreg are selected.", x = "Timestamp in seconds", y = "Current sample in mA") +
  scale_color_manual(
		name = "Configuration",
    values = c(
      "1.1V"      = "black",
      "1.05V"      = "purple",
      "1.00V"      = "brown",
      "0.95V"      = "dark green",
      "0.90V"      = "dark blue",
      "0.85V"      = "orange",
      "0.80V"      = "dark grey",
      "0.75V"      = "blue"
    )
  )
ggsave("xosc_pairs_freqs.pdf", plot=p)
