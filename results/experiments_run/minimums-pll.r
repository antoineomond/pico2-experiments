name <- "minimums-pll"
confs <- c(
	"PLL (min f)",
	"ROSC (min f)",
	"PLL (min v)",
	"XOSC (min v)",
	"ROSC (min v)",
	"PLL (min f v)",
	"XOSC (min f v)",
	"ROSC (min f v)",
	"PLL (baseline)"
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
	"53" = "PLL (baseline)"
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
	"PLL (baseline)" = "black"
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
	"PLL (baseline)" = 150000000 
)
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 47

df <- df %>%
	filter(expe_num %in% c(0:5, 12:17, 30:35))

df_baseline <- read.csv("baseline.csv")
df_baseline$expe_num <- df_baseline$expe_num + max_expe_num+1 
df <- rbind(df, df_baseline)
df <- df %>% filter(expe_num < max_expe_num+6)
max_expe_num <- max_expe_num+6
df$expe_num[df$expe_num == 52] <- 53 # slight hack due to pll expes doing mat mut too fast yielding no record in the csv (to fix)
x_power_median <- 60
y_power_median_offset <- 5
y_energy_offset <- 20
x_clock_freq <- 33
y_max <- 85
