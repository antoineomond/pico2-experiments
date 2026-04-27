name <- paste(folder, "minimums-pll", sep="")
confs <- c(
	"PLL (baseline)",
	"PLL (lowest f)",
	"PLL (lowest v)",
	"PLL (lowest f v)"
)
names = c(
	"5"  = "PLL (baseline)",
	"11" = "PLL (lowest f)",
	"17" = "PLL (lowest v)",
	"23" = "PLL (lowest f v)"
)
clock_colors <- c(
	"PLL (baseline)" = "black",
	"PLL (lowest f)"   = "orange",
	"PLL (lowest v)"  = "brown",
	"PLL (lowest f v)" = "grey"
)
freqs <- c(
	"PLL (baseline)" = 150000000, 
	"PLL (lowest f)"    = 15429000, 
	"PLL (lowest v)"    = 150000000, 
	"PLL (lowest f v)"  = 15428000
)
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 23

df <- df %>%
	filter(expe_num > 5)

df_baseline <- read.csv(paste(folder, "baseline.csv", sep=""))
df_baseline <- df_baseline %>%
	filter(expe_num < 6)
#df_baseline$expe_num <- df_baseline$expe_num + max_expe_num+1 
df <- rbind(df, df_baseline)
#df <- df %>% filter(expe_num < max_expe_num+6)
#max_expe_num <- max_expe_num+6
#df$expe_num[df$expe_num == 52] <- 53 # slight hack due to pll expes doing mat mut too fast yielding no record in the csv (to fix)
x_power_median <- 60
y_power_median_offset <- 5
y_energy_offset <- 20
x_clock_freq <- 33
y_max <- 85
clock_freq_unit <- "MHz"
