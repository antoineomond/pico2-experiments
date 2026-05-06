library("dplyr")
folder = ""
name <- paste(folder, "results", sep="")
df <- read.csv(paste(name, ".csv", sep=""))
df <- df %>%
  mutate(config_row = expe_num + 1) %>%
	mutate(row_num = row_number())
parameters <- read.csv("configurations.csv") 
df <- df %>%
	left_join(
		parameters %>% mutate(config_row = row_number()),
		by = "config_row"
	) %>%
	select(-config_row)

MHz <- 1000000
df_print <- df %>%
	filter(clock_source == "ROSC") %>%
	filter(vreg_output == "0.80V") %>%
	filter(clock_freq > 3*MHz & clock_freq < 4*MHz) %>%
	filter(power_sample > 8)
print(df_print)
