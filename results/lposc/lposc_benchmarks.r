library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("lposc_benchmarks.csv")
params <- c("iteration_num","expe_num","validation_result","clock_freq","current_sample","current_timestamp","timing_sample")

p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=factor(expe_num), group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=c(0.4, 3), n.breaks=15) +
	geom_hline(yintercept = median(df[df$expe_num == 0,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 5,]$current_sample, na.rm = TRUE), color = "red") +
	geom_hline(yintercept = median(df[df$expe_num == 10,]$current_sample, na.rm = TRUE), color = "red") +
	labs(title = "lposc benchmarks", x = "Timestamp in seconds", y = "Current sample in mA") +
	scale_color_manual(name = "Dormant source", 
										 labels = c(
												"prime", "prime_multicore", "mat_mul", "mat_mul_float", "mat_mul_double",
												"prime (0.9V)", "prime_multicore (0.9V)", "mat_mul (0.9V)", "mat_mul_float (0.9V)", "mat_mul_double (0.9V)",
												"prime (0.75V)", "prime_multicore (0.75V)", "mat_mul (0.75V)", "mat_mul_float (0.75V)", "mat_mul_double (0.75V)"
											), 
										 values = c(
												"black", "black", "black", "black", "black",
												"purple", "purple", "purple", "purple", "purple",
												"dark grey", "dark grey", "dark grey", "dark grey", "dark grey"
											))
ggsave("lposc_benchmarks.pdf", plot=p)

