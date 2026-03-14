name <- "minimums"
confs <- c(
	"PLL (min f)",
	"ROSC (min f)",
	"PLL (min v)",
	"XOSC (min v)",
	"ROSC (min v)",
	"PLL (min f v)",
	"XOSC (min f v)",
	"ROSC (min f v)",
	"PLL (baseline)",
	"XOSC (baseline)",
	"ROSC (baseline)"
)
names = c(
	"5"  = "PLL (min f)",
	"11" = "ROSC (min f)",
	"17" = "PLL (min v)",
	"23" = "XOSC (min v)",
	"29" = "ROSC (min v)",
	"35" = "PLL (min f v)",
	"41" = "XOSC (min f v)",
	"47" = "ROSC (min f v)",
	"53" = "PLL (baseline)",
	"59" = "XOSC (baseline)",
	"65" = "ROSC (baseline)"
)
clock_colors <- c(
	"PLL (min f)"   = "orange",
	"ROSC (min f)"  = "purple",
	"PLL (min v)"  = "brown",
	"XOSC (min v)" = "dark green",
	"ROSC (min v)" = "red",
	"PLL (min f v)" = "grey",
	"XOSC (min f v)" = "green",
	"ROSC (min f v)" = "orange",
	"PLL (baseline)" = "black",
	"ROSC (baseline)" = "dark blue",
	"XOSC (baseline)" = "blue"
)
freqs <- c(
	"PLL (min f)"    = 15429000, 
	"ROSC (min f)"   = 2902000, 
	"PLL (min v)"    = 150000000, 
	"XOSC (min v)"   = 12000000, 
	"ROSC (min v)"   = 10881000, 
	"PLL (min f v)"  = 15428000, 
	"XOSC (min f v)" = 12000000, 
	"ROSC (min f v)" = 2902000, 
	"PLL (baseline)" = 150000000, 
	"XOSC (baseline)" = 12000000,
	"ROSC (baseline)" = 10887000 
)
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 47

df <- df %>%
	filter(expe_num %in% c(6:11, 24:29, 42:47))

df_baseline <- read.csv("baseline.csv")
df_baseline$expe_num <- df_baseline$expe_num + max_expe_num+1 
df <- rbind(df, df_baseline)
df <- df %>%
	filter(expe_num %in% c(6:11, 24:29, 42:47, 60:65))
max_expe_num <- max_expe_num+18
df$expe_num[df$expe_num == 52] <- 53 # slight hack due to pll expes doing mat mut too fast yielding no record in the csv (to fix)
x_power_median <- 400
y_power_median_offset <- 2
x_clock_freq <- 300 
y_max <- 25
