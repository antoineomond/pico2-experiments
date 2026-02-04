library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("lposc_benchmarks.csv")
df <- df %>%
  mutate(
    x_range = cut(
      expe_num,
      breaks = c(0, 15, 30),   # upper bound is exclusive by default
      right = FALSE,              # [0,15), [15,30), ...
      labels = c("non trimmed", "trimmed")
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
df <- df %>% filter(iteration_num == 0)
p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=legend_group, group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(0.4, 1.4), n.breaks=15) +
	geom_hline(yintercept = median(df[df$expe_num == 0,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 5,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 10,]$current_sample, na.rm = TRUE), color = "red") +
	facet_wrap(~ x_range) +
	labs(title = "Running all 5 benchmarks one after the other (prime, prime multicore, \nmat mul int, mat mul float, mat mul double)", x = "Timestamp in seconds", y = "Current sample in mA") +
  scale_color_manual(
		name = "Configuration",
    values = c(
      "1.1V (default)"      = "black",
      "0.9V"                = "purple",
      "0.75V"               = "darkgrey"
    )
  )
ggsave("lposc_benchmarks.pdf", plot=p)

