name <- paste(folder, "baseline", sep="")
confs <- c("PLL", "XOSC", "ROSC", "LPOSC")
names = c(
	"5"  = "PLL",
	"11" = "XOSC",
	"17" = "ROSC"
)
clock_colors <- c(
	"PLL"   = "black",
	"XOSC"  = "purple",
	"ROSC"  = "brown",
	"LPOSC" = "dark green"
)
freqs <- c(
	"PLL"   = 150000000,
	"XOSC"  = 12000000,
	"ROSC"  = 11100000,
	"LPOSC" = 20000
)
df <- read.csv(paste(name, ".csv", sep=""))
max_expe_num <- 23
x_power_median <- 60
y_power_median_offset <- 2
y_energy_offset <- 20
x_clock_freq <- 33
y_max <- 85
