library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("pairs_freqs_manual.csv")
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
      expe_num %in% c(4:9, 10:15)   ~ "30MHz",
      expe_num %in% c(16:21, 22:27)   ~ "50MHz",
      expe_num %in% c(28:33, 34:39)   ~ "70MHz",
      expe_num %in% c(40:45, 46:51)   ~ "110MHz",
      expe_num %in% c(52:57, 58:63)   ~ "130MHz",
      expe_num %in% c(64:69, 70:75)   ~ "150MHz",
      expe_num %in% c(76:81, 82:87)   ~ "11MHz"
    )
  )
df <- df %>% filter(expe_num > 3)
p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=legend_group, group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	#scale_y_continuous(limits=c(0, 20), n.breaks=15) +
	#geom_hline(yintercept = median(df[df$expe_num == 0,]$current_sample, na.rm = TRUE), color = "red") +
	#facet_wrap(~ x_range) +
	labs(title = "Running all 5 benchmarks one after the other. \nFor each target frequency, the closest configurations with the lowest \nand highest vreg are selected.", x = "Timestamp in seconds", y = "Current sample in mA") +
  scale_color_manual(
		name = "Configuration",
    values = c(
      "30MHz"      = "black",
      "50MHz"      = "purple",
      "70MHz"      = "brown",
      "110MHz"      = "dark green",
      "130MHz"      = "dark blue",
      "150MHz"      = "orange",
      "11MHz"      = "dark grey"
    )
  )
ggsave("pairs_freqs_manual.pdf", plot=p)

