name <- "minimums-rosc"
confs <- c(
	"ROSC (baseline)",
	"ROSC (min f)",
	"ROSC (min v)",
	"ROSC (min f v)"
)
names = c(
	"5" = "ROSC (baseline)",
	"11"  = "ROSC (min f)",
	"17" = "ROSC (min v)",
	"23" = "ROSC (min f v)"
)
clock_colors <- c(
	"ROSC (baseline)" = "black",
	"ROSC (min f)"  = "purple",
	"ROSC (min v)" = "red",
	"ROSC (min f v)" = "orange"
)
freqs <- c(
	"ROSC (baseline)" = 10881000, 
	"ROSC (min f)"   =   2905000, 
	"ROSC (min v)"   =  10897000, 
	"ROSC (min f v)" =   2905000
)
max_expe_num <- 23
df <- read.csv(paste(name, ".csv", sep=""))
x_power_median <- 400
y_power_median_offset <- 2
x_clock_freq <- 300 
y_max <- 25
