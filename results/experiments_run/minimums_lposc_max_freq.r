name <- paste(folder, "/minimums_lposc_max_freq", sep="")
confs <- c(
	"LPOSC 1.10V (baseline)",
	"LPOSC 1.00V",
	"LPOSC 0.90V",
	"LPOSC 0.80V"
)
names = c(
	"5"  = "LPOSC 1.10V (baseline)",
	"11" = "LPOSC 1.00V",
	"17" = "LPOSC 0.90V",
	"23" = "LPOSC 0.80V"
)
clock_colors <- c(
	"LPOSC 1.10V (baseline)"   = "black",
	"LPOSC 1.00V"  = "dark blue",
	"LPOSC 0.90V"  = "dark grey",
	"LPOSC 0.80V"  = "dark green"
)
freqs <- c(
	"LPOSC 1.10V (baseline)" = 33000,
	"LPOSC 1.00V"  = 34000,
	"LPOSC 0.90V"   = 35000,
	"LPOSC 0.80V"  = 36000
)
max_expe_num <- 23
df <- read.csv(paste(name, ".csv", sep=""))
x_power_median <- 400
y_power_median_offset <- 0.05
y_energy_offset <- 50
x_clock_freq <- 25 
y_max <- 25
