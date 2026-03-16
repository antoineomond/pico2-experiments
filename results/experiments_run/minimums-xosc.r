name <- "minimums-xosc"
confs <- c(
	"XOSC (min v)",
	"XOSC (baseline)"
)
names = c(
	"5" = "XOSC (min v)",
	"11" = "XOSC (baseline)"
)
clock_colors <- c(
	"XOSC (min v)" = "dark green",
	"XOSC (baseline)" = "black"
)
freqs <- c(
	"XOSC (min v)"   = 12000000, 
	"XOSC (baseline)" = 12000000
)
max_expe_num <- 5
df <- read.csv(paste(name, ".csv", sep=""))
df <- df %>%
	filter(expe_num > 5) %>%
	mutate(expe_num = expe_num - 6)
df_baseline <- read.csv("baseline.csv")
df_baseline <- df_baseline %>%
	filter(expe_num %in% c(6:11))
df <- rbind(df, df_baseline)
max_expe_num <- max_expe_num+6
x_power_median <- 43
y_power_median_offset <- 5
x_clock_freq <- 33
y_max <- 85
