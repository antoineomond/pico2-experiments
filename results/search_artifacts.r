library("dplyr")
folder = "wo_noop_20_iters/"
name <- paste(folder, "results.csv", sep="")
df <- read.csv(name)
df <- df %>%
  mutate(config_row = expe_num + 1) %>%
	mutate(row_num = row_number())
parameters <- read.csv(paste(folder, "configurations.csv", sep="")) 
df <- df %>%
	left_join(
		parameters %>% mutate(config_row = row_number()),
		by = "config_row"
	) %>%
	select(-config_row)

MHz <- 1000000
df_print <- df %>%
	filter(clock_source == "ROSC") %>%
	filter(vreg_output == "1.10V") %>%
	filter(clock_freq > 2*MHz & clock_freq < 3*MHz) %>%
	#filter(clock_freq >149*MHz) %>%
	filter(power_sample > 14)
print(df_print)

