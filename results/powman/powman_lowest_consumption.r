# commit 81d1c2ca51724c5aedbe3bcae8f4897cf7343228
library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("powman_lowest_consumption.csv")
params <- c("iteration_num","expe_num","validation_result","clock_freq","current_sample","current_timestamp","timing_sample")

p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=factor(expe_num), group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(0, 1)) +
	geom_hline(yintercept = median(df[df$expe_num == 0,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 1,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 2,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 3,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 4,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 5,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 6,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 7,]$current_sample, na.rm = TRUE), color = "red") +
	labs(title = "Deactivation of all sub-systems (deep sleep)", x = "Timestamp in seconds", y = "Current sample in mA") +
	scale_color_manual(name = "Deactivation", labels = c("SRAM0+SRAM1", "None", "SRAM1", "SRAM0", "SRAM0+SRAM1 (1V)", "SRAM0+SRAM1 (0.9V)", "SRAM0+SRAM1 (0.8V)", "SRAM0+SRAM1 (0.7V)"), values = c("black", "purple", "dark grey", "dark green", "blue", "green", "grey", "brown"))
ggsave("powman_lowest_consumption.pdf", plot=p)

