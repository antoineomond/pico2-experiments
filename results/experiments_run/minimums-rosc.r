name <- paste(folder, "minimums-rosc", sep="")
confs <- c(
	"ROSC (baseline)",
	"ROSC (lowest f)",
	"ROSC (lowest v)",
	"ROSC (lowest f v)"
)
names = c(
	"5" = "ROSC (baseline)",
	"11"  = "ROSC (lowest f)",
	"17" = "ROSC (lowest v)",
	"23" = "ROSC (lowest f v)"
)
clock_colors <- c(
	"ROSC (baseline)" = "black",
	"ROSC (lowest f)"  = "purple",
	"ROSC (lowest v)" = "red",
	"ROSC (lowest f v)" = "orange"
)
freqs <- c(
	"ROSC (baseline)" = 11006000, 
	"ROSC (lowest f)"   =   2937000, 
	"ROSC (lowest v)"   =   3927000, 
	"ROSC (lowest f v)" =   1051000
)
max_expe_num <- 23
df <- read.csv(paste(name, ".csv", sep=""))
x_power_median <- 600
y_power_median_offset <- 0.5
y_energy_offset <- 50
x_clock_freq <- 350 
y_max <- 25
clock_freq_unit <- "MHz"
