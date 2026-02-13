library("ggplot2")
library("dplyr")
library(rlang)

name <- "pairs_freqs_reboot_3_iters"
df <- read.csv(paste(name, ".csv", sep=""))
df <- df %>% filter(is.na(clock_freq) | (!is.na(clock_freq) & iteration_num == 0))

# Fill in clock freq by parameters
lookup <- df %>%
	filter(!is.na(df$clock_freq)) %>%
	select(vreg,lposc_trim,rosc_div,rosc_range,rosc_freqa,rosc_freqb,clock_freq)
df <- df %>%
  left_join(lookup, by = c("vreg","lposc_trim","rosc_div","rosc_range","rosc_freqa","rosc_freqb"), suffix = c("", ".new")) %>%
  mutate(clock_freq = ifelse(is.na(clock_freq), clock_freq.new, clock_freq)) %>%
  select(-clock_freq.new)
#df <- df %>%
#	mutate(value_group = cut(clock_freq, breaks = seq(30000, 151000, by = 10000)))

iter_num <- 2
df <- df %>% filter(iteration_num == iter_num)
write.csv(df, "filtered.csv")
p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=benchmark_name)) +
	geom_line(data=df %>% filter(clock_freq == 50452), aes(x = current_timestamp, y = current_sample), size = 0.5, na.rm = TRUE) +
	geom_line(data=df %>% filter(clock_freq == 49519), aes(x = current_timestamp, y = current_sample), size = 0.5, na.rm = TRUE) +
	#geom_line(clock_freq == 150429, na.rm = TRUE) +
  facet_wrap(~iteration_num) +
	labs(title = "Running all 5 benchmarks one after the other (prime, prime multicore, \nmat mul int, mat mul float, mat mul double)", x = "Timestamp in seconds", y = "Current sample in mA")

ggsave(paste(name, iter_num, ".pdf", sep=""), plot=p)

