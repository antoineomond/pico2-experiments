library("ggplot2")
library("dplyr")
library(rlang)

results_name <- "rosc_benchmarks_div1"
titles <- list(
	"rosc_benchmarks_div1" = "Rosc benchmarks, divider: 1",
	"rosc_benchmarks_div10" = "Rosc benchmarks, divider: 10",
	"rosc_benchmarks_div20" = "Rosc benchmarks, divider: 20",
	"rosc_benchmarks_div30" = "Rosc benchmarks, divider: 30"
)
limits <- list(
	"rosc_benchmarks_div1" = c(1, 10),
	"rosc_benchmarks_div10" = c(2, 3.5),
	"rosc_benchmarks_div20" = c(1.5, 3.5),
	"rosc_benchmarks_div30" = c(1, 3)
)

df <- read.csv(paste(results_name, ".csv", sep=""))
df <- df %>% filter(iteration_num == 0)
p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=factor(expe_num), group=factor(expe_num))) +
	geom_line(na.rm = TRUE) +
	scale_y_continuous(limits=limits[[results_name]], n.breaks=15) +
	labs(title = titles[[results_name]], x = "Timestamp in seconds", y = "Current sample in mA") +
	scale_color_manual(
	 name = "Benchmarks", 
	 labels = c(
			"prime", "prime_multicore", "mat_mul", "mat_mul_float", "mat_mul_double",
			"prime (0.9V)", "prime_multicore (0.9V)", "mat_mul (0.9V)", "mat_mul_float (0.9V)", "mat_mul_double (0.9V)",
			"prime (0.75V)", "prime_multicore (0.75V)", "mat_mul (0.75V)", "mat_mul_float (0.75V)", "mat_mul_double (0.75V)",
			"prime (trimmed, 0.9V)", "prime_multicore (trimmed, 0.9V)", "mat_mul (trimmed, 0.9V)", "mat_mul_float (trimmed, 0.9V)", "mat_mul_double (trimmed, 0.9V)",
			"prime (trimmed)", "prime_multicore (trimmed)", "mat_mul (trimmed)", "mat_mul_float (trimmed)", "mat_mul_double (trimmed)",
			"prime (trimmed, 0.75V)", "prime_multicore (trimmed, 0.75V)", "mat_mul (trimmed, 0.75V)", "mat_mul_float (trimmed, 0.75V)", "mat_mul_double (trimmed, 0.75V)"
		),
	 values = c(
			"black", "purple", "dark grey", "blue", "brown",
			"green", "green", "green", "green", "green",
			"yellow", "yellow", "yellow", "yellow", "yellow",
			"blue", "blue", "blue", "blue", "blue",
			"brown", "brown", "brown", "brown", "brown",
			"grey", "grey", "grey", "grey", "grey"
		))
ggsave(paste(results_name, ".pdf", sep=""), plot=p)

