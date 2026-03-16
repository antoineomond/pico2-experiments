name <- "minimums-rosc"
confs <- c(
	"ROSC (min f)",
	"ROSC (min v)",
	"ROSC (min f v)"
)
names = c(
	"5"  = "ROSC (min f)",
	"11" = "ROSC (min v)",
	"17" = "ROSC (min f v)",
	"23" = "ROSC (baseline)"
)
clock_colors <- c(
	"ROSC (min f)"  = "purple",
	"ROSC (min v)" = "red",
	"ROSC (min f v)" = "orange",
	"ROSC (baseline)" = "dark blue"
)
freqs <- c(
	"ROSC (min f)"   =   2905000, 
	"ROSC (min v)"   =  10897000, 
	"ROSC (min f v)" =   2905000, 
	"ROSC (baseline)" = 10881000 
)
max_expe_num <- 17
df <- read.csv(paste(name, ".csv", sep=""))
#df_baseline <- read.csv("baseline.csv")
#df_baseline <- df_baseline %>%
#	filter(expe_num %in% c(12:17))
#df <- rbind(df, df_baseline)
#max_expe_num <- max_expe_num+6
x_power_median <- 400
y_power_median_offset <- 2
x_clock_freq <- 300 
y_max <- 25
