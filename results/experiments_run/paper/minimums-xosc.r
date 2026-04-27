name <- paste(folder, "minimums-xosc", sep="")
confs <- c(
	"XOSC (baseline)",
	"XOSC (1.00V)",
	"XOSC (0.90V)",
	"XOSC (0.80V) (lowest v)"
)
names = c(
	"5" = "XOSC (baseline)",
	"11" = "XOSC (1.00V)",
	"17" = "XOSC (0.90V)",
	"23" = "XOSC (0.80V) (lowest v)"
)
clock_colors <- c(
	"XOSC (baseline)" = "black",
	"XOSC (1.00V)" = "dark green",
	"XOSC (0.90V)" = "blue",
	"XOSC (0.80V) (lowest v)" = "orange"
)
freqs <- c(
	"XOSC (baseline)" = 12000000,
	"XOSC (1.00V)" = 12000000,
	"XOSC (0.90V)" = 12000000,
	"XOSC (0.80V) (lowest v)" = 12000000
)
max_expe_num <- 23
df <- read.csv(paste(name, ".csv", sep=""))
x_power_median <- 43
y_power_median_offset <- 5
y_energy_offset <- 20
x_clock_freq <- 20
y_max <- 85
clock_freq_unit <- "MHz"
