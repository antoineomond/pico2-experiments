library("ggplot2")
library("dplyr")
library(rlang)

df <- read.csv("pairs_freqs.csv")
df <- df %>% filter(iteration_num == 0)

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

p <- ggplot(df, aes(x = current_timestamp, y = current_sample, color=benchmark_name)) +
	geom_line(data=df %>% filter(clock_freq == 150429), aes(x = current_timestamp, y = current_sample), size = 0.5, na.rm = TRUE) +
	geom_line(data=df %>% filter(clock_freq == 150324), aes(x = current_timestamp, y = current_sample), size = 0.5, na.rm = TRUE) +
	#geom_line(clock_freq == 150429, na.rm = TRUE) +
  #facet_wrap(~value_group) +
	labs(title = "Running all 5 benchmarks one after the other (prime, prime multicore, \nmat mul int, mat mul float, mat mul double)", x = "Timestamp in seconds", y = "Current sample in mA")

ggsave("pairs_freqs.pdf", plot=p)

