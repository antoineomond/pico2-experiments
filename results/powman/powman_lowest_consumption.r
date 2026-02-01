library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("powman_lowest_consumption.csv")
params <- c("iteration_num","expe_num","validation_result","clock_freq","current_sample","current_timestamp","timing_sample")

p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=factor(expe_num), group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(0, 1)) +
	geom_hline(yintercept = median(df$current_sample, na.rm = TRUE), color = "red") +
	labs(title = "", x = "Timestamp in seconds", y = "Current sample in mA") +
	scale_color_manual(name = "Configuration", labels = c("SRAM0 + SRAM1"), values = c("black"))
ggsave("powman_lowest_consumption.pdf", plot=p)

