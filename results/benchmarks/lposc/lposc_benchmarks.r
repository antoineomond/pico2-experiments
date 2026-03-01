library("ggplot2")
library("dplyr")
library(rlang)

voltage <- 5.2
df <- read.csv("lposc_benchmarks.csv")
df <- df %>%
  mutate(
    x_range = cut(
      expe_num,
      breaks = c(0, 15, 30),   # upper bound is exclusive by default
      right = FALSE,              # [0,15), [15,30), ...
      labels = c("Not trimmed", "Trimmed")
    )
  )
df <- df %>%
  mutate(
    legend_group = case_when(
      expe_num %in% c(0:4, 20:24)   ~ "1.1V (default)",
      expe_num %in% c(5:9, 15:19)   ~ "0.9V",
      expe_num %in% c(10:14, 25:29) ~ "0.75V"
    )
  )
df$legend_group <- factor(df$legend_group, levels=c("1.1V (default)", "0.9V", "0.75V"))
df <- df %>% filter(iteration_num == 0)
p <- ggplot(df, aes(x = current_timestamp, y = current_sample*voltage, color=legend_group, group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(2.5, 6), n.breaks=15) +
	facet_wrap(~ x_range) +
	labs(title = "", x = "Time (seconds)", y = "Power (mW)") +
  scale_color_manual(
		name = "VREG output",
    values = c(
      "1.1V (default)"      = "black",
      "0.9V"                = "purple",
      "0.75V"               = "darkgrey"
    )
  )
ggsave("lposc_benchmarks.pdf", plot=p)

